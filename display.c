/* 
 * Copyright 2014-2020 JL1TTD Hidenori Ishii
 * 
 * File:   display.c
 * Author: JL1TTD Hidenori Ishii
 *
 * Created on September 6, 2020, 12:41 AM
 */

#include <xc.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "thunderbolt.h"
#include "gps_weeks.h"

#include "lcd_st7032.h"
#include "display.h"

uint8_t line0[40];
uint8_t line1[40];
uint8_t line2[40];
uint8_t line3[40];

static inline uint32_t ten_power32(uint8_t digit) {
    uint32_t result = 1L;
    for (int i=0; i<digit; i++) {
        result *= 10L;
    }
    return result;
}
// uint8_t 2 digit pad blank selectable
static void set_digit99(uint8_t* buf, uint8_t num, bool pad_sp) {
    uint8_t digit;
    digit = num / 10;
    buf[0] = digit + '0';
    if (pad_sp && digit == 0) {
        buf[0] = ' ';
    }
    buf[1] = (num % 10) + '0';
}
// uint32_t len digit decimal convert
static void set_digits(uint8_t* buf, uint32_t num, uint8_t len) {
    uint8_t digit;
    uint32_t multi = ten_power32(len - 1);
    for (int i=0; i<len; i++) {
        digit = num / multi;
        buf[i] = digit + '0';
        num -= ((uint32_t)digit * multi);
        multi /= 10L;
    }
}

static void set_date_line(void)
{
                        //0123456789abcd    ef
    strcpy((char*)line0, "0000/00/00   \x1c  ");
    //        sprintf((char*)line0,"%04d/%02d/%02d   \x1c%d", 
    //                        gps_year, gps_month, gps_day, gps_svs_fix_num);
    set_digits(&line0[0], (uint32_t)gps_year, 4);
    set_digit99((uint8_t*)&line0[5], gps_month, false);
    set_digit99((uint8_t*)&line0[8], gps_day, false);
    set_digit99((uint8_t*)&line0[14], gps_svs_fix_num, true);
}
static void set_time_line(void)
{
    uint8_t integer;
    uint8_t decimal;
    float temp_decimal;
    float abs_value;
                        //0123456789    abcdef
    strcpy((char*)line1, "00:00:00 \x15+  .00");
//    sprintf((char*)line1,"%02d:%02d:%02d  \x16%+02.2f",
//                    gps_hours, gps_minutes, gps_seconds, integer, gps_tenmega_offset);
    set_digit99((uint8_t*)&line1[0], gps_hours, false);
    set_digit99((uint8_t*)&line1[3], gps_minutes, false);
    set_digit99((uint8_t*)&line1[6], gps_seconds, false);
  
    uint32_t integer32;
    uint32_t decimal32;
    if (gps_tenmega_offset < 0.0) {
        line1[10] = '-';
        abs_value = gps_tenmega_offset * (-1.0);
    } else {
        abs_value = gps_tenmega_offset;
    }    
    integer32 = (uint32_t)abs_value;
    temp_decimal = abs_value - (float)integer32;
    decimal32 = (uint32_t)(temp_decimal * 100.0);

    set_digits((uint8_t*)&line1[11], integer32, 2);
    set_digits((uint8_t*)&line1[14], decimal32, 2);
}

static void set_lla_line(void)
{
    uint8_t integer8;
    uint32_t integer32;
    uint32_t decimal32;
    float temp_decimal;
    float abs_value;

    ///////// latitude
                        //0123456789abcdef 
    strcpy((char*)line2, "+99.99999 999.9m");
    if (gps_altitude < 0.0) {
        line2[0] = '-';
        abs_value = gps_latitude * (-1.0);
    } else {
        abs_value = gps_latitude;
    }    
    integer8= (uint8_t)abs_value;
    temp_decimal = abs_value - (float)integer32;
    decimal32 = (uint32_t)(temp_decimal * 100000.0);
//  sprintf((char*)line2,"%+02d.%05d", integer16, decimal32);
    set_digit99((uint8_t*)&line2[1], integer8, false);
    set_digits((uint8_t*)&line2[4], decimal32, 5);


    ///////// altitude
    integer32= (uint32_t)gps_altitude;
    temp_decimal = gps_altitude - (float)integer32;
    decimal32 = (uint32_t)(temp_decimal * 10.0);
//  sprintf((char*)&line2[10],"%03d.%01d", integer16, decimal32);
    if (integer32 > 999) {
        integer32 = 999;
    }
    set_digits((uint8_t*)&line2[10], integer32, 3);
    set_digits((uint8_t*)&line2[14], decimal32, 1);

    ///////// longitude
                        //0123456789abcdef 
    strcpy((char*)line3, "+999.99999      ");
    if (gps_longitude < 0.0) {
        line3[0] = '-';
        abs_value = gps_longitude * (-1.0);
    } else {
        abs_value = gps_longitude;
    }    
    integer32= (uint32_t)abs_value;
    temp_decimal = abs_value - (float)integer32;
    decimal32 = (uint32_t)(temp_decimal * 100000.0);
//  sprintf((char*)line3,"%+03d.%05d", integer32, decimal32);
    set_digits((uint8_t*)&line3[1], integer32, 3);
    set_digits((uint8_t*)&line3[5], decimal32, 5);
}

static uint8_t current_page = 0;

void display_page0(void) {
    if (date_changed || satellite_changed) {
        date_changed = satellite_changed = false;
        set_date_line();
        lcd_setpos(0, 0);
        lcd_puts(line0);
    }
    if (satellite_changed || (primary_timing_changed && secondary_timing_changed)) {
        satellite_changed = primary_timing_changed = secondary_timing_changed = false;
        set_time_line();
        lcd_setpos(0, 1);
        lcd_puts(line1);
    }
}
void display_page1(void) {
    if (lla_changed) {
        lla_changed = false;
        set_lla_line();
        lcd_setpos(0, 0);
        lcd_puts(line2);
        lcd_setpos(0, 1);
        lcd_puts(line3);
    }
}
void display_title(void) {
    lcd_setpos(0,0);
    lcd_puts((uint8_t*)"ThunderBolt\x10 GPS");
    lcd_setpos(0,1);
    //                 "0123456789abcdef"
    //                 " $#$ JL1TTD $#$ " 
    lcd_puts((uint8_t*)" \x1d\x1c\x1d JL1TTD \x1d\x1c\x1d ");
}
// 0123456789abcdef
// 2020/09/06|||$16
// 01:13:36||&99.90
void display(void) {
    if (current_page == 0) {
        display_page0();
    } else
    if (current_page == 1) {
        display_page1();
    } else {
        display_title();
    }
        
}
void page_change(void) {
    current_page++;
    if (current_page > 2) {
        current_page = 0;
    }
}
