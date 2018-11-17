/**************************************************************
 Target MCU & internal clock speed: ATtiny13A @ 9.6Mhz
 Name    : RF_PCint_FanMosq_WDT.c
 Author  : Insoo Kim (insoo@hotmail.com)
 Date    : April 11, 2015
 UPDATE  : Sep. 11, 2015 (based on RF_PCint_FanMosquito.c)
 
 Description: Interrupt driven RF for time control of fan and mosquito vaporizer
 1) default mode is for mosquito vaporizer
 on for 10 min, off for 10 min for the 1st two hours
 then off 20 & 30 min for the following two hours each, on time remains same.
 
 2) RF 315Mhz remote control button
 A: FAN mode
 B: mosquito vaporizer mode
 C: on
 D: off
 
 
 HEX size[char]: 622 out of 1024
 
 Ref:
 ** Written for and tested with a custom board with ATtiny13A run on 9.6Mhz internal osc
 *****************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>

#define halfSec 75

#define latchPort PORTB
#define latchPin  PB0 //latch
#define clockPort PORTB
#define clockPin  PB1 //clock
#define dataPort  PORTB
#define dataPin   PB2 //data

#define relayPin  PB3 //relay

#define latchLow()  latchPort&=~_BV(latchPin)
#define latchHi() latchPort|=_BV(latchPin)
#define clockLow()  clockPort&=~_BV(clockPin)
#define clockHi() clockPort|=_BV(clockPin)

#define FAN 1
#define MOSQUITO_BURNER 2
// power off hours for safety
#define safetyPwrOff 8 //after 8 hrs of run-time, the system will be off.

//mode of operation: 1 - Fan, 2 - mosquito repellent vaporizer
char modeOP = MOSQUITO_BURNER; //FAN MOSQUITO_BURNER

char ONintervalMin;
char OFFintervalMin;

uint8_t cnt8sec=0;

char secCnt=0;
char loopCnt=0;
char minCnt=0, oneSixthiethHour=0;
char hourCnt=0;



//Define functions
//======================
void ioinit(void);
void readRF(char*);
void relayHigh(void);
void relayLow(void);
void proceedTime(void);
void timedPowerCtrl(void);
//======================

//------------------------------------------------
ISR(PCINT0_vect)
{
    char val;
    
    readRF(&val);
    
    if (val & 0x09) // Button A
    {
        modeOP = FAN;
        PORTB ^= 1<<relayPin;
    }
    if (val & 0x20) // Button B
    {
        modeOP = MOSQUITO_BURNER;
        PORTB ^= 1<<relayPin;
    }
    if (val & 0x84) // Button C
    {
        //Turn on the system by resetting the minCnt
        hourCnt = 0;
        minCnt = 0;
        relayHigh();
    }
    if (val & 0x10) // Button D
    {
        //Turn off the system by overflowing the minCnt
        hourCnt = safetyPwrOff+1;
        minCnt = 0;
        secCnt = 0;
        relayLow();
    }
    
}//ISR(PCINT0_vect)

//----------------------------------------------------------
ISR(WDT_vect)
{
    if (cnt8sec >= 8)
    {
        cnt8sec = 0;
        proceedTime();
    }
    timedPowerCtrl();
    cnt8sec++;
}//ISR(WDT_vect)

//------------------------------------------------
int main (void)
{
    ioinit(); //Setup IO pins and defaults
    
    // enable PC(Pin Change) interrupt
    GIMSK |= _BV(PCIE);  //Enable PC interrupt
    // Enable pin change interrupt for PBn
    PCMSK |= _BV(PCINT4);
    
    // temporarily prescale timer to n sec so we can measure current
    WDTCR |= (1<<WDP3) | (1<<WDP0); // 8s
    //WDTCR |= (1<<WDP3); // 4s
    // Enable watchdog timer interrupts
    WDTCR |= (1<<WDTIE);
    
    // enable global interrupt
    sei();
    
    // Use the Power Down sleep mode
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    relayHigh();
    //relayHigh();
    while(1) {
        sleep_mode();   // go to sleep and wait for interrupt...
    }
}//main

//------------------------------------------------
void ioinit (void)
{
    DDRB  = 0x00; //1 = output, 0 = input
    DDRB |= _BV(latchPin); //output
    DDRB |= _BV(clockPin); //output
    DDRB &= ~_BV(dataPin); //input
    DDRB |= _BV(relayPin); //output
    
    PORTB = 0x00;
}//ioinit

//------------------------------------------------
void readRF(char* val)
{
    int i;
    char inputData=0;
    
    clockLow();
    latchLow();
    for (i=7;i>=0;i--)
    {
        inputData |= (PINB & _BV(dataPin)) << i;
        clockHi();
        clockLow();
    }
    latchHi();
    
    *val = inputData;
    
}//readRF

//------------------------------------------------
void relayHigh()
{
    PORTB |= _BV(PB3);
}//relayHigh

//------------------------------------------------
void relayLow()
{
    PORTB &= ~_BV(PB3);
}//relayLow

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
        //minCnt=0;
    }
    
}//proceedTime

//------------------------------------------------

void timedPowerCtrl()
{
    if (hourCnt < safetyPwrOff)
    {
        //For fan control
        switch (modeOP)
        {
            case FAN: //fan
                ONintervalMin = 1;
                OFFintervalMin = 3;
                break;
                
            case MOSQUITO_BURNER:  //mosquito
                if (hourCnt < 2)
                {
                    ONintervalMin = 10;
                    OFFintervalMin = 10;
                }
                else if (hourCnt < 4)
                {
                    ONintervalMin = 10;
                    OFFintervalMin = 20;
                }
                else
                {
                    ONintervalMin = 10;
                    OFFintervalMin = 30;
                }
                break;
        }//switch (modeOP)
        
        //From minCnt is zero to ONintervalMin,
        //the relay is turned On
        if (minCnt < ONintervalMin)
        {
            relayHigh();
        }
        //From minCnt is ONintervalMin to twice ONintervalMin,
        //the relay is turned Off
        else
        {
            relayLow();
            if ( minCnt >= (ONintervalMin + OFFintervalMin) )
            {
                //minCnt is reset every twice ONintervalMin
                minCnt = 0;
            }
        }
        //wait half a second, so that the loopCnt increase by one at every half sec
        //delay(halfSec);
    }// if (hourCnt < safetyPwrOff)
    else
    {
        //system off
        relayLow();
        if (hourCnt == 250)
            hourCnt = safetyPwrOff;
    }//else, that is (minCnt >= safetyPwrOff)
}//timedPowerCtrl

