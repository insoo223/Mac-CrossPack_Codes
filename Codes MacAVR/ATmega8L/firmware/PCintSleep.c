#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>

#define swPin PB1
//volatile unsigned char timer_overflow_count = 0;

ISR(PCINT0_vect)
{
    // Toggle PBn output state
    PORTB ^= 1<<PB0;
}

int main(void) {
    // Set up PBn mode to output
    DDRB  = 0x00; //1 = output, 0 = input
    DDRB |= _BV(DDB0); //output
    DDRB &= ~_BV(DDB3); //input
    
    // enable PC(Pin Change) interrupt
    GIMSK |= _BV(PCIE);  //Enable PC interrupt
    // Enable pin change interrupt for PB3
    PCMSK |= _BV(PCINT3);
    
    sei();
    
    // Use the Power Down sleep mode
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    
    while(1) {
        sleep_mode();   // go to sleep and wait for interrupt...
    }
}