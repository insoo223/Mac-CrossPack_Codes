#include <avr/interrupt.h>

volatile int timer_overflow_count = 0;

ISR(TIMER0_OVF_vect) {
    if (++timer_overflow_count > 5) {   // a timer overflow occurs 4.6 times per second
        // Toggle Port B pin 4 output state
        PORTB ^= 1<<PB0;
        timer_overflow_count = 0;
    }
}

int main(void) {
    // Set up Port B pin 0 mode to output
    DDRB = 1<<DDB0;
    
    // prescale timer to 1/1024th the clock rate
    TCCR0B |= (1<<CS02) | (1<<CS00);
    
    // enable timer overflow interrupt
    TIMSK0 |=1<<TOIE0;
    sei();
    
    while(1) {
        // let ISR handle the LED forever
    }
}