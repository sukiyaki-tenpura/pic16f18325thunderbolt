/* 
 * Copyright 2014-2020 JL1TTD Hidenori Ishii
 * 
 * File:   switch_io.c
 * Author: JL1TTD Hidenori Ishii
 *
 * Created on August 13, 2020, 3:45 PM
 */
#include "mcc_generated_files/mcc.h"
#include "switch_io.h"

static bool sw0_pending      = false;
static bool sw1_pending      = false;
static bool sw2_pending      = false;
static uint8_t sw0_pending_long_press = 0;
static uint8_t sw1_pending_long_press = 0;
static uint8_t sw2_pending_long_press = 0;

static switch_state_t switch_state = { 1,1,1, 0,0,0, 0,0,0 };

#if 0
// insert interrupt_manager.c 
        else if(PIE2bits.TMR4IE == 1 && PIR2bits.TMR4IF == 1)
        {
            TMR4_ISR();
        } 
#endif

static void sw_pending_interrupt(void);

void switch_io_initialize(void)
{
//    get_re1_state();
    //////////////// Set TMR4 to the options selected in the User Interface
    // 16MHz clock 62.5ns -> *4=250ns 62*64*16=15.872ms
    // PR4 62; 
    PR4 = 0x3E;
    // TMR4 0; 
    TMR4 = 0x00;
    // Clearing IF flag before enabling the interrupt.
    PIR2bits.TMR4IF = 0;
    // Enabling TMR4 interrupt.
    PIE2bits.TMR4IE = 1;
    // T4CKPS 1:64; T4OUTPS 1:16; TMR4ON off; 
    T4CON = 0x7B;
}
static inline void TMR4_Stop(void)
{
    T4CONbits.TMR4ON = 0;
}
static inline void TMR4_Restart(void)
{
    // Start the Timer by writing to TMRxON bit
    TMR4 = 0;
    T4CONbits.TMR4ON = 1;
}
void TMR4_ISR(void)
{
    // clear the TMR4 interrupt flag
    PIR2bits.TMR4IF = 0;

   sw_pending_interrupt();
}

// IOC interrupt handler
void sw0_ioc_interrupt(void)
{
    sw0_pending = true;
    TMR4_Restart();
}
void sw1_ioc_interrupt(void)
{
    sw1_pending = true;
    TMR4_Restart();
}
void sw2_ioc_interrupt(void)
{
    sw2_pending = true;
    TMR4_Restart();
}

static void sw_pending_interrupt(void)
{
    bool sw0_now = SW0_PORT;
    bool sw1_now = SW1_PORT;
    bool sw2_now = SW2_PORT;
   
    if (sw0_pending) {
        if (sw0_pending_long_press >= LONG_PRESS_COUNT) {
            switch_state.sw0_long_pressed = true;
            sw0_pending = false;
            sw0_pending_long_press = 0;
        } else
        if (sw0_now == RELEASE) {
            switch_state.sw0_pressed = true;
            sw0_pending = false;
            sw0_pending_long_press = 0;
        } else { // PRESS
            sw0_pending_long_press++;
        }
    }
    if (sw1_pending) {
        if (sw1_pending_long_press >= LONG_PRESS_COUNT) {
            switch_state.sw1_long_pressed = true;
            sw1_pending = false;
            sw1_pending_long_press = 0;
        } else
        if (sw1_now == RELEASE) {
            switch_state.sw1_pressed = true;
            sw1_pending = false;
            sw1_pending_long_press = 0;
        } else { // PRESS
            sw1_pending_long_press++;
        }
    }
    if (sw2_pending) {
        if (sw2_pending_long_press >= LONG_PRESS_COUNT) {
            switch_state.sw2_long_pressed = true;
            sw2_pending = false;
            sw2_pending_long_press = 0;
        } else 
        if (sw2_now == RELEASE) {
            switch_state.sw2_pressed = true;
            sw2_pending = false;
            sw2_pending_long_press = 0;
        } else { // PRESS
            sw2_pending_long_press++;
        }
    }
    // All switches released then Stop Timer 4
    if (sw0_pending && sw1_pending && sw2_pending) {
        TMR4_Stop();
    }
}
switch_state_t get_switches(void) {
    switch_state.sw0_now = SW0_PORT;
    switch_state.sw1_now = SW1_PORT;
    switch_state.sw2_now = SW2_PORT;
    switch_state_t state = switch_state;
    switch_state.sw0_pressed = false;
    switch_state.sw0_long_pressed = false;
    switch_state.sw1_pressed = false;
    switch_state.sw1_long_pressed = false;
    switch_state.sw2_pressed = false;
    switch_state.sw2_long_pressed = false;
    return state;
}
