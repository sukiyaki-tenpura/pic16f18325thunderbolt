/* 
 * Copyright 2014-2020 JL1TTD Hidenori Ishii
 * 
 * File:   thunderbolt.h
 * Author: JL1TTD Hidenori Ishii
 *
 * Created on August 18, 2020, 1:53 AM
 */

#ifndef UART_H
#define	UART_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define DLE 0x10
#define ETX 0x03

#define RX_STATE_IDLE      0
#define RX_STATE_DLE       1
#define RX_STATE_RECEIVE   2
#define RX_STATE_COMMAND   3
#define RX_STATE_SKIP      4
#define RX_STATE_GARBAGE   5

#define RX_WAIT_MS 1
#define TIMEOUT_MAX (100/RX_WAIT_MS)

/////////////////////////////////////////////////////////////////
// Auto report packet
#define XYZ_POS 0x42
#define FIX_XYZ 0x43
#define LLA_POS 0x4A
#define FIX_ENU 0x56
#define SYS_DAT 0x58
#define SATELITE 0x6D

#define TSIP    0x8F
#define TS_SOLUTION  0xA7
#define TS_P_TIMMING   0xAB
#define TS_S_TIMMING   0xAC


#ifdef	__cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////
// GPS Measurement value
// LLA
extern float gps_latitude;
extern float gps_longitude;
extern float gps_altitude;
extern bool   lla_changed;
// Satellite
extern uint8_t gps_fix_dim;
extern uint8_t gps_svs_fix_num;
extern bool    satellite_changed;
// secondary timing
extern float   gps_pps_offset;
extern float   gps_tenmega_offset;
extern bool    secondary_timing_changed;

void parse_packet(void);

#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */

