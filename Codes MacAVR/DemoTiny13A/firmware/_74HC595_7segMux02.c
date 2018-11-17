/**************************************************************
 Target MCU & internal clock speed: ATtiny13A @ 9.6Mhz
 Name    : _74HC595_7segMux
 Author  : Insoo Kim (insoo@hotmail.com)
 Date    : Nov. 6, 2015
 UPDATE  : Nov. 6, 2015 (based on _74HC595_7seg.c)
 
 Description: 7 segment LED on ATtiny13A cumstom board (5 by 7 cm)
 
 How to upload hex to target MCU using usbtiny programmer:
 
 On MacBook Pro 2015
 cd "/Users/insoo/Google Drive/Coding/Ard/AVR/DemoTiny13A/firmware"
 
 On MacBook 2009 White
 cd "/Users/admin/Box Sync/BoxElec/DIY Electronics/Arduino/AVR C/Mac CrossPack_Codes/Codes MacAVR/DemoTiny13A"
 
 avrdude -c usbtiny -P usb -p attiny13 -U flash:w:main.hex:i
 
 HEX size[char]: 978 out of 1024
 
 Ref:
 ** Written for and tested with a custom board with ATtiny13A run on 9.6Mhz internal osc
 *****************************************************************/
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define halfSec 75

#define latchPort PORTB
uint8_t latPin = 2; //latch
#define clockPort PORTB
uint8_t ckPin = 1; //clock
#define dataPort  PORTB
uint8_t datPin = 0;

#define ONES_GND ~0x01 // grounding of the Ones digit of common cathde 7 segment LED
#define TENS_GND ~0x02 // grounding of the Tens digit of common cathde 7 segment LED
#define TWO_DIGIT_GND ~0x03 // grounding of the Ones & Tens digit of common cathde 7 segment LED
#define OFF_7SEG 255

#define DS_low()  dataPort&=~_BV(datPin)
#define DS_high() dataPort|=_BV(datPin)
#define ST_CP_low()  latchPort&=~_BV(latPin)
#define ST_CP_high() latchPort|=_BV(latPin)
#define SH_CP_low()  clockPort&=~_BV(ckPin)
#define SH_CP_high() clockPort|=_BV(ckPin)

#define swPin   PB4     // tactile switch
#define buzzPin PB3     // 5V buzzer
#define DISP_DURATION 100

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
uint8_t minCnt;
//counter to advance hour counter
uint8_t oneSixthiethHour;
//hours from the beginning of the system power-on
uint8_t hourCnt;

//Define functions
//======================
void alarmMin(uint8_t num);
void proceedTime();
void countButton(uint8_t *cnt);
void enableAlarm();
void enableWDT(uint8_t);
void disableWDT();

//void dispTwo7seg(uint8_t duration, uint8_t num);
void dispTwo7seg(   uint8_t num);
void primitive7seg(uint8_t num, uint8_t unit);
void ioinit(void);
void shiftOut_INSOO(uint8_t datPin, uint8_t ckPin, uint8_t val);


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

//----------------------------------------------------------
// Watch-Dong Timer interrupt routine
ISR(WDT_vect)
{
    //Experimentally found 2000 is very accuratedly 1 minute as of Nov 5,2015
    if (curCnt125ms >= 2000)
    {
        curCnt125ms = 0;
        proceedTime();
    }
    
    lapseCnt125ms = curCnt125ms - prevCnt125ms;
    if (lapseCnt125ms > 32) // around 1 sec
    {
        //If a valide menu is selected then skip menu selection until the previously selected one is accomplished, and WDT routine keeps time.
        if (!MENUSEL)
        {
            switch (menuCnt)
            {
                case 1:
                    alarmMin(1);
                    ALMIN=3;
                    MENUSEL=1;
                    break;
                case 2:
                    alarmMin(2);
                    ALMIN=5;
                    MENUSEL=1;
                    break;
                case 3:
                    alarmMin(3);
                    ALMIN=10;
                    MENUSEL=1;
                    break;
                case 4:
                    alarmMin(4);
                    ALMIN=20;
                    MENUSEL=1;
                    break;
                case 5:
                    alarmMin(5);
                    ALMIN=30;
                    MENUSEL=1;
                    break;
                case 6:
                    alarmMin(6);
                    ALMIN=60;
                    MENUSEL=1;
                    break;
                default:
                    MENUSEL=0;
                    break;
            }//switch (menuCnt)
        }//if
    }//if
    
    curCnt125ms++;
}//ISR(WDT_vect)

//----------------------------------------------------------
ISR(PCINT0_vect)
{
    pinIntCnt++;
    
    //only take one edge either falling or rising
    if (pinIntCnt % 2 == 1)
    {
        //if swPin is pressed for the first time, enable alarm routine
        if (!firstPressed)
        {
            //------ Enable WDT ------
            //WDTCR |= (1<<WDP3); // 4s
            // 125 ms, 8 times per sec (expermentally, 2000 times/min)
            // 1 s, 1 time per sec (expermentally, 2000 times/min)
            WDTCR |= _BV(WDP1) || _BV(WDP2);
            //WDTCR |= _BV(WDP2); // 125 ms, 8 times per sec
            // Enable watchdog timer interrupts
            WDTCR |= _BV(WDTIE);
            
            firstPressed = 1;
        }//if (!firstPressed)
        
        if (firstPressed)
        {
            menuCnt++;
            prevCnt125ms = curCnt125ms;
            _delay_ms(30); //debounce
        }//if (firstPressed)
    }//if (pinIntCnt % 2 == 1)
}//ISR(PCINT0_vect)

