/* 
 * Copyright 2014-2020 JL1TTD Hidenori Ishii
 * 
 * File:   i2c1_master_driver.c
 * Author: JL1TTD Hidenori Ishii
 *
 * Created on 2020/03/28, 23:51
 *
 *      Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.4
        Device            :  PIC16F18325
        Driver Version    :  1.0.0
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.20 and above or later
        MPLAB             :  MPLAB X 5.40
*/
#include <xc.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "mcc_generated_files/device_config.h"
#include "i2c1_master_driver.h"

#define BUS_COLLISION_RETRY 15

bool AckWait;
bool Collision;

void MSSP1_BCLInterruptHandler(void) {
    Collision = true;
    PIR1bits.BCL1IF = 0;
}
void MSSP1_SSPInterruptHandler(void) {
    AckWait = false;
    PIR1bits.SSP1IF = 0;
}

void I2C1_Initialize(void) {
    SSP1STAT = 0x00;
    SSP1CON1 = 0x08;
    SSP1CON2 = 0x00;
    SSP1CON3 = 0x00;
    SSP1ADD  = 0x27;
    SSP1CON1bits.SSPEN = 1;
    PIE1bits.SSP1IE = 1;
    PIE1bits.BCL1IE = 1;
    PIR1bits.SSP1IF = 0;
    PIR1bits.BCL1IF = 0;
}

static inline void I2C1_MasterStart(void) {
    SSP1CON2bits.SEN = 1;
}
#ifndef DETER_I2C1_RX_FUNC
static inline void I2C1_MasterRepeatStart(void) {
    SSP1CON2bits.RSEN = 1;
}
#endif
static inline void I2C1_MasterStop(void) {
    SSP1CON2bits.PEN = 1;
}
static inline void I2C1_WaitForIdle(void) {
    while((SSP1CON2 & 0b00011111) | (SSP1STAT & 0b00000101)) {
        __delay_us(40);
    }
}
#ifndef DETER_I2C1_RX_FUNC
static inline void I2C1_WaitForReceive(void) {
    while((SSP1CON2 & 0b00011111) | (SSP1STAT & 0b00000100)) {
        __delay_us(40);
    }
}
#endif
static inline void I2C1_MasterWaitForAck(void) {
    while (AckWait) {
        __delay_us(40);
    }
}
static inline void I2C1_MasterSendTxData(uint8_t data) {
    SSP1BUF  = data;
}
#ifndef DETER_I2C1_RX_FUNC
static inline void I2C1_MasterStartRx(void) {
    SSP1CON2bits.RCEN = 1;
}
static inline uint8_t I2C1_MasterGetRxData(void) {
    return SSP1BUF;
}
static inline void I2C1_MasterSendAck(bool Nack) {
    SSP1CON2bits.ACKDT = Nack;
    SSP1CON2bits.ACKEN = 1;
}
#endif

i2c1_error_t I2C1_Open(i2c1_address_t address, uint8_t read) {
    Collision = false;
    // send start bit
    I2C1_WaitForIdle();
    I2C1_MasterStart();
    I2C1_WaitForIdle();
    if (Collision) {
        return I2C1_BUSY;
    }
    // send address
    AckWait = true;
    I2C1_MasterSendTxData((address << 1) | read);
    I2C1_MasterWaitForAck();
    if (Collision) {
        return I2C1_BUSY;
    }
    return SSP1CON2bits.ACKSTAT;
}

#ifndef DETER_I2C1_RX_FUNC
i2c1_error_t I2C1_RpeatOpen(i2c1_address_t address, uint8_t read) {
    Collision = false;
    // send repeat start bit
    I2C1_WaitForIdle();
    I2C1_MasterRepeatStart();
    I2C1_WaitForIdle();
    if (Collision) {
        return I2C1_BUSY;
    }
    // send address
    AckWait = true;
    I2C1_MasterSendTxData((address << 1) | read);
    I2C1_MasterWaitForAck();
    if (Collision) {
        return I2C1_BUSY;
    }
    return SSP1CON2bits.ACKSTAT;
}
#endif
i2c1_error_t I2C1_Stop() {
    Collision = false;
    // send stop bit
    I2C1_WaitForIdle();
    I2C1_MasterStop();
    if (Collision) {
        return I2C1_BUSY;
    }
    return I2C1_NOERR;
}
i2c1_error_t I2C1_WriteData(uint8_t data) {
    Collision = false;
    I2C1_WaitForIdle();
    if (Collision) {
        return I2C1_BUSY;
    }
    // send data
    AckWait = true;
    I2C1_MasterSendTxData(data);
    I2C1_MasterWaitForAck();
    if (Collision) {
        return I2C1_BUSY;
    }
    return SSP1CON2bits.ACKSTAT;
}
#ifndef DETER_I2C1_RX_FUNC
uint8_t I2C1_ReadData(bool Nack) {
    uint8_t data;

    Collision = false;
    I2C1_WaitForIdle();
    if (Collision) {
        return I2C1_BUSY;
    }
    I2C1_MasterStartRx();
    I2C1_WaitForReceive();
    if (Collision) {
        return I2C1_BUSY;
    }
    data = I2C1_MasterGetRxData();
    I2C1_WaitForIdle();
    if (Collision) {
        return I2C1_BUSY;
    }
     I2C1_MasterSendAck(Nack);
     return data;
}

i2c1_error_t I2C1_ReadRegisterBytes(i2c1_address_t address, uint8_t reg, uint8_t* data, uint8_t len) {
    i2c1_error_t result;

    for (uint8_t retry=0 ; retry<BUS_COLLISION_RETRY ; retry++) {
        result = I2C1_Open(address, false);
        if (result == I2C1_NOERR) {
            I2C1_WriteData(reg);
            result = I2C1_RpeatOpen(address, true);
            if (result == I2C1_NOERR) {
                for (uint8_t i=1 ; i<=len ; i++) {
                    *data = I2C1_ReadData(i==len);
                    data++;
                }
            }
        }
        I2C1_Stop();
        if (result !=I2C1_BUSY) {
            break;
        } else {
            __delay_ms(100);
        }
    }
    return result;
}
#endif

i2c1_error_t I2C1_WriteRegisterBytes(i2c1_address_t address, uint8_t reg, uint8_t* data, uint8_t len) {
    i2c1_error_t result;

    for (uint8_t retry=0 ; retry<BUS_COLLISION_RETRY ; retry++) {
        result = I2C1_Open(address, false);
        if (result == I2C1_NOERR) {
            I2C1_WriteData(reg);
            for (uint8_t i=0 ; i<len ; i++) {
                I2C1_WriteData(*data++);
            }
        }
        I2C1_Stop();
        if (result !=I2C1_BUSY) {
            break;
        } else {
            __delay_ms(100);
        }
    }
    return result;
}
