/**************************************************************
 Target MCU & internal clock speed: ATtiny2313A @ 8Mhz
 Name    : Two7seg.c (used by main.c)
 Author  : Insoo Kim (insoo@hotmail.com)
 Date    : Nov. 18, 2015
 UPDATE  : Nov. 18, 2015 (based on _74HC595_7seg.c)
 
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
void dispClock7seg(uint8_t nightMode, unsigned int duration, unsigned int num)
{
    uint8_t thousands, hundreds, tens, ones;
    unsigned int i;
    
    thousands = num / 1000;
    hundreds = (num - thousands*1000) / 100;
    tens = (num - thousands*1000 - hundreds*100) / 10;
    ones = num % 10;
    
    for(i=0; i<duration; i++)
    {
        if(thousands > 0)
        {
            PORTB = get7segByte_Num(thousands);
            OnKilo();
            
            OffHundred();
            OffTen();
            OffOne(); 
            _delay_ms(SINGLE_DIGIT_DELAY);
        }
        
        if ( (thousands > 0) || (hundreds > 0) )
        {
            PORTB = get7segByte_Num(hundreds);
            OnHundred();
            OffKilo();
            OffTen();
            OffOne();
            _delay_ms(SINGLE_DIGIT_DELAY);
        }
        
        if ( (thousands > 0) || (hundreds > 0) || (tens >0) )
        {
            PORTB = get7segByte_Num(tens);
            OnTen();
            OffKilo();
            OffHundred();
            OffOne();
            _delay_ms(SINGLE_DIGIT_DELAY);
        }
        
        PORTB = get7segByte_Num(ones);
        OnOne();
        OffTen();
        
        _delay_ms(SINGLE_DIGIT_DELAY);
        if (nightMode == 1)
        {
            OffKilo();
            OffHundred();
            OffTen();
            OffOne();
            _delay_ms(NIGHT_MODE_DELAY);
        }
    }
    OffOne();
}//dispClock7seg

//-------------------------------------------------
void OnOne()
{
    PORTD &= ~_BV(_7segDigit_U_Pin);
}
void OffOne()
{
    PORTD |= _BV(_7segDigit_U_Pin);
}

//-------------------------------------------------
void OnTen()
{
    PORTD &= ~_BV(_7segDigit_T_Pin);
}
void OffTen()
{
    PORTD |= _BV(_7segDigit_T_Pin);
}

//-------------------------------------------------
void OnHundred()
{
    PORTD &= ~_BV(_7segDigit_H_Pin);
}
void OffHundred()
{
    PORTD |= _BV(_7segDigit_H_Pin);
}

//-------------------------------------------------
void OnKilo()
{
    PORTD &= ~_BV(_7segDigit_K_Pin);
}
void OffKilo()
{
    PORTD |= _BV(_7segDigit_K_Pin);
}

//-------------------------------------------------
//-------------------------------------------------