//--------------------------------------------------
int main (void)
{
    
    ioinit(); //Setup IO pins and defaults
    
    // enable PC(Pin Change) interrupt
    GIMSK |= _BV(PCIE);  //Enable PC interrupt
    // Enable pin change interrupt for PB4
    PCMSK |= _BV(swPin); // PCINT4
    
    // enable global interrupt
    sei();
    
    // Use the Power Down sleep mode
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    
    while(1)
    {
        if (ALMIN == 0)
            sleep_mode();   // go to sleep and wait for interrupt...
        else
            alarmMin(ALMIN);
        
    }
}//main

//--------------------------------------------------
//void dispTwo7seg(uint8_t duration, uint8_t num)
void dispTwo7seg(uint8_t num)
{
    uint8_t tens, ones, i;
    
    tens = num/10;
    ones = num%10;
    
    //for (i=0; i<duration; i++)
    {
        //7 seg LED brightness control in the dark ambience
        if (num == OFF_7SEG)
            primitive7seg(_7seg[tens], ~TWO_DIGIT_GND); //don't care what num is
        else
        {
            primitive7seg(_7seg[tens], TENS_GND);
            primitive7seg(_7seg[ones], ONES_GND);
        }
    }
}//dispTwo7seg

//--------------------------------------------------
void primitive7seg(uint8_t num, uint8_t unit)
{
    ST_CP_low();
    shiftOut_INSOO(datPin, ckPin, unit);
    shiftOut_INSOO(datPin, ckPin, num);
    ST_CP_high();
    
}//primitive7seg

//--------------------------------------------------
void ioinit (void)
{
    //DDRB  = 0xff; //1 = output, 0 = input
    DDRB  = 0x00; //1 = output, 0 = input
    DDRB |= _BV(latPin); //output
    DDRB |= _BV(ckPin); //output
    DDRB |= _BV(datPin); //output
    
    DDRB |= _BV(buzzPin); //output
    DDRB &= ~_BV(swPin); //input
    
    
    PORTB = 0x00;
}//ioinit

//--------------------------------------------------
void shiftOut_INSOO(uint8_t myDataPin, uint8_t myClockPin, uint8_t val)
{
    uint8_t i;
    
    for (i = 0; i < 8; i++)
    {
        if ( !!(val & (1 << (7 - i))) == 1)
            PORTB |= _BV(myDataPin);
        else
            PORTB &= ~_BV(myDataPin);
        
        //digitalWrite(myClockPin, HIGH);
        PORTB |= _BV(myClockPin);
        //digitalWrite(myClockPin, LOW);
        PORTB &= ~_BV(myClockPin);
    }
}

//------------------------------------------------
void alarmMin (uint8_t num)
{
    uint8_t i;
    for (i=0; i<DISP_DURATION; i++)
    {
        dispTwo7seg( num);
        dispTwo7seg( OFF_7SEG);
        _delay_ms(2);
    }
    /***********************************
     uint8_t prevMinCnt;
     uint8_t i;
     
     //reset WDT counter to give zero-reference to alarm duration
     curCnt125ms = 0;
     prevCnt125ms = 0;
     minCnt = 0;
     prevMinCnt=minCnt;
     while (!doneAlarm)
     {
     if (prevMinCnt != minCnt)
     {
     dispTwo7seg(minCnt);
     prevMinCnt=minCnt;
     }
     //check whether alarm is done
     if (minCnt >= num)
     {
     //show alarm minutes just done
     dispTwo7seg(num);
     
     //bee-lee bee-lee bee-lee
     for (i=0; i<10; i++)
     {
     PORTB |= _BV(buzzPin); //on buzz
     _delay_ms(2);
     PORTB &= ~_BV(buzzPin); //off buzz
     _delay_ms(2);
     }
     
     doneAlarm = 1;
     }
     }
     
     ***********************************/
    //reset alarm duration
    ALMIN=0;
    //reset usr selected menu
    menuCnt = 0;
    //reset menu selection notification
    MENUSEL=0;
    //reset alarm finish notification
    doneAlarm=0;
    //reset wake up WDT interrupt sentinel
    firstPressed=0;
    //disable WDT and only wake up when the tactile button is pressed
    disableWDT();
}//alarmMin
//------------------------------------------------
void disableWDT()
{
    // Disable watchdog timer interrupts
    WDTCR &= ~(1<<WDTIE);
}//disableWDT

//------------------------------------------------
void proceedTime()
{
    minCnt++;
    oneSixthiethHour++;
    if ( (oneSixthiethHour != 0) && (oneSixthiethHour == 60) )
    {
        hourCnt++;
        //oneSixthiethHour is reset every hour
        oneSixthiethHour = 0;
    }
}//proceedTime
