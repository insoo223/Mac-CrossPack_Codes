/**************************************************************
 Target MCU & internal clock speed: ATtiny13A @ 9.6Mhz
 Name    : Mosquito_WDT.c
 Author  : Insoo Kim (insoo@hotmail.com)
 Date    : Sep. 22, 2015
 UPDATE  : Oct. 19, 2015 (based on RF_PCint_FanMosq_WDT.c)
 
 Description: time control of mosquito vaporizer
 1) On for 10 min, off for 10 min for the 1st two hours
 then off 10 & 10 (previously 15 & 15, 20 & 30 but invincible mosquitos were still flying, so that shorten up off interval 10 & 10 as of Sep26,2015) min for the following two hours each, on time remains same.
 
 How to upload hex to target MCU using usbtiny programmer:
 
 cd "/Users/insoo/Google Drive/Coding/Ard/AVR/DemoTiny13A/firmware"
 
 avrdude -c usbtiny -P usb -p attiny13 -U flash:w:main.hex:i
 
 HEX size[char]: 324 out of 1024
 
 Ref:
 ** Written for and tested with a custom board with ATtiny13A run on 9.6Mhz internal osc
 *****************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>

#define halfSec 75


#define relayPin  PB4 //relay

// power off hours for safety
#define safetyPwrOff 12 //after 8 hrs of run-time, the system will be off.

#define ON_MIN_1st3rds  10
#define OFF_MIN_1st3rds 5

#define ON_MIN_2nd3rds  10
#define OFF_MIN_2nd3rds 5

#define ON_MIN_3rd3rds  10
#define OFF_MIN_3rd3rds 5


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
void relayHigh(void);
void relayLow(void);
void proceedTime(void);
void timedPowerCtrl(void);
//======================


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
    DDRB |= _BV(relayPin); //output
    
    PORTB = 0x00;
}//ioinit

//------------------------------------------------
void relayHigh()
{
    PORTB |= _BV(relayPin);
}//relayHigh

//------------------------------------------------
void relayLow()
{
    PORTB &= ~_BV(relayPin);
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
        if (hourCnt < safetyPwrOff/3)
        {
            ONintervalMin = ON_MIN_1st3rds;
            OFFintervalMin = OFF_MIN_1st3rds;
        }
        else if (hourCnt < safetyPwrOff/3*2)
        {
            ONintervalMin = ON_MIN_2nd3rds;
            OFFintervalMin = OFF_MIN_2nd3rds;
        }
        else
        {
            ONintervalMin = ON_MIN_3rd3rds;
            OFFintervalMin = OFF_MIN_3rd3rds;
        }
        
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

