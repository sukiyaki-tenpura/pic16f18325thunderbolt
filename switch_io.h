/* 
 * Copyright 2014-2020 JL1TTD Hidenori Ishii
 * 
 * File:   switch_io.h
 * Author: JL1TTD Hidenori Ishii
 *
 * Created on August 13, 2020, 3:45 PM
 */
#ifndef SWITCH_IO_H
#define	SWITCH_IO_H

#include <stdint.h>
#include <stdbool.h>

#define RELEASE     true
#define PRESS       false
#define SW0_PORT    I_SW0_PORT
#define SW1_PORT    I_SW1_PORT
#define SW2_PORT    I_SW2_PORT
// long press time 15.9ms * 127 = 2018ms
#define LONG_PRESS_COUNT 127
#ifdef	__cplusplus
extern "C" {
#endif

void switch_io_initialize(void);
void TMR4_ISR(void);

void sw0_ioc_interrupt(void);
void sw1_ioc_interrupt(void);
void sw2_ioc_interrupt(void);

typedef struct {
    unsigned sw0_now:1;
    unsigned sw1_now:1;
    unsigned sw2_now:1;
    unsigned sw0_pressed:1;
    unsigned sw1_pressed:1;
    unsigned sw2_pressed:1;
    unsigned sw0_long_pressed:1;
    unsigned sw1_long_pressed:1;
    unsigned sw2_long_pressed:1;
} switch_state_t;

switch_state_t get_switches(void);

#ifdef	__cplusplus
}
#endif

#endif	/* SWITCH_IO_H */

