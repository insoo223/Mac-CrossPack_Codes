/**************************************************************
 Target MCU & clock speed: ATmega328P @ 8Mhz internal
 Name    : LEDintr_TMR1_328P.c
 Author  : Insoo Kim (insoo@hotmail.com)
 Date    : Sun April 12, 2015
 
 Description: Blink LED of PB0, using timer1 interrupt (T=1s)
 
 HEX size[Byte]: 218 out of 32K
 
 Ref:
 https://sites.google.com/site/qeewiki/books/avr-guide/timers-on-the-atmega328
 Note:
 Strange stuff resolved!: Refer LEDintr_TMR0.c
 *****************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>

ISR(TIMER1_COMPA_vect)
{
    PORTB ^= 1<<PB0;
}

int main(void) {
    
    //init I/O
    // Set up Port B pin 0 mode to output
    DDRB = 1<<DDB0;
    
    //interrupt condition for Timer 1
    
    //value of Output Compare Register 1A
    //0.5sec at 8Mhz internal clock
    OCR1A = 0x1E8;
    
    //Mode 4, CTC on OCR1A
    //Datasheet: ch15.11.1 table 15.4
    TCCR1B |= (1 << WGM12);
    
    //Set Output Compare Interrupt Enable 1A match
    //Datasheet: ch15.11.8
    TIMSK1 |= (1 << OCIE1A);
    
    //Clock Select: set prescaler to 1024 and start the timer
    //Datasheet: ch15.11.2 table 15.5
    TCCR1B |= (1 << CS12) | (1 << CS10);
    sei();
    
    
    while(1) {
        // let ISR handle the LED forever
    }
    
}