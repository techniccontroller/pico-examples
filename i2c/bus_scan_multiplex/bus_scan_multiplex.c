/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Sweep through all 7-bit I2C addresses on all ports of multiplexer, 
// to see if any slaves are present on the I2C bus. 

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

#define I2C_PORT0 i2c0
#define I2C_SDA0 12
#define I2C_SCL0 13

#define I2C_PORT1 i2c1
#define I2C_SDA1 14
#define I2C_SCL1 15


#define TCAADDR 0x70

void tcaselect(uint8_t i) {
  if (i > 7) return;

  int value = 1 << i;

  int ret = i2c_write_blocking(I2C_PORT0, TCAADDR, (uint8_t *) &value, 1, false);
  //int ret = i2c_write_timeout_us(I2C_PORT0, TCAADDR, (uint8_t *) &value, 1, true, 1000);
  printf("\ntcaselect: %d -> %d\n", i, ret);
}

// I2C reserves some addresses for special purposes. We exclude these from the scan.
// These are any addresses of the form 000 0xxx or 111 1xxx
bool reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

int main() {
    // Enable UART so we can print status output
    stdio_init_all();

    for(int i = 0; i < 20; i++) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
    
    i2c_init(I2C_PORT0, 100 * 1000);
    gpio_set_function(I2C_SDA0, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL0, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA0);
    gpio_pull_up(I2C_SCL0);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(I2C_SDA0, I2C_SCL0, GPIO_FUNC_I2C));

    while(true) {
        printf("Hello, world!\n");
        printf("\nI2C Bus Scan with Multiplexer\n");
        
        
        for (uint8_t t=0; t<8; t++) {
            printf("\nTCA Port #%02d\n", t);
            tcaselect(t);
            

            for (uint8_t addr = 0; addr<=127; addr++) {
                if (addr == TCAADDR) continue;
                //if (reserved_addr(addr)) continue;
                
                uint8_t rxvalue = 0;
                int ret = i2c_write_timeout_us(I2C_PORT0, addr, (uint8_t *) &rxvalue, 1, true, 1000);

                if (ret == PICO_ERROR_GENERIC || ret == PICO_ERROR_TIMEOUT) {
                    printf(".");
                }
                else {
                    printf("\nFound I2C 0x%02x\n", addr);
                }
            }
        }
        printf("\ndone");

        sleep_ms(2000);
    }
    
    return 0;
}
