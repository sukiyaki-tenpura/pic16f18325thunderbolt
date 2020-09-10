/* 
 * Copyright 2014-2020 JL1TTD Hidenori Ishii
 * 
 * File:   display.h
 * Author: JL1TTD Hidenori Ishii
 *
 * Created on September 6, 2020, 12:41 AM
 */

#ifndef DISPLAY_H
#define	DISPLAY_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


#ifdef	__cplusplus
extern "C" {
#endif

void page_change(void);
void display_page0(void);
void display_page1(void);
void display_title(void);
void display(void);

#ifdef	__cplusplus
}
#endif

#endif	/* DISPLAY_H */
