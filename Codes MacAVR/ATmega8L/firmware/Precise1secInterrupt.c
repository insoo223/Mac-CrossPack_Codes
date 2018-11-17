#include <avr/interrupt.h>

volatile unsigned char timer_overflow_count = 0;

ISR(TIM0_OVF_vect)
{
    if (++timer_overflow_count > 35)
    {   // with 1024/256/64 prescaler, a timer overflow occurs 4.6/18/73 times per second accordingly.
        // Toggle Port B pin 4 output state
        PORTB ^= 1<<PB0;
        timer_overflow_count = 0;
    }
}

int main(void) {
    // Set up Port B pin 0 mode to output
    DDRB = 1<<DDB0;
    
    // prescale timer to 1/1024th the clock rate
    //TCCR0B |= (1<<CS02) | (1<<CS00);

    // prescale timer to 1/256th the clock rate
    //TCCR0B |= (1<<CS02);
    
    // prescale timer to 1/64th the clock rate
    TCCR0B |= (1<<CS01) | (1<<CS00);

    // enable timer overflow interrupt
    TIMSK0 |=1<<TOIE0;
    sei();
    
    while(1) {
        // let ISR handle the LED forever
    }
}