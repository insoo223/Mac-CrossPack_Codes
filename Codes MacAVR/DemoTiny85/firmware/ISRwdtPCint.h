#include "externs.h"
#include "defs.h"



//----------------------------------------------------------
// Watch-Dong Timer interrupt routine
ISR(WDT_vect)
{
    //Experimentally found 1753 is very accuratedly 1 minute as of Nov 6,2015
    //if (curCnt125ms >= 1753)
    if (curCnt125ms >= 100) // 3sec
    {
        curCnt125ms = 0;

        proceedTime();
    }
    
    lapseCnt125ms = curCnt125ms - prevCnt125ms;
    if (lapseCnt125ms > 30) // around 1 sec
    {
        //If a valide menu is selected then skip menu selection until the previously selected one is accomplished, and WDT routine keeps time.
        if (!MENUSEL)
        {
            switch (menuCnt)
            {
                case 1:
                    dispTwo7seg(gNightMode,DISP_DURATION, 1);
                    ALMIN=3;
                    MENUSEL=1;
                    break;
                case 2:
                    dispTwo7seg(gNightMode,DISP_DURATION, 2);
                    ALMIN=5;
                    MENUSEL=1;
                    break;
                case 3:
                    dispTwo7seg(gNightMode,DISP_DURATION, 3);
                    ALMIN=10;
                    MENUSEL=1;
                    break;
                case 4:
                    dispTwo7seg(gNightMode,DISP_DURATION, 4);
                    ALMIN=20;
                    MENUSEL=1;
                    break;
                case 5:
                    dispTwo7seg(gNightMode,DISP_DURATION, 5);
                    ALMIN=30;
                    MENUSEL=1;
                    break;
                case 6:
                    dispTwo7seg(gNightMode,DISP_DURATION, 6);
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
            WDTCR |= _BV(WDP1) || _BV(WDP0);
            //WDTCR |= _BV(WDP1); // 64 ms
            // Enable watchdog timer interrupts
            WDTCR |= _BV(WDIE);
            
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

