/**************************************************************
 Target MCU & internal clock speed: ATtiny85 @ 8Mhz
 Name    : _74HC595_7segMux
 Author  : Insoo Kim (insoo@hotmail.com)
 Date    : Nov. 6, 2015
 UPDATE  : Nov. 7, 2015 (based on _74HC595_7seg.c)
 
 Description: 7 segment LED on ATtiny85 cumstom board (5 by 7 cm)
 
 How to upload hex to target MCU using usbtiny programmer:
 
 cd "/Users/insoo/Google Drive/Coding/Ard/AVR/DemoTiny85/firmware"
 
 avrdude -c usbtiny -P usb -p attiny85 -U flash:w:main.hex:i
 
 HEX size[char]: 1326 out of 9632

 Ref:
 ** Written for and tested with a custom board with ATtiny13A run on 9.6Mhz internal osc

 When "Build" doesn't seem to work...
 Open the “Project” menu from the menubar
 Choose “Manage Schemes…”
 Your corrupt scheme should be in red, remove it
 Click the “Autocreate Schemes Now” button
 Done! (Ref: http://mathijskadijk.nl/post/14637472099/xcode-build-button-disabled)
 *****************************************************************/
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "externs.h"
#include "defs.h"
#include "ISRwdtPCint.h" 

//--------------------------------------------------
int main (void)
{
    uint8_t val;
    
    ioinit(); //Setup IO pins and defaults
    // enable PC(Pin Change) interrupt
    GIMSK |= _BV(PCIE);  //Enable PC interrupt
    // Enable pin change interrupt for PB4
    PCMSK |= _BV(swPin); // PCINT4
    // enable global interrupt
    sei();
    
    // Use the Power Down sleep mode
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    
    ctrlAll(0,0); //reset all 74HC595s
    
    gNightMode = 0;
    
    while(1)
    {
        /*--------------
        if (ALMIN == 0)
            sleep_mode();   // go to sleep and wait for interrupt...
        else
            alarmMin(ALMIN);

         dispTwo7seg(gNightMode,DISP_DURATION, 23);
         dispTwo7seg(gNightMode,DISP_DURATION, OFF_7SEG);
         _delay_ms(2);
         
         --------------*/

        //digitalWrite(latch4021pin,1);
        PORTB |= _BV(latch4021pin);
        //set it to 1 to collect parallel data, wait
        _delay_ms(2);
        //set it to 0 to transmit data serially
        //digitalWrite(latch4021pin,0);
        PORTB &= ~_BV(latch4021pin);

        val = shiftIn_INSOO(data4021pin, clock4021pin);
        if ((val & 0x01)==1)
            PORTB |= _BV(PB0);
        else
            PORTB &= ~_BV(PB0);
        //PORTB |= _BV(PB0);
         
        /*--------------
        if ((PINB & _BV(swPin)) == 0)
            PORTB |= _BV(PB0);
        else
            PORTB &= ~_BV(PB0);
         --------------*/
        
        _delay_ms(50);
    }
}//main

//--------------------------------------------------
void ioinit (void)
{
    //DDRB  = 0xff; //1 = output, 0 = input
    DDRB  = 0xFF; //1 = output, 0 = input
    DDRB |= _BV(latPin); //output
    DDRB |= _BV(ckPin); //output
    DDRB |= _BV(datPin); //output
    
    DDRB &= ~_BV(swPin); //input
    DDRB &= ~_BV(data4021pin); //input
    
    
    PORTB = 0x00;
}//ioinit

//--------------------------------------------------
void  beeleebeelee(uint8_t duration)
{
    uint8_t i;
    
    for (i=0; i<duration; i++)
    {
        //Positive vtg on buzz pin + at Q2 of 74HC595
        ctrlSingle(0x02, HIGH);
        //Ground on buzz pin - at Q3 of 74HC595
        ctrlSingle(0x03, LOW);
        //Turn ON "bee" in 2ms
        _delay_ms(2);
        //Turn OFF "bee" in 2ms, sounds "lee"
        ctrlSingle(0x02, LOW);
        _delay_ms(2);
    }
    
}//beeleebeelee

//------------------------------------------------
void alarmMin (uint8_t num)
{
    //uint8_t i;
    int8_t prevMinCnt;
    
    //reset WDT counter to give zero-reference to alarm duration
    curCnt125ms = 0;
    prevCnt125ms = 0;
    minCnt = 0;
    prevMinCnt=-1;
    
    /***********************************
     for (i=0; i<DISP_DURATION; i++)
     {
     dispTwo7seg(gNightMode,DISP_DURATION,  minCnt);
     dispTwo7seg(gNightMode,DISP_DURATION,  OFF_7SEG);
     _delay_ms(2);
     }
     ***********************************/
    //dispTwo7seg(gNightMode,DISP_DURATION_LONG, num-minCnt);
    
    while (!doneAlarm)
    {
        if (prevMinCnt != minCnt)
        {
            dispTwo7seg(gNightMode,DISP_DURATION_LONG, num-minCnt);
            prevMinCnt=minCnt;
        }//if (prevMinCnt != minCnt)
        
        //check whether alarm is done
        if (minCnt >= num)
        {
            //show alarm minutes just done
            dispTwo7seg(gNightMode,DISP_DURATION_LONG, num);
            
            //bee-lee bee-lee bee-lee
            beeleebeelee(30);
            
            doneAlarm = 1;
        }//if (minCnt >= num)
    }//while (!doneAlarm)
    
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
    curCnt125ms = 0;
    prevCnt125ms = 0;
   
    disableWDT();
}//alarmMin
//------------------------------------------------
void disableWDT()
{
    // Disable watchdog timer interrupts
    WDTCR &= ~(1<<WDIE);
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
