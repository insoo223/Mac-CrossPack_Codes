/**************************************************************
 Target MCU & internal clock speed: ATtiny2313A @ 8Mhz
 Name    : globals.c (used by main.c)
 Author  : Insoo Kim (insoo@hotmail.com)
 Date    : Nov. 8, 2015
 UPDATE  : Nov. 8, 2015 (based on _74HC595_7seg.c)
 
 Description: 7 segment LED on ATtiny2313A cumstom board
 *****************************************************************/

#include <avr/io.h>
#include "defs.h"

//----------------------------------------------
//--- Pin assignments for ATmega8L
//----------------------------------------------
uint8_t swPin; // tactile switch pin, Active Low
uint8_t buzzPin; // buzzer pin positive
uint8_t _7segDigit_K_Pin; //common cathode 7seg GND of Kilo(Thousand) digit
uint8_t _7segDigit_H_Pin; //common cathode 7seg GND of Hundreds digit
uint8_t _7segDigit_T_Pin; //common cathode 7seg GND of Tens digit
uint8_t _7segDigit_U_Pin; //common cathode 7seg GND of Unit digit
//enum _7segPin = {G, F, A, B, E, D, C, DP};
uint8_t _7segApin;
uint8_t _7segBpin;
uint8_t _7segCpin;
uint8_t _7segDpin;
uint8_t _7segEpin;
uint8_t _7segFpin;
uint8_t _7segGpin;
uint8_t _7segDPpin;

//======================
// A to G & DP pin assignments for common cathode 7 segment LED
uint8_t _7segABC_Num[12] = {
    // dpGFEDCBA
    0b00111111, //0
    0b00000110, //1
    0b01011011, //2
    0b01001111, //3
    0b01100110, //4
    0b01101101, //5
    0b01111100, //6
    0b00000111, //7
    0b01111111, //8
    0b01100111, //9
    0b00000000, //Blank (10)
    0b10000000 //DP (11)
}; //_7segABC_Num[11]


// Variable depending on schematic
// MCU pin assignments to 7 seg,
//  beginning A to G & last one is for DP
uint8_t MCU7segPin[8] = {0, 1, 2, 3, 4, 5, 6, 7};  // ATmega8L pin

uint8_t gNightMode;
uint8_t gSingleDigitDelay;
unsigned char prevMin;
/*---------------------------------------------------------------
uint8_t doneINITeeprom=0;
volatile uint8_t gActMode=0;
volatile uint8_t gOpMode=0;
//notifier: 1 - tactile button is pressed the 1st time
//          0 - not yet pressed
volatile uint8_t firstPressed=0;
//tactile button counter as user selected menu
volatile uint8_t menuCnt=-1;
//volatile int curWDTcnt=0, prevWDTcnt=0, lapseWDTcnt;
//counter: current WDT intrpt
volatile int curWDTcnt=0;
//counter: previous WDT intrpt
volatile int prevWDTcnt=0;
//counter: from previous to current WDT intrpt
volatile int lapseWDTcnt;
//alarm duration in minutes
volatile uint8_t ALMIN=0;
//alarm duration in hours
volatile uint8_t ALHOUR=0;
//notifier: 1 - a valid menu is selected
//          0 - not yet pressed
volatile uint8_t MENUSEL=0;
//notifier: 1 - assigned alarm is accomplished
//          0 - not yet done
volatile uint8_t doneAlarm = 0;


//minute passed from the beginning of the current alarm
volatile uint8_t minCnt;
//counter to advance hour counter
volatile uint8_t oneSixthiethHour;
//hours from the beginning of the system power-on
volatile uint8_t hourCnt;

uint8_t gNightMode;


 ---------------------------------------------------------------*/

