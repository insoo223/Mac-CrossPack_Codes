/**************************************************************
 Target MCU & internal clock speed: ATtiny2313A @ 8Mhz
 Name    : defs.h (used by main.c)
 Author  : Insoo Kim (insoo@hotmail.com)
 Date    : Nov. 8, 2015
 UPDATE  : Nov. 8, 2015 (based on _74HC595_7seg.c)
 
 Description: 7 segment LED on ATtiny2313A cumstom board
*****************************************************************/

/*
#ifndef HIGH
#define HIGH 1
#endif

#ifndef LOW
#define LOW 0
#endif

 #ifndef halfSec
 #define halfSec 75
 #endif
 
*/
#ifndef ALARM_PROGRESS_DIV
#define ALARM_PROGRESS_DIV 6 //pieces of progress bar
#endif

#ifndef ALARM_PROGRESS_UNIT
#define ALARM_PROGRESS_UNIT (100/ALARM_PROGRESS_DIV) //size of each progress bar
#endif

#ifndef SINGLE_DIGIT_DELAY
#define SINGLE_DIGIT_DELAY 400 //[us]
#endif

#ifndef NIGHT_MODE_DELAY
#define NIGHT_MODE_DELAY 400 //[us]
#endif


#ifndef OFF_7SEG
#define OFF_7SEG 255
#endif


#ifndef DISP_DURATION
#define DISP_DURATION 100
#endif

#ifndef DISP_DURATION_LONG
#define DISP_DURATION_LONG 100
#endif

#ifndef NIGHTMODE_DURATION
#define NIGHTMODE_DURATION 3 //[ms]
#endif
/*---------------------------------------------------------------
 ---------------------------------------------------------------*/

