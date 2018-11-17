/**************************************************************
 Target MCU & internal clock speed: ATtiny2313A @ 8Mhz
 Name    : externs.h (used by main.c)
 Author  : Insoo Kim (insoo@hotmail.com)
 Date    : Nov. 8, 2015
 UPDATE  : Nov. 8, 2015 (based on _74HC595_7seg.c)
 
 Description: 7 segment LED on ATtiny2313A cumstom board
 *****************************************************************/

#include <avr/io.h>

//----------------------------------------------
//--- Pin assignments for ATmega8L
//----------------------------------------------
extern uint8_t swPin; // tactile switch pin, Active Low
extern uint8_t buzzPin; // buzzer pin positive
extern uint8_t _7segDigit_K_Pin; //common cathode 7seg GND of Kilo(Thousand) digit
extern uint8_t _7segDigit_H_Pin; //common cathode 7seg GND of Hundreds digit
extern uint8_t _7segDigit_T_Pin; //common cathode 7seg GND of Tens digit
extern uint8_t _7segDigit_U_Pin; //common cathode 7seg GND of Unit digit
//enum _7segPin = {G, F, A, B, E, D, C, DP};
extern uint8_t _7segApin;
extern uint8_t _7segBpin;
extern uint8_t _7segCpin;
extern uint8_t _7segDpin;
extern uint8_t _7segEpin;
extern uint8_t _7segFpin;
extern uint8_t _7segGpin;
extern uint8_t _7segDPpin;

//======================
extern uint8_t _7segABC_Num[12];
extern uint8_t MCU7segPin[8];

extern uint8_t gNightMode;
extern uint8_t gSingleDigitDelay;
extern unsigned char prevMin;
//======================
//Define functions
//======================
//DS1307
void i2c_init(void);
unsigned char i2c_start(unsigned char address);
void i2c_start_wait(unsigned char address);
unsigned char i2c_rep_start(unsigned char address);
void i2c_stop(void);
unsigned char i2c_write( unsigned char data );
unsigned char i2c_write_reg(unsigned char devReg, unsigned char data );
unsigned char i2c_readAck(void);
unsigned char i2c_readNak(void);
unsigned char i2c_read_reg(unsigned char devReg );
unsigned char decToBcd(unsigned char val);
unsigned char bcdToDec(unsigned char val);
void setDS1307time(unsigned char second, unsigned char minute, unsigned char hour, unsigned char dayOfWeek, unsigned char dayOfMonth, unsigned char month, unsigned char year);
void readDS1307time(unsigned char *second,
                    unsigned char *minute,
                    unsigned char *hour,
                    unsigned char *dayOfWeek,
                    unsigned char *dayOfMonth,
                    unsigned char *month,
                    unsigned char *year);
int mainDS1307(void);
/*
void SetTime(char HH,char MM, char SS, char ampm);
char GetHH();
char GetMM();
char GetSS();

void SetDate(char DD,char MM, char YY);
char GetDD();
char GetMonth();
char GetYY();
*/

void DisplayDateTime();

int Read_RTC(char add);
int Write_RTC(char add,char data1);

//Main
void ioinit(void);

uint8_t get7segByte_Num(uint8_t num);
void dispClock7seg(uint8_t nightMode, unsigned int duration, unsigned int num);
void OnOne();
void OffOne();
void OnTen();
void OffTen();
void OnHundred();
void OffHundred();
void OnKilo();
void OffKilo();

/*---------------------------------------------------------------
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


//Define functions
//======================
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

 ---------------------------------------------------------------*/
