/* 
 * Copyright 2014-2020 JL1TTD Hidenori Ishii
 * 
 * File:   lcd_st7032.h
 * Author: JL1TTD Hidenori Ishii
 *
 * Created on 2017/03/28, 23:51
 */
#ifndef LCD_ST7032_H
#define	LCD_ST7032_H

#include <xc.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "mcc_generated_files/device_config.h"
#include "i2c1_master_driver.h"

#define DETER_LCD_FUNC

#define LCD_RESET   LATCbits.LATC2
#define LCD_ADDR    0x3e

// 5V 21 3.3V 55
#define CONTRAST_ADDR       0
#define CONTRAST_DEFAULT    55
#define CONTRAST_MAX        127

#ifdef	__cplusplus
extern "C" {
#endif

void lcd_hw_reset(void);
void lcd_command(uint8_t command);
void lcd_homepos(void);
void lcd_setpos(uint8_t column, uint8_t line);
#ifndef DETER_LCD_FUNC
void lcd_putc(uint8_t c_data);
#endif
void lcd_puts(uint8_t* s_data);

void lcd_init(void);
void lcd_clear(void);
void lcd_contrast_delta(int8_t delta);
void lcd_contrast_reset(void);
void lcd_contrast(uint8_t contrast);

#ifdef	__cplusplus
}
#endif

#endif	/* LCD_ST7032_H */

