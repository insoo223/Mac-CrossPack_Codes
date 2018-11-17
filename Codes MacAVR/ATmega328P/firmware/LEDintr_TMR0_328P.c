/**************************************************************
 Target MCU & clock speed: ATmega328P @ 8Mhz internal
 Name    : LEDintr_TMR0_328P.c
 Author  : Insoo Kim (insoo@hotmail.com)
 Date    : Sun April 12, 2015
 
 Description: Only when you touch the MCU, LED of PB blinks, using timer1 interrupt (T=1s) --> very strange phenomenon!
 
 HEX size[Byte]: 232 out of 32K
 
 Ref:
 https://sites.google.com/site/qeewiki/books/avr-guide/timers-on-the-atmega328
 Note:
     Strange stuff: When you touch the MCU, it works otherwise NOT!
     The circuit is built on a breadboard.
 *****************************************************************/

//Target: ATmega328P @ 8Mhz internal
//Strange stuff: When you touch the MCU, it works otherwise NOT!
//  Circuit is built on a breadboard.

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

uint8_t timer_overflow_count = 0;

//ISR(TIMER0_OVF_vect) {
ISR(TIMER0_COMPA_vect) {
    //if (++timer_overflow_count > 2) {   // a timer overflow occurs 4.6 times per second
    // Toggle Port B pin 4 output state
    PORTB ^= 1<<PB0;
    timer_overflow_count = 0;
    //}
}

int main(void) {
    
    //init I/O
    // Set up Port B pin 0 mode to output
    DDRB = 1<<DDB0;
    
    //interrupt condition for Timer
    // Set the Timer Mode to CTC
    TCCR0A |= (1 << WGM01);
    
    // Set the value that you want to count to
    OCR0A = 0x1a;
    //Set the ISR COMPA vect
    TIMSK0 |= (1 << OCIE0A);
    // prescale timer to 1/1024th the clock rate
    //TCCR2B |= (1<<CS22) | (1<<CS21) | (1<<CS20);
    TCCR0B |= (1<<CS02) | (1<<CS01) | (1<<CS00);
    
    // enable timer overflow interrupt
    TIMSK0 |= 1<< OCIE0A; //TOIE2;
    
    sei();
    
    while(1) {
        // let ISR handle the LED forever
    }
    
}