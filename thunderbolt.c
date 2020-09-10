/* 
 * Copyright 2014-2020 JL1TTD Hidenori Ishii
 * 
 * File:   uart.c
 * Author: JL1TTD Hidenori Ishii
 *
 * Created on August 18, 2020, 1:53 AM
 */
#include <xc.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "mcc_generated_files/device_config.h"
#include "mcc_generated_files/eusart.h"

#include "thunderbolt.h"
#include "gps_weeks.h"

//////////////////////////////////////////////////////////////////////
// GPS Measurement value
// LLA
float gps_latitude;
float gps_longitude;
float gps_altitude;
bool   lla_changed = false;
// Satelite
uint8_t gps_fix_dim = 0;
uint8_t gps_svs_fix_num = 0;
bool    satellite_changed = false;
// secondary timming
float   gps_pps_offset;
float   gps_tenmega_offset;
bool    secondary_timing_changed = false;

typedef struct {
    uint8_t command;
    float   latitude;
    float   longitude;
    float   altitude;
    float   clock_bias;
    float   time_of_fix;
} single_lla_t;

typedef struct {
    uint8_t command;
    uint8_t fix_state;
    float   pdop;
    float   hdop;
    float   vdop;
    float   tdop;
    int8_t  sv_prn[16]; 
} satelite_t;

typedef struct {
    uint8_t command;
    uint8_t sub_command;
    uint32_t    time_of_week;
    uint16_t    week_number;
    int16_t     utc_offset;
    uint8_t     flags;
    uint8_t     seconds;
    uint8_t     minutes;
    uint8_t     hours;
    uint8_t     day_failed;
    uint8_t     month_failed;
    uint16_t    year_failed;
} primary_timing_t;

typedef struct {
    uint8_t command;
    uint8_t sub_command;
    uint8_t     rec_mode;
    uint8_t     disc_mode;
    uint8_t     surver_progress;
    uint32_t    holdover_sec;
    uint16_t    critical_alarm;
    uint16_t    minor_alarm;
    uint8_t     decode_state;
    uint8_t     disc_activity;
    uint8_t     spare_1;
    uint8_t     spare_2;
    float       pps_offset;
    float       tenmega_offset;
    uint32_t    dac_val;
    uint32_t    dac_vol;
    float       temperature;
    double      latitude;
    double      longitude;
    double      altitude;
    uint8_t     spare_area[8];
} secondary_timing_t;

volatile char rx_buffer[128];
volatile uint8_t rx_data;
volatile uint8_t rx_pos;

uint8_t rx_state = RX_STATE_IDLE;
bool timeout = false;
bool uart_error = false;
uint8_t timeout_count = 0;

void (*command_handler)(void);

static inline uint16_t swap16(uint16_t big) {
    uint16_t little;
    little = big << 8;
    little |= big >> 8;
    return little;
}
static inline uint32_t swap32(uint32_t big) {
    uint32_t little;
    little  = big                << 24;
    little |= (big & 0x0000FF00) <<  8;
    little |= (big & 0x00FF0000) >>  8;
    little |= big                >> 24;
    return little;
}
typedef union {
    float float_val;
    uint8_t inner[4];
} float_swap_t;
static inline float swapf(float big) {
    float_swap_t temp, little;
    temp.float_val = big;
    little.inner[0] = temp.inner[3];
    little.inner[1] = temp.inner[2];
    little.inner[2] = temp.inner[1];
    little.inner[3] = temp.inner[0];
    return little.float_val;
    
}

void report_single_lla(void) {
    single_lla_t* single_lla = (single_lla_t*)rx_buffer;
    gps_latitude = swapf(single_lla->latitude);
    gps_longitude = swapf(single_lla->longitude);
    gps_altitude = swapf(single_lla->altitude);
    lla_changed = true;
}

void report_satellite(void) {
    satelite_t* satelite = (satelite_t*)rx_buffer;
    uint8_t fix_dim = satelite->fix_state & 0x07;
    uint8_t svs_num = satelite->fix_state >> 3;
    uint8_t svs_fix_num = 0;
    for (uint8_t svs=0; svs<svs_num; svs++) {
        int8_t svs_prn = satelite->sv_prn[svs];
        if (svs_prn > 0) {
            svs_fix_num++;
        }
    }
    gps_fix_dim = fix_dim;
    gps_svs_fix_num = svs_fix_num;
    satellite_changed = true;
}

