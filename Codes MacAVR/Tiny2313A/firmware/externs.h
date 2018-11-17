/**************************************************************
 Target MCU & internal clock speed: ATtiny2313A @ 8Mhz
 Name    : externs.h (used by main.c)
 Author  : Insoo Kim (insoo@hotmail.com)
 Date    : Nov. 8, 2015
 UPDATE  : Nov. 8, 2015 (based on _74HC595_7seg.c)
 
 Description: 7 segment LED on ATtiny2313A cumstom board
 *****************************************************************/

#include <avr/io.h>

extern uint8_t doneINITeeprom;
extern volatile uint8_t gActMode;
extern volatile uint8_t gOpMode;

//notifier: 1 - tactile button is pressed the 1st time
//          0 - not yet pressed
extern volatile uint8_t firstPressed;
//tactile button counter as user selected menu
extern volatile uint8_t menuCnt;
//extern volatile int curWDTcnt, prevWDTcnt, lapseWDTcnt;
//counter: current WDT intrpt
extern volatile int curWDTcnt;
//counter: previous WDT intrpt
extern volatile int prevWDTcnt;
//counter: from previous to current WDT intrpt
extern volatile int lapseWDTcnt;
//alarm duration in minute
extern volatile uint8_t ALMIN;
//alarm duration in hours
extern volatile uint8_t ALHOUR;
//notifier: 1 - a valid menu is selected
//          0 - not yet pressed
extern volatile uint8_t MENUSEL;
//notifier: 1 - assigned alarm is accomplished
//          0 - not yet done
extern volatile uint8_t doneAlarm;

//minute passed from the beginning of the current alarm
extern volatile uint8_t minCnt;
//counter to advance hour counter
extern volatile uint8_t oneSixthiethHour;
//hours from the beginning of the system power-on
extern volatile uint8_t hourCnt;


extern uint8_t gNightMode;

//----------------------------------------------
//--- Pin assignments for ATtiny85
//----------------------------------------------
extern uint8_t swPin; // tactile switch pin, Active Low
extern uint8_t buzzPin; // buzzer pin positive
extern uint8_t _7segLowerPin; //common cathode 7seg GND of ones digit
extern uint8_t _7segHigherPin; //common cathode 7seg GND of tens digit

extern uint8_t _7segApin;
extern uint8_t _7segBpin;
extern uint8_t _7segCpin;
extern uint8_t _7segDpin;
extern uint8_t _7segEpin;
extern uint8_t _7segFpin;
extern uint8_t _7segGpin;
extern uint8_t _7segDPpin;

//======================
//extern uint8_t _7seg[12];
extern uint8_t _7segABC_Num[12];
extern uint8_t MCU7segPin[8];

//Define functions
//======================
void initSerial();
void delayLong();
unsigned char serialCheckRxComplete(void);
unsigned char serialCheckTxReady(void);
unsigned char serialRead(void);
void serialWrite(unsigned char DataOut);
void establishContact();

void USARTInit(unsigned int ubrr_value, uint8_t x2, uint8_t stopbits);
void USART_Transmit( unsigned char data );

uint8_t get7segByte_Num(uint8_t num);
void dispTwo7seg(uint8_t nightMode, uint8_t duration, uint8_t num);
void OnOne();
void OffOne();
void OnTen();
void OffTen();
void ioinit(void);

void initEEPROM();
void EEPROM_write(unsigned int ucAddress, unsigned char ucData);
unsigned char EEPROM_read(unsigned int ucAddress);

void beeleebeelee(uint8_t duration);
void alarmMin(uint8_t num);
void alarmPresetMenu();
void showProgress(uint8_t curNum, uint8_t finNum);
void proceedTime();
void countButton(uint8_t *cnt);
void enableAlarm();
void enableWDT(uint8_t);
void disableWDT();

/*---------------------------------------------------------------
 ---------------------------------------------------------------*/
