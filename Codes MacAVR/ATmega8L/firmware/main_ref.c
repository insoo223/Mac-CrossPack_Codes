/**************************************************************
 Target MCU & internal clock speed: ATtiny2313A @ 8Mhz
 Name    : main.c
 Author  : Insoo Kim (insoo@hotmail.com)
 Date    : Nov. 8, 2015
 UPDATE  : Nov. 8, 2015 (based on _74HC595_7seg.c)
 
 Description: 7 segment LED on ATtiny2313A cumstom board
 
 How to upload hex to target MCU using usbtiny programmer:
 
 cd "/Users/insoo/Google Drive/Coding/Ard/AVR/ATmega8L/firmware"
 
 avrdude -c usbtiny -P usb -p atmega8 -U flash:w:main.hex:i
 
 For reading hex from an AVR MCU
 avrdude -c usbtiny -P usb -p attiny2313 -U flash:r:usbtinyFW.hex:i
 
 HEX size[char]: 1492 out of 2048
 
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
#include <avr/wdt.h>

#include "externs.h"
#include "defs.h"
#include "ISRwdtExtInt.h"
/*--------------
 --------------*/

//--------------------------------------------------
int main (void)
{
    //uint8_t val, doneEEPROM=0;
    
    ioinit(); //Setup IO pins and defaults
    
    //******* Enable INT0
    //Enable external interrupt INT0 at PD2
    GIMSK |= _BV(INT0);  //Enable
    //Trigger on low level of INT0
    MCUCR &= ~_BV(ISC01);
    MCUCR &= ~_BV(ISC00);
    //******* Enable INT0 (End)
    
    //WDTCSR |= _BV(WDP1) || _BV(WDP0); // and sometimes seems to be 100ms!! (DS: 125ms)
    
    //------ Enable WDT ------
    /*-------------
     wdt_reset();
     wdt_reset();
     wdt_enable(5);
     ------------------------*/
    // Working Good!
    //WDTCSR |= _BV(WDP3); // 4s (same as datasheet description): Good!
    //WDTCSR |= _BV(WDP2); // seems to be 295ms (DS: 250 ms)
    // Working NO good!
    //WDTCSR |= _BV(WDP3)|| _BV(WDP0); // very fast, even DS says 8s, No good.
    //WDTCSR |= _BV(WDP2) || _BV(WDP1) || _BV(WDP0); // seems to be 100ms (DS: 2s)
    //WDTCSR |= _BV(WDP2) || _BV(WDP1); // seems to be 100ms (DS: 1s)
    //WDTCSR |= _BV(WDP2) || _BV(WDP0); // seems to be 100ms (DS: 500ms)
    //WDTCSR |= _BV(WDP2) || _BV(WDP0); // and sometimes seems to be 25s!! (DS: 0.5s)
    //WDTCSR |= _BV(WDP1) || _BV(WDP0); // and sometimes seems to be 100ms!! (DS: 125ms)
    //WDTCR |= _BV(WDP1); // 64 ms
    // Enable watchdog timer interrupts
    //WDTCSR |= _BV(WDIE);
    
    // enable global interrupt
    sei();
    
    // Use the Power Down sleep mode
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    
    gNightMode = 0;
    //OnOne();
    
    while(1)
    {
        if (ALMIN == 0)
            sleep_mode();   // go to sleep and wait for interrupt...
        else
            alarmMin(ALMIN);
        /*********************
         initEEPROM();
         *********************/
        
    }
}//main

//--------------------------------------------------
void ioinit (void)
{
    swPin  = PD2; //INT0, tactile switch pin, Active Low
    _7segLowerPin  = PD4; //common cathode 7seg GND of ones digit
    _7segHigherPin  = PD5; //common cathode 7seg GND of tens digit
    buzzPin  = PD6; // buzzer pin positive
    
    _7segApin = PB2;
    _7segBpin = PB3;
    _7segCpin = PB6;
    _7segDpin = PB5;
    _7segEpin = PB4;
    _7segFpin = PB1;
    _7segGpin = PB0;
    _7segDPpin  = PB7;
    
    DDRB  = 0xFF; //1 = output, 0 = input
    DDRD  = 0xFF; //1 = output, 0 = input
    DDRD &= ~_BV(swPin); //INT0 as Input
    DDRA  = 0x00; //1 = output, 0 = input
    
    PORTB = 0x00;
    PORTD = 0x00;
}//ioinit


