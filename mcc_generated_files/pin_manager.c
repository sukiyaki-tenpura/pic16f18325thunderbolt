/**
  Generated Pin Manager File

  Company:
    Microchip Technology Inc.

  File Name:
    pin_manager.c

  Summary:
    This is the Pin Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for pin APIs for all pins selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.4
        Device            :  PIC16F18325
        Driver Version    :  2.11
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.20 and above
        MPLAB             :  MPLAB X 5.40

    Copyright (c) 2013 - 2015 released Microchip Technology Inc.  All rights reserved.
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include "pin_manager.h"

///// Added for original interrupt handler
#include "../switch_io.h"
//



//void (*IOCAF2_InterruptHandler)(void);
//void (*IOCAF4_InterruptHandler)(void);
//void (*IOCCF3_InterruptHandler)(void);


void PIN_MANAGER_Initialize(void)
{
    /**
    LATx registers
    */
    LATA = 0x00;
    LATC = 0x00;

    /**
    TRISx registers
    */
    TRISA = 0x37;
    TRISC = 0x2B;

    /**
    ANSELx registers
    */
    ANSELC = 0x00;
    ANSELA = 0x00;

    /**
    WPUx registers
    */
    WPUA = 0x17;
    WPUC = 0x0B;

    /**
    ODx registers
    */
    ODCONA = 0x00;
    ODCONC = 0x00;

    /**
    SLRCONx registers
    */
    SLRCONA = 0x37;
    SLRCONC = 0x3F;

    /**
    INLVLx registers
    */
    INLVLA = 0x0F;
    INLVLC = 0x08;


    /**
    IOCx registers 
    */
    //interrupt on change for group IOCAF - flag
    IOCAFbits.IOCAF2 = 0;
    //interrupt on change for group IOCAF - flag
    IOCAFbits.IOCAF4 = 0;
    //interrupt on change for group IOCAN - negative
    IOCANbits.IOCAN2 = 1;
    //interrupt on change for group IOCAN - negative
    IOCANbits.IOCAN4 = 1;
    //interrupt on change for group IOCAP - positive
    IOCAPbits.IOCAP2 = 0;
    //interrupt on change for group IOCAP - positive
    IOCAPbits.IOCAP4 = 0;
    //interrupt on change for group IOCCF - flag
    IOCCFbits.IOCCF3 = 0;
    //interrupt on change for group IOCCN - negative
    IOCCNbits.IOCCN3 = 1;
    //interrupt on change for group IOCCP - positive
    IOCCPbits.IOCCP3 = 0;



//    // register default IOC callback functions at runtime; use these methods to register a custom function
//    IOCAF2_SetInterruptHandler(IOCAF2_DefaultInterruptHandler);
//    IOCAF4_SetInterruptHandler(IOCAF4_DefaultInterruptHandler);
//    IOCCF3_SetInterruptHandler(IOCCF3_DefaultInterruptHandler);

    // Enable IOCI interrupt 
    PIE0bits.IOCIE = 1; 
    
	
    RC0PPS = 0x18;   //RC0->MSSP1:SCL1;    
    RXPPS = 0x15;   //RC5->EUSART:RX;    
    SSP1CLKPPS = 0x10;   //RC0->MSSP1:SCL1;    
    RC1PPS = 0x19;   //RC1->MSSP1:SDA1;    
    RC4PPS = 0x14;   //RC4->EUSART:TX;    
    SSP1DATPPS = 0x11;   //RC1->MSSP1:SDA1;    
}
  
void PIN_MANAGER_IOC(void)
{   
	// interrupt on change for pin IOCAF2
    if(IOCAFbits.IOCAF2 == 1)
    {
        IOCAF2_ISR();  
    }	
	// interrupt on change for pin IOCAF4
    if(IOCAFbits.IOCAF4 == 1)
    {
        IOCAF4_ISR();  
    }	
	// interrupt on change for pin IOCCF3
    if(IOCCFbits.IOCCF3 == 1)
    {
        IOCCF3_ISR();  
    }	
}

/**
   IOCAF2 Interrupt Service Routine
*/
void IOCAF2_ISR(void) {

    // Add custom IOCAF2 code
///// Remove redundant codes
//    // Call the interrupt handler for the callback registered at runtime
//    if(IOCAF2_InterruptHandler)
//    {
//        IOCAF2_InterruptHandler();
///// Added original handler
    sw0_ioc_interrupt();
    IOCAFbits.IOCAF2 = 0;
}
///// Remove redundant codes
///**
//  Allows selecting an interrupt handler for IOCAF2 at application runtime
//*/
//void IOCAF2_SetInterruptHandler(void (* InterruptHandler)(void)){
//    IOCAF2_InterruptHandler = InterruptHandler;
//}
//
///**
//  Default interrupt handler for IOCAF2
//*/
//void IOCAF2_DefaultInterruptHandler(void){
//    // add your IOCAF2 interrupt custom code
//    // or set custom function using IOCAF2_SetInterruptHandler()
//}

/**
   IOCAF4 Interrupt Service Routine
*/
void IOCAF4_ISR(void) {

    // Add custom IOCAF4 code
///// Remove redundant codes
//    // Call the interrupt handler for the callback registered at runtime
//    if(IOCAF4_InterruptHandler)
//    {
//        IOCAF4_InterruptHandler();
//}
///// Added original handler
    sw1_ioc_interrupt();
    IOCAFbits.IOCAF4 = 0;
}
///// Remove redundant codes
///**
//  Allows selecting an interrupt handler for IOCAF4 at application runtime
//*/
//void IOCAF4_SetInterruptHandler(void (* InterruptHandler)(void)){
//    IOCAF4_InterruptHandler = InterruptHandler;
//}
//
///**
//  Default interrupt handler for IOCAF4
//*/
//void IOCAF4_DefaultInterruptHandler(void){
//    // add your IOCAF4 interrupt custom code
//    // or set custom function using IOCAF4_SetInterruptHandler()
//}
//
/**
   IOCCF3 Interrupt Service Routine
*/
void IOCCF3_ISR(void) {

    // Add custom IOCCF3 code
///// Remove redundant codes
//    // Call the interrupt handler for the callback registered at runtime
//    if(IOCCF3_InterruptHandler)
//    {
//        IOCCF3_InterruptHandler();
//    }
    sw2_ioc_interrupt();
    IOCCFbits.IOCCF3 = 0;
}
///// Remove redundant codes
///**
//  Allows selecting an interrupt handler for IOCCF3 at application runtime
//*/
//void IOCCF3_SetInterruptHandler(void (* InterruptHandler)(void)){
//    IOCCF3_InterruptHandler = InterruptHandler;
//}
//
///**
//  Default interrupt handler for IOCCF3
//*/
//void IOCCF3_DefaultInterruptHandler(void){
//    // add your IOCCF3 interrupt custom code
//    // or set custom function using IOCCF3_SetInterruptHandler()
//}

/**
 End of File
*/