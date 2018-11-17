#include <avr/io.h>
//#include <util/delay.h>
//#include <avr/interrupt.h>
//#include <avr/sleep.h>
#include "defs.h"

//----------------------------------------------
//--- Pin assignments for ATtiny85
//----------------------------------------------
uint8_t swPin  = 4; // tactile switch
uint8_t latPin = 2; //latch of 74HC595s
uint8_t ckPin = 1; //clock of 74HC595s
uint8_t datPin = 0; //data of 74HC595s

uint8_t latch4021pin = 2;
uint8_t clock4021pin = 1;
uint8_t data4021pin = 3;

//clock edge selector in PCINT intrp routine
volatile uint8_t pinIntCnt=0;
//notifier: 1 - tactile button is pressed the 1st time
//          0 - not yet pressed
volatile uint8_t firstPressed=0;
//tactile button counter as user selected menu
volatile uint8_t menuCnt=0;
//alarm duration in minute
volatile uint8_t ALMIN=0;
//notifier: 1 - a valid menu is selected
//          0 - not yet pressed
volatile uint8_t MENUSEL=0;
//notifier: 1 - assigned alarm is accomplished
//          0 - not yet done
volatile uint8_t doneAlarm = 0;

//volatile int curCnt125ms=0, prevCnt125ms=0, lapseCnt125ms;
//counter: current WDT intrpt
volatile int curCnt125ms=0;
//counter: previous WDT intrpt
volatile int prevCnt125ms=0;
//counter: from previous to current WDT intrpt
volatile int lapseCnt125ms;

//minute passed from the beginning of the current alarm
volatile uint8_t minCnt;
//counter to advance hour counter
volatile uint8_t oneSixthiethHour;
//hours from the beginning of the system power-on
volatile uint8_t hourCnt;

uint8_t gCurTopX, gCurBotX;
uint8_t gNightMode;

//======================
uint8_t _7seg[12]= {
    //
    0b01111110, //0
    0b00000110, //1
    0b10111100, //2
    0b10101110, //3
    0b11000110, //4
    0b11101010, //5
    0b11011010, //6
    0b00100110, //7
    0b11111110, //8
    0b11100110, //9
    0b00000001, //DP (10)
    0b00000000 //Blank (11)
};
