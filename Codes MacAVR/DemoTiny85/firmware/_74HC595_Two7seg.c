#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "externs.h"
#include "defs.h"
//#include "ISRwdtPCint.h"

//-----------------------------------------------
// bit-oriented control for 74HC595
// Control three 74HC595 output eXpansion chips
// created: Thu Sep24,2015
// input args
//    ctrlID: refer "defs.h", 74HC595 pin assignments
//    state: HIGH to set, LOW to clear a corresponding pin of a 74HC595
void ctrlSingle(uint8_t ctrlID, uint8_t state)
{
    uint8_t bottomByte=0, topByte=0;
    uint8_t sectorX, dataX;
    
    // The first bit  in the upper nibble, bit 4, of ctrlID represents
    // which of three 74HC595s, called "sectorX", holds the corresponding pin
    // sectorX 0 for bottom, 1 for middle, and 2 for top sector of three 74HC595s
    sectorX = ctrlID >> SECTOR_BIT_LOCATION_74HC595;
    
    // The bits in the lower nibble, bit3 to bit0, of ctrlID represent
    // the corresponding pin of the 74HC595 chip of ctrlID
    dataX = ctrlID & DATA_BITS_MASK_74HC595;
    
    switch (sectorX)
    {
        case 0: //bottom 74HC595
            if  (state == HIGH)
                bottomByte = gCurBotX | _BV(dataX);
            else
                bottomByte = gCurBotX & ~_BV(dataX);
            topByte = gCurTopX;
            break;
            
        case 1: //top 74HC595
            bottomByte = gCurBotX;
            if  (state == HIGH)
                topByte = gCurTopX | _BV(dataX);
            else
                topByte = gCurTopX & ~_BV(dataX);
            break;
    }//switch(sectorX)
    
    //update the current bytes of tripple 74HC595s
    updateX(topByte, bottomByte);
    
    ctrlAll(gCurTopX, gCurBotX);
    
}//ctrlSingle

//-----------------------------------------------
// byte-oriented control for 74HC595
void ctrlAll(uint8_t topX, uint8_t botX)
{
    ST_CP_low();
    shiftOut_INSOO(datPin, ckPin, botX); // ctrl digit, buzz
    shiftOut_INSOO(datPin, ckPin, topX); // 7 seg
    ST_CP_high();
    
    //updateX(topX, midX, botX); //if execute, remnant display on K-unit
}//ctrlAll


//-----------------------------------------------
// maintain the most current top and bottom bytes of tripple 74HC595s
void updateX(uint8_t topX, uint8_t botX)
{
    gCurTopX = topX;
    gCurBotX = botX;
}//updateX

//--------------------------------------------------
void dispTwo7seg(uint8_t nightMode, uint8_t duration, uint8_t num)
//void dispTwo7seg(uint8_t num)
{
    uint8_t tens, ones, i;
    
    tens = num/10;
    ones = num%10;
    
    for (i=0; i<duration; i++)
    {
        if (tens == 0)
            tens=11; //blank leading zero
        ctrlAll(_7seg[tens], TENS_GND);
        ctrlAll(_7seg[ones], ONES_GND);
        if (nightMode)
        {
            //7 seg LED brightness control in the dark ambience
            ctrlAll(_7seg[tens], ~TWO_DIGIT_GND); //don't care what num is
            _delay_ms(NIGHTMODE_DURATION);
        }
    }
    ctrlAll(_7seg[tens], ~TWO_DIGIT_GND);
}//dispTwo7seg

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

