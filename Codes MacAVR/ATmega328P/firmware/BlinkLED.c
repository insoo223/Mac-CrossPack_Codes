/**************************************************************
 Target MCU & clock speed: ATmega328P @ 8Mhz internal
 Name    : BlinkLED.c
 Author  : Insoo Kim (insoo@hotmail.com)
 Date    : Sun April 12, 2015
 
 Description: Blink LED of PB0
 
 HEX size[Byte]: 164 out of 32K
 
 Ref:
 *****************************************************************/
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    // Set up Port B pin 0 mode to output
    DDRB = 1<<DDB0;

    while(1)
    {
        PORTB ^= 1<<PB0;
        _delay_ms(75);
    }
    
}