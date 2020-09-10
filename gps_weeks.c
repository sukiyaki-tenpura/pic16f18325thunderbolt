/* 
 * Copyright 2014-2020 JL1TTD Hidenori Ishii
 * 
 * File:   gps_weeks.c
 * Author: JL1TTD Hidenori Ishii
 *
 * Created on September 1, 2020, 0:28
 */

#include "gps_weeks.h"

typedef struct {
    uint16_t gps_week;
    uint16_t  year;
    uint8_t  month;     // 1 origin
    uint8_t  day;
} gps_week_t;

#define DAYSMONTH_SIZE 13
#define LEAPDAY_OFFSET 13
const int8_t days_of_month[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
                                 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }; // with leap day
//                                   1   2   3   4   5   6   7   8   9  10  11  12

//
const gps_week_t date_of_week[] = {
    {    0, 2038, 11, 21 },
    {    6, 2039, 1,  2 },
    {   58, 2040, 1,  1 },
    {   73, 2040, 4,  15 },
    // this week here
    {   74, 2020, 9,  6 },
    {   91, 2021, 1,  3 },
    {  143, 2022, 1,  2 },
    {  195, 2023, 1,  1 },
    {  248, 2024, 1,  7 },
    {  300, 2025, 1,  5 },
    {  352, 2026, 1,  4 },
    {  404, 2027, 1,  3 },
    {  456, 2028, 1,  2 },
    {  509, 2029, 1,  7 },
    {  561, 2030, 1,  6 },
    {  613, 2031, 1,  5 },
    {  665, 2032, 1,  4 },
    {  717, 2033, 1,  2 },
    {  769, 2034, 1,  1 },
    {  822, 2035, 1,  7 },
    {  874, 2036, 1,  6 },
    {  926, 2037, 1,  4 },
    {  978, 2038, 1,  3 },
};

//
bool primary_timing_changed = false;
bool date_changed = false;

////////////////////////////////////////////////////////////////////////////////
// GPS Parameters
uint32_t gps_time_of_week;
uint16_t gps_week_number;
int16_t  gps_utc_offset;
uint8_t  gps_flags;
uint8_t  gps_seconds;   // always calc utc
uint8_t  gps_minutes;
uint8_t  gps_hours;

////////////////////////////////////////////////////////////////////////////////
// Calculated gps Date
uint16_t gps_year;
uint8_t  gps_month;
uint8_t  gps_day;
int8_t  gps_day_of_week;   // 0:Sunday 1:Monday ...
int8_t  prev_gps_day_of_week = -1;

////////////////////////////////////////////////////////////////////////////////
uint32_t prev_gps_week_number = 0xffffffff;
gps_week_t base_Sunday = { 0, 3000, 1, 1 };
int8_t  weeks_offset = 0;  // elapsed weeks from base_Sunday
int16_t days_offset = 0;   // elapsed days  from base_Sunday
uint8_t leap_offset = 0;

static void normalizeDateTime() {
    if (gps_seconds > 59) {
        gps_seconds -= 60;
        gps_minutes++;
    }
    if (gps_minutes > 59) {
        gps_minutes -= 60;
        gps_hours++;
    }
    if (gps_hours > 23) {
        gps_hours -= 24;
        gps_day++;
        gps_day_of_week++;
        if (gps_day_of_week > 6) {
            gps_day_of_week = 0;
        }
        prev_gps_day_of_week = -1;
        date_changed = true;
    }
    if (gps_day > days_of_month[gps_month+leap_offset]) {
        gps_day -= days_of_month[gps_month+leap_offset];
        gps_month++; 
    }
    if (gps_month > 12) {
        gps_month -= 12;
        gps_year++;
    }
}

static inline void setJST() {
    if ((gps_flags & 0x01) == 0) {
        gps_seconds += gps_utc_offset;
    }
    gps_hours += 9;
}

void get_date_of_Sunday(void) {
    if (prev_gps_week_number != gps_week_number) {
        size_t size = sizeof(date_of_week)/sizeof(gps_week_t);
        for (int i=size-1; i>=0; i--) {
            if (gps_week_number >= date_of_week[i].gps_week) {
                base_Sunday = date_of_week[i];
                weeks_offset = gps_week_number - date_of_week[i].gps_week;
                days_offset = weeks_offset * 7;
                break;
            }
        }
        gps_year = base_Sunday.year;
        if ((gps_year % 100) != 0 && (gps_year % 4) == 0) {
            leap_offset = LEAPDAY_OFFSET;
        } else {
            leap_offset = 0;
        }
        prev_gps_week_number = gps_week_number;
        date_changed = true;
        prev_gps_day_of_week = -1;
    }
}
// get date from gps_week_number, gps_time_of_week
void get_date(void) {
    uint8_t m;
    int16_t days_off;
    int16_t days_remain;

    gps_day_of_week = gps_time_of_week / (60L * 60L * 24L);
    if (prev_gps_day_of_week != gps_day_of_week) {
        days_off = days_offset + gps_day_of_week + base_Sunday.day - 1;
        for (m=base_Sunday.month; m<DAYSMONTH_SIZE; m++) {
            days_remain = days_off;
            days_off -= days_of_month[m+leap_offset];
            if (days_off < 0 || m == 12) {
                gps_month = m;
                gps_day = days_remain + 1;
                break;
            }
        }
        // Check new year on December
        if (gps_month == 12 && gps_day > 31) {
            gps_month = 1;
            gps_year++;
            gps_day -= 31;
        }
        prev_gps_day_of_week = gps_day_of_week;
        date_changed = true;
    }
}
void set_gps_primary_timing(uint32_t tow, uint16_t week, int16_t utc_offset,
            uint8_t flags, uint8_t seconds, uint8_t minutes, uint8_t hours)
{
    gps_time_of_week = tow;
    gps_week_number = week - 1024;
    gps_utc_offset = utc_offset;
    gps_flags = flags;
    gps_seconds = seconds;
    gps_minutes = minutes;
    gps_hours  = hours;
    if (gps_flags & 0b0100) {
        prev_gps_week_number = 0xffffffff;
        prev_gps_day_of_week = -1;
    }    
    primary_timing_changed = true;
    get_date_of_Sunday();
    get_date();
    setJST();
    normalizeDateTime();
}
