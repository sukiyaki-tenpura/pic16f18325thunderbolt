/* 
 * Copyright 2014-2020 JL1TTD Hidenori Ishii
 * 
 * File:   gps_weeks.h
 * Author: JL1TTD Hidenori Ishii
 *
 * Created on September 1, 2020, 0:28
 */
#ifndef GPSWEEK_H
#define	GPSWEEK_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
// GPS Parameters
extern uint32_t gps_time_of_week;
extern uint16_t gps_week_number;
extern int16_t  gps_utc_offset;
extern uint8_t  gps_flags;
extern uint8_t  gps_seconds;   // always calc utc
extern uint8_t  gps_minutes;
extern uint8_t  gps_hours;

////////////////////////////////////////////////////////////////////////////////
// Calcurated gps Date
extern uint16_t gps_year;
extern uint8_t  gps_month;
extern uint8_t  gps_day;
extern int8_t  gps_day_of_week;   // 0:Sunday 1:Monday ...6:Saturday

extern bool primary_timing_changed;
extern bool date_changed;

void set_gps_primary_timing(uint32_t tow, uint16_t week, int16_t utc_offset,
        uint8_t flags, uint8_t seconds, uint8_t minutes, uint8_t hours);
void get_date_of_Sunday(void);
void get_date(void);

#ifdef	__cplusplus
}
#endif

#endif	/* GPSWEEK_H */

