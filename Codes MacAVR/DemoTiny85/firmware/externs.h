#include <avr/io.h>

extern uint8_t swPin; // tactile switch
extern uint8_t latPin; //latch
extern uint8_t ckPin; //clock
extern uint8_t datPin;

extern uint8_t latch4021pin;
extern uint8_t clock4021pin;
extern uint8_t data4021pin;

//clock edge selector in PCINT intrp routine
extern volatile uint8_t pinIntCnt;
//notifier: 1 - tactile button is pressed the 1st time
//          0 - not yet pressed
extern volatile uint8_t firstPressed;
//tactile button counter as user selected menu
extern volatile uint8_t menuCnt;
//alarm duration in minute
extern volatile uint8_t ALMIN;
//notifier: 1 - a valid menu is selected
//          0 - not yet pressed
extern volatile uint8_t MENUSEL;
//notifier: 1 - assigned alarm is accomplished
//          0 - not yet done
extern volatile uint8_t doneAlarm;

//extern volatile int curCnt125ms, prevCnt125ms, lapseCnt125ms;
//counter: current WDT intrpt
extern volatile int curCnt125ms;
//counter: previous WDT intrpt
extern volatile int prevCnt125ms;
//counter: from previous to current WDT intrpt
extern volatile int lapseCnt125ms;

//minute passed from the beginning of the current alarm
extern volatile uint8_t minCnt;
//counter to advance hour counter
extern volatile uint8_t oneSixthiethHour;
//hours from the beginning of the system power-on
extern volatile uint8_t hourCnt;

extern uint8_t gCurTopX, gCurBotX;
extern uint8_t gNightMode;


//======================
extern uint8_t _7seg[12];

//Define functions
//======================
void  beeleebeelee(uint8_t duration);

void ctrlSingle(uint8_t ctrlID, uint8_t state);
void updateX(uint8_t topX, uint8_t botX);

void alarmMin(uint8_t num);
void proceedTime();
void countButton(uint8_t *cnt);
void enableAlarm();
void enableWDT(uint8_t);
void disableWDT();

void dispTwo7seg(uint8_t nightMode, uint8_t duration, uint8_t num);
//void dispTwo7seg(1, uint8_t num);
void ctrlAll(uint8_t num, uint8_t unit);
void ioinit(void);
void shiftOut_INSOO(uint8_t datPin, uint8_t ckPin, uint8_t val);
uint8_t shiftIn_INSOO(uint8_t myDataPin, uint8_t myClockPin);