//------------------------------------------------
void alarmMin (uint8_t num)
{
    int8_t prevMinCnt;
    uint8_t eepromAdrs, val;
    
    //Reset WDT counter to give zero-reference to alarm mgmt
    curWDTcnt = 0;
    prevWDTcnt = 0;
    minCnt = 0;
    prevMinCnt=-1;
    
    while (!doneAlarm)
    {
        showProgress(minCnt, num);
        if (prevMinCnt != minCnt)
        {
            dispTwo7seg(gNightMode,DISP_DURATION, num-minCnt);
            //_delay_ms(100);
            //showProgress(minCnt, num);
            prevMinCnt=minCnt;
        }//if (prevMinCnt != minCnt)
        
        //check whether alarm is done
        if (minCnt >= num)
        {
            //bee-lee bee-lee bee-lee
            beeleebeelee(30);
            
            //show alarm minutes just done
            dispTwo7seg(gNightMode,DISP_DURATION, num);
            
            doneAlarm = 1;
            
            if (gActMode != 0)
            {
                eepromAdrs = (gActMode-1)*6 + menuCnt - 1;
                val = EEPROM_read(eepromAdrs);
                _delay_ms(10);
                val++;
                EEPROM_write(eepromAdrs, val);
                _delay_ms(20);
                val = EEPROM_read(eepromAdrs);
                _delay_ms(20);
                dispTwo7seg(gNightMode,DISP_DURATION, gActMode);
                dispTwo7seg(gNightMode,DISP_DURATION, menuCnt);
                dispTwo7seg(gNightMode,DISP_DURATION, val);
                _delay_ms(100);
            }
        }//if (minCnt >= num)
    }//while (!doneAlarm)
    OffOne();
    OffTen();
    //reset alarm duration
    ALMIN=0;
    //reset usr selected menu
    menuCnt = -1;
    //reset menu selection notification
    MENUSEL=0;
    //reset alarm finish notification
    doneAlarm = 0;
    //reset wake up WDT interrupt sentinel
    firstPressed=0;
    //disable WDT and only wake up when the tactile button is pressed
    curWDTcnt = 0;
    prevWDTcnt = 0;
    gOpMode = 0;
    gActMode = 0;
    
    disableWDT();
}//alarmMin

//--------------------------------------------------
void alarmPresetMenu()
{
    switch (menuCnt)
    {
        case 1:
            ALMIN=3;
            MENUSEL=1;
            break;
        case 2:
            ALMIN=5;
            MENUSEL=1;
            break;
        case 3:
            ALMIN=10;
            MENUSEL=1;
            break;
        case 4:
            ALMIN=20;
            MENUSEL=1;
            break;
        case 5:
            ALMIN=30;
            MENUSEL=1;
            break;
        case 6:
            ALMIN=60;
            MENUSEL=1;
            break;
        default:
            MENUSEL=0;
            break;
    }//switch (menuCnt)
}//alarmPresetMenu

//--------------------------------------------------
void showProgress(uint8_t curNum, uint8_t finNum)
{
    uint8_t pct, i, pctile=0;
    
    pct = curNum * 100 / finNum;
    
    //find current percentile of alarm progress
    for(i=0; i<ALARM_PROGRESS_DIV; i++)
        if (pct < (i+1)*ALARM_PROGRESS_UNIT)
        {
            pctile = i;
            break;
        }
    PORTB = _BV(MCU7segPin[pctile]) | _BV(_7segDPpin);
    OnOne();
    if (gNightMode)
        OffOne();
}//showProgress

//--------------------------------------------------
void  beeleebeelee(uint8_t duration)
{
    uint8_t i;
    
    for (i=0; i<duration; i++)
    {
        //Positive vtg on buzz pin + at Q2 of 74HC595
        //ctrlSingle(0x02, HIGH);
        PORTD |= _BV(buzzPin);
        //Ground on buzz pin - at Q3 of 74HC595
        //ctrlSingle(0x03, LOW);
        
        //Turn ON "bee" in 2ms
        _delay_ms(2);
        //Turn OFF "bee" in 2ms, sounds "lee"
        //ctrlSingle(0x02, LOW);
        PORTD &= ~_BV(buzzPin);
        _delay_ms(2);
    }
    
}//beeleebeelee


//------------------------------------------------
void disableWDT()
{
    // Disable watchdog timer interrupts
    WDTCSR &= ~_BV(WDIE);
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
