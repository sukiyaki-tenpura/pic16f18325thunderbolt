/* 
 * Copyright 2014-2020 JL1TTD Hidenori Ishii
 * 
 * File:   i2c1_master_driver.h
 * Author: JL1TTD Hidenori Ishii
 *
 * Created on August 16, 2020, 4:12 PM
 *
 *      Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.4
        Device            :  PIC16F18325
        Driver Version    :  1.0.0
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.20 and above or later
        MPLAB             :  MPLAB X 5.40
*/
#ifndef I2C1_MCC_LIB_H
#define	I2C1_MCC_LIB_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define DETER_I2C1_RX_FUNC

#ifdef	__cplusplus
extern "C" {
#endif

typedef enum {
    I2C1_NOERR, // The message was sent.
    I2C1_FAIL,   // Message was not sent, bus failure
    I2C1_BUSY  // Message was not sent, bus was busy.
} i2c1_error_t;

typedef uint8_t i2c1_address_t;

void I2C1_Initialize(void);

void MSSP1_BCLInterruptHandler(void);
void MSSP1_SSPInterruptHandler(void);

i2c1_error_t I2C1_ReadRegisterBytes(i2c1_address_t address, uint8_t reg, uint8_t* data, uint8_t len);
i2c1_error_t I2C1_WriteRegisterBytes(i2c1_address_t address, uint8_t reg, uint8_t* data, uint8_t len);


#ifdef	__cplusplus
}
#endif

#endif	/* I2C1_MCC_LIB_H */

