/**************************************************************
 Target MCU & internal clock speed: ATtiny2313A @ 8Mhz
 Name    : defs.h (used by main.c)
 Author  : Insoo Kim (insoo@hotmail.com)
 Date    : Nov. 8, 2015
 UPDATE  : Nov. 8, 2015 (based on _74HC595_7seg.c)
 
 Description: 7 segment LED on ATtiny2313A cumstom board
*****************************************************************/


#ifndef TW_STATUS_insoo
#define TW_STATUS_insoo (TWSR & 0xF8)
#endif

/*
#ifndef F_CPU
#define F_CPU 1000000
#endif
*/
#ifndef DISP_DATE_DURATION
#define DISP_DATE_DURATION 500
#endif

#ifndef DISP_DAY_DURATION
#define DISP_DAY_DURATION 1000
#endif
#ifndef DISP_CLK_DURATION
#define DISP_CLK_DURATION 5000
#endif


#ifndef DISP_DURATION
#define DISP_DURATION 500
#endif

#ifndef UART_BAUD_RATE
#define UART_BAUD_RATE 2400
#endif

#ifndef SINGLE_DIGIT_DELAY
#define SINGLE_DIGIT_DELAY 5 //[ms]
#endif

#ifndef NIGHTMODE_DURATION
#define NIGHTMODE_DURATION 3 //[ms]
#endif

#ifndef NIGHT_MODE_DELAY
#define NIGHT_MODE_DELAY 3 //[ms]
#endif

/*---------------------------------------------------------------
 #ifndef ALARM_PROGRESS_DIV
 #define ALARM_PROGRESS_DIV 6 //pieces of progress bar
 #endif
 
 #ifndef ALARM_PROGRESS_UNIT
 #define ALARM_PROGRESS_UNIT (100/ALARM_PROGRESS_DIV) //size of each progress bar
 #endif
 
 
 #ifndef OFF_7SEG
 #define OFF_7SEG 255
 #endif
 
 #ifndef DISP_DURATION_LONG
 #define DISP_DURATION_LONG 100
 #endif
 
 ---------------------------------------------------------------*/

