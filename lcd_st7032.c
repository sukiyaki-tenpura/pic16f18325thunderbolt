/* 
 * Copyright 2014-2020 JL1TTD Hidenori Ishii
 * 
 * File:   lcd_st7032.c
 * Author: JL1TTD Hidenori Ishii
 *
 * Created on 2017/03/28, 23:51
 */
#include "lcd_st7032.h"


////////////////////////////////////////////////////////////////////////////////
// AQM1602

uint8_t  ee_contrast = CONTRAST_DEFAULT;

void lcd_hw_reset() {
#ifdef CONTRAST_ADDR
    ee_contrast = eeprom_read(CONTRAST_ADDR);
    if (ee_contrast > CONTRAST_MAX || ee_contrast <= 10) {
        ee_contrast = CONTRAST_DEFAULT;
    }
#endif
#ifdef LCD_RESET
    LCD_RESET = 1;
    __delay_ms(2);
#endif
}

void lcd_command(uint8_t command) {
    uint8_t control_byte = 0x00;
    
    I2C1_WriteRegisterBytes(LCD_ADDR, control_byte, &command, 1);
    __delay_us(27);
}

void lcd_setpos(uint8_t column, uint8_t line) {
    lcd_command(0x80 | (line << 6) | column);        // line0 column0 = 0x00, line1 column0 = 0x40    
}
#ifndef DETER_LCD_FUNC
void lcd_putc(uint8_t c_data) {
    uint8_t control_byte = 0xC0;
    
    I2C1_WriteRegisterBytes(LCD_ADDR, control_byte, &c_data, 1);
    __delay_us(27);
}
#endif
void lcd_puts(uint8_t* s_data) {
    uint8_t control_byte = 0x40;
    uint8_t len;
    len = strlen((const char *)s_data);
    I2C1_WriteRegisterBytes(LCD_ADDR, control_byte, s_data, len);
    __delay_us(27);
}

void lcd_clear(void) {
    lcd_command(0x01);
    __delay_us(1100);
}
#ifndef DETER_LCD_FUNC
void lcd_homepos(void) {
    lcd_command(0x02);
    __delay_us(1100);
}
#endif
void lcd_contrast_reset(void) {
    lcd_contrast(CONTRAST_DEFAULT);
}
void lcd_contrast_delta(int8_t delta) {
    ee_contrast += delta;
    if (ee_contrast > CONTRAST_MAX) {
        ee_contrast = 0;
    }
    lcd_contrast(ee_contrast);
}
void lcd_contrast(uint8_t contrast) {
    lcd_command(0x39);
    lcd_command(0x70 | (contrast & 0x0F));
    lcd_command(0x50 | 0x04 | ((contrast >> 4) & 0x03));
    lcd_command(0x38);

    ee_contrast = contrast;
#ifdef CONTRAST_ADDR
    eeprom_write(CONTRAST_ADDR, contrast);
#endif
}

void lcd_init(void) {
    __delay_ms(40);
    lcd_command(0x38);
    lcd_command(0x39);
    lcd_command(0x14);
    lcd_command(0x70 | (ee_contrast & 0x0F));
    lcd_command(0x50 | 0x04 | ((ee_contrast >> 4) & 0x03));
    lcd_command(0x6C);
    __delay_ms(210);
    lcd_command(0x38);
    lcd_command(0x01);
    __delay_us(1100);
    lcd_command(0x0C);
    lcd_command(0x06);
}