void report_primary_timing(void) {
    primary_timing_t* timing = (primary_timing_t*)rx_buffer;
    uint32_t time_of_week   = swap32(timing->time_of_week);
    uint16_t week_number    = swap16(timing->week_number);
    int16_t  utc_offset     = swap16(timing->utc_offset);
    uint8_t  flags          = timing->flags;
    uint8_t  seconds        = timing->seconds;
    uint8_t  minutes        = timing->minutes;
    uint8_t  hours          = timing->hours;

    set_gps_primary_timing(time_of_week, week_number, utc_offset, flags,
            seconds, minutes, hours);
}

void report_secondary_timing(void) {
    secondary_timing_t* timing = (secondary_timing_t*)rx_buffer;
    gps_pps_offset = swapf(timing->pps_offset);
    gps_tenmega_offset = swapf(timing->tenmega_offset);
    secondary_timing_changed = true;
    
}

static inline void flash_recive() {
    timeout = false;
    timeout_count = 0;
}
static inline bool wait_for_recive() {
    flash_recive();
    timeout = true;
    while (timeout_count++ < TIMEOUT_MAX) {
        if (EUSART_is_rx_ready()) {
            timeout = false;
            break;
        }
        __delay_ms(RX_WAIT_MS);
    }
    return !timeout;
}

static void parse_info_body() {
    uint8_t body_state = RX_STATE_RECEIVE;
    do {
        if (!wait_for_recive()) {
            rx_state = RX_STATE_GARBAGE;
            return;
        }
        rx_data = EUSART_Read();
        if (rx_data == DLE) {
            if (body_state == RX_STATE_DLE) {
                body_state = RX_STATE_RECEIVE;
                continue;
            }
            body_state = RX_STATE_DLE;
        } else 
        if (rx_data == ETX && body_state == RX_STATE_DLE) {
            rx_state = RX_STATE_COMMAND;
        }
        rx_buffer[rx_pos++] = rx_data;
    } while (rx_state == RX_STATE_RECEIVE);

    if (rx_state == RX_STATE_COMMAND) {
        command_handler();
        rx_state = RX_STATE_IDLE;
    }
}

// receive DLE wait for command ID
static void parse_info_command() {
    rx_state = RX_STATE_RECEIVE;
    rx_pos = 0;
    rx_buffer[rx_pos++] = rx_data; 
    switch (rx_data) {
    case LLA_POS:
        command_handler = report_single_lla;
        parse_info_body();
        break;
    case SATELITE:
        command_handler = report_satellite;
        parse_info_body();
        break;
    case TSIP:
        if (!wait_for_recive()) {
            rx_state = RX_STATE_GARBAGE;
            return;
        }
        rx_data = EUSART_Read();
        switch (rx_data) {
        case TS_P_TIMMING:
            rx_buffer[rx_pos++] = rx_data; 
            command_handler = report_primary_timing;
            parse_info_body();
            break;
        case TS_S_TIMMING:
            rx_buffer[rx_pos++] = rx_data; 
            command_handler = report_secondary_timing;
            parse_info_body();
            break;
        default:
            rx_pos = 0;
            rx_state = RX_STATE_SKIP;
            break;
        }
        break;
    default:
        rx_pos = 0;
        rx_state = RX_STATE_SKIP;
        break;
    }
}

void parse_packet() {
    rx_data = EUSART_Read();
    switch (rx_state) {
    case RX_STATE_IDLE:
        if (rx_data == DLE) {
            rx_state = RX_STATE_DLE;
        } else {
            rx_state = RX_STATE_SKIP;
        }
        break;
    case RX_STATE_DLE:
        if (rx_data == ETX) {
            rx_state = RX_STATE_IDLE;
        } else {
            parse_info_command();
        }
        break;
    case RX_STATE_GARBAGE:
    case RX_STATE_SKIP:
        if (rx_data == DLE) {
            if (!wait_for_recive()) {
                rx_state = RX_STATE_IDLE;
                return;
            }
            rx_data = EUSART_Read();
            if (rx_data == ETX) {
                rx_state = RX_STATE_IDLE;
            } else if (rx_data == DLE) {
                return;
            }
        }
        break;
    case RX_STATE_RECEIVE:
        rx_buffer[rx_pos++] = rx_data; 
        parse_info_body();
        break;
    default:
        rx_state = RX_STATE_SKIP;
        break;
    }
}
