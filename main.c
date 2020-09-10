/* 
 * Copyright 2014-2020 JL1TTD Hidenori Ishii
 * 
 * File:    main.c
 * Author: JL1TTD Hidenori Ishii
 *
 * Created on 2020/09/01, 0:28
 
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.4
        Device            :  PIC16F18325
        Driver Version    :  2.00
*/

#include "mcc_generated_files/mcc.h"
#include "switch_io.h"
#include "lcd_st7032.h"
#include "thunderbolt.h"

#include "i2c1_master_driver.h"
#include "display.h"

void main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    I2C1_Initialize();
    switch_io_initialize();

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    __delay_ms(80);
    lcd_hw_reset();
    lcd_init();

    lcd_clear();
    display_title();
    __delay_ms(2000);

    while (1) {
        switch_state_t switches = get_switches();
        if (switches.sw0_long_pressed) {
            lcd_contrast_reset();
        } else
        if (switches.sw0_pressed) {
            lcd_contrast_delta(1);
        }
        if (switches.sw2_long_pressed) {
            lcd_clear();
            __delay_ms(1000);
        } else
        if (switches.sw2_pressed) {
            page_change();
        }
        if (EUSART_is_rx_ready()) {
            parse_packet();
        }
        display();
    }
}
/**
 End of File
*/