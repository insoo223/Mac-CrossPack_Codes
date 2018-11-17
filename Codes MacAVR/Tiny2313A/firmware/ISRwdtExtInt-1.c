/**************************************************************
 Target MCU & internal clock speed: ATtiny2313A @ 8Mhz
 Name    : ISRwdtExtInt-1.c 
 Purpose : Backup of ISRwdtExtInt.h working as EEPROM save mode alarm
 Author  : Insoo Kim (insoo@hotmail.com)
 Date    : Nov. 8, 2015
 UPDATE  : Nov. 8, 2015 (based on _74HC595_7seg.c)
 
 Description: 7 segment LED on ATtiny2313A cumstom board
 *****************************************************************/
#include "externs.h"
#include "defs.h"

//----------------------------------------------------------
// Watch-Dong Timer interrupt routine
ISR(WDT_OVERFLOW_vect)
{
    //PORTB ^= _BV(_7segDPpin); //blink DP when WDT is enabled and working
    
    curWDTcnt++;
    //Experimentally found 203 is very accuratedly 1 minute as of Nov 7,2015
    if (curWDTcnt >= 203)
        //if (curWDTcnt >= 15) // 2sec
    {
        curWDTcnt = 0;
        proceedTime();
    }
    
    lapseWDTcnt = curWDTcnt - prevWDTcnt;
    
    // passing around 1 sec lapse time between pressings of tactile switch
    if (lapseWDTcnt > 3)
    {
        //If a valide menu is selected then skip menu selection until the previously selected one is accomplished, while WDT routine still keeps time.
        if (!MENUSEL) //MENUSEL is set to 1 after a valide alarm is set thru alarmPresetMenu()
        {
            //PORTB |= _BV(_7segDPpin); //blink DP when WDT is enabled and working
            
            if (gOpMode == 1) // set alarm menu
            {
                alarmPresetMenu();
            }
            else if (gOpMode == 2) // set activity mode
            {
                //set activity mode i.e.
                //1-algorithm building 2-coding  3-HW schematic & soldering
                //4-wring a novel, 5-NYT, 6-Economist etc.
                gActMode = menuCnt;
                dispTwo7seg(gNightMode, DISP_DURATION, 20+gActMode);
                gOpMode = 1; // set alarm menu
                menuCnt = 0; // reset menuCnt to select alarm period
                prevWDTcnt = curWDTcnt;
            }// else if (gOpMode == 2)
        }//if (!MENUSEL)
    }//if (lapseWDTcnt > 3) // around 1 sec
    
    // passing around 2 sec lapse time between pressings of tactile switch
    if (lapseWDTcnt > 6)
    {
        if (!MENUSEL)
        {
            gOpMode = 2;
            dispTwo7seg(gNightMode, DISP_DURATION, 10+gOpMode);
            prevWDTcnt = curWDTcnt;
        }
    }
    /*-------------------
     -------------------*/
    
}//ISR(WDT_vect)

//----------------------------------------------------------
ISR(INT0_vect)
{
    //if swPin is pressed for the first time, enable alarm routine
    if (!firstPressed)
    {
        //------ Enable WDT ------
        WDTCSR |= _BV(WDP2); // seems to be 295ms (DS: 250 ms)
        //WDTCSR |= _BV(WDP3); // 4s (same as datasheet description): Good!
        //WDTCSR |= _BV(WDP2) || _BV(WDP0); // seems to be 100ms (DS: 1s)
        //WDTCSR |= _BV(WDP2); // seems to be 500ms (DS: 250 ms)
        // Enable watchdog timer interrupts
        WDTCSR |= _BV(WDIE);
        //WDTCSR |= _BV(WDCE) | _BV(WDIE);
        
        firstPressed = 1;
    }//if (!firstPressed)
    
    if (firstPressed)
    {
        menuCnt++;
        prevWDTcnt = curWDTcnt;
        dispTwo7seg(1, 30, menuCnt);
        _delay_ms(50); //debounce
    }//if (firstPressed)
    /*------------------
     -------------------*/
}//ISR(PCINT0_vect)

