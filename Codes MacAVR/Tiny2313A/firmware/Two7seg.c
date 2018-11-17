/**************************************************************
 Target MCU & internal clock speed: ATtiny2313A @ 8Mhz
 Name    : Two7seg.c (used by main.c)
 Author  : Insoo Kim (insoo@hotmail.com)
 Date    : Nov. 8, 2015
 UPDATE  : Nov. 8, 2015 (based on _74HC595_7seg.c)
 
 Description: 7 segment LED on ATtiny2313A cumstom board 
*****************************************************************/
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "externs.h"
#include "defs.h"

//-------------------------------------------------
uint8_t get7segByte_Num(uint8_t num)
{
    uint8_t targetByte, mcuByte=0;
    uint8_t i;
    
    // get A to G & DP bit pattern for "num"
    targetByte = _7segABC_Num[num];
    
    // convert 7seg ABC uint8_t into the uint8_t for MCU pins
    for (i=0; i<8; i++)
        if ( (targetByte >> i) & 0x01 )
            mcuByte |= 1 << MCU7segPin[i];
    
    return (mcuByte);
} //get7segByte_Num

//-------------------------------------------------
void dispTwo7seg(uint8_t nightMode, uint8_t duration, uint8_t num)
{
    uint8_t tens, ones;
    uint8_t i;
    
    tens = num / 10;
    ones = num % 10;
    
    for(i=0; i<duration; i++)
    {
        if(tens >0)
        {
            //PORTB = _7seg[tens];
            PORTB = get7segByte_Num(tens);
            OnTen();
            OffOne();
            _delay_us(SINGLE_DIGIT_DELAY);
        }

        //PORTB = _7seg[ones];
        PORTB = get7segByte_Num(ones);
        OnOne();
        OffTen();
        _delay_us(SINGLE_DIGIT_DELAY);
        if (nightMode == 1)
        {
            OffOne();
            OffTen();
            _delay_us(NIGHT_MODE_DELAY);
        }
    }
    OffTen();
    OffOne();
}

void OnOne()
{
    PORTD &= ~_BV(_7segLowerPin);
}

void OffOne()
{
    PORTD |= _BV(_7segLowerPin);
}

void OnTen()
{
    PORTD &= ~_BV(_7segHigherPin);
}
void OffTen()
{
    PORTD |= _BV(_7segHigherPin);
}