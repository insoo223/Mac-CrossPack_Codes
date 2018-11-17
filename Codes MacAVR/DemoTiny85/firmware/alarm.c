/**************************************************************
 Target MCU & internal clock speed: ATtiny13A @ 9.6Mhz
 Name    : Alarm.c
 Author  : Insoo Kim (insoo@hotmail.com)
 Date    : Nov. 4, 2015
 UPDATE  : Nov. 5, 2015 (based on PCintSleep.c)
 
 Description: alarm control
 How to upload hex to target MCU using usbtiny programmer:
 
 cd "/Users/insoo/Google Drive/Coding/Ard/AVR/DemoTiny13A/firmware"
 
 avrdude -c usbtiny -P usb -p attiny13 -U flash:w:main.hex:i
 
 HEX size[char]: 720 out of 1024
 
 Ref:
 ** Written for and tested with a custom board with ATtiny13A run on 9.6Mhz internal osc
 *****************************************************************/
//#define F_CPU 9600000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>

#define swPin   PB4     // tactile switch
#define buzzPin PB3     // 5V buzzer
#define ledPin  PB0     // 3mm yellow LED

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

//*** Function prototype
void alarmMin(uint8_t num);
void proceedTime();
void blinkLED(uint8_t num);
void countButton(uint8_t *cnt);
void enableAlarm();
void enableWDT(uint8_t);
void disableWDT();

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
                    blinkLED(1);
                    ALMIN=3;
                    MENUSEL=1;
                    break;
                case 2:
                    blinkLED(2);
                    ALMIN=5;
                    MENUSEL=1;
                    break;
                case 3:
                    blinkLED(3);
                    ALMIN=10;
                    MENUSEL=1;
                    break;
                case 4:
                    blinkLED(4);
                    ALMIN=20;
                    MENUSEL=1;
                    break;
                case 5:
                    blinkLED(5);
                    ALMIN=30;
                    MENUSEL=1;
                    break;
                case 6:
                    blinkLED(6);
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

//----------------------------------------------------------

int main(void) {
    //Setup IO pins and defaults
    // Set up PBn mode to output
    //DDRB  = 0x00; //1 = output, 0 = input
    DDRB |= _BV(DDB0); //output
    DDRB |= _BV(DDB3); //output
    DDRB &= ~_BV(DDB4); //input
    
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
        /*-----------------
         //beep it
         PORTB |= _BV(buzzPin); //on buzz
         _delay_ms(2);
         PORTB &= ~_BV(buzzPin); //off buzz
         _delay_ms(2);
         -----------------*/
        
        if (ALMIN == 0)
            sleep_mode();   // go to sleep and wait for interrupt...
        else
            alarmMin(ALMIN);
    }
}//main

//------------------------------------------------
void alarmMin (uint8_t num)
{
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
            blinkLED(1);
            prevMinCnt=minCnt;
        }
        //check whether alarm is done
        if (minCnt >= num)
        {
            //show alarm minutes just done
            blinkLED(num);
            
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
void blinkLED(uint8_t num)
{
    int i;
    
    for (i=0; i<num; i++)
    {
        PORTB = 1 << ledPin;
        _delay_ms(10);
        PORTB = 0 << ledPin;
        _delay_ms(10);
    }
}//blinkLED

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
