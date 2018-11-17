#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>

#define swPin PB1
//volatile unsigned char timer_overflow_count = 0;

ISR(INT0_vect)
{
    // Toggle PBn output state
    PORTB ^= 1<<PB0;
}

int main(void) {
    // Set up PBn mode to output
    //DDRB = 1<<DDB0;
    DDRB  = 0x00; //1 = output, 0 = input
    DDRB |= _BV(DDB0); //output
    DDRB &= ~_BV(DDB1); //input

    // enable EXT interrupt, INT0
    GIMSK |= _BV(INT0);  //Enable INT0

    //Trigger on falling edge of INT0
    MCUCR |= _BV(ISC01);
    MCUCR &= ~_BV(ISC00);
    
    sei();

    // Use the Power Down sleep mode
    //set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    set_sleep_mode(SLEEP_MODE_IDLE);
    
    while(1) {
        sleep_mode();   // go to sleep and wait for interrupt...
        /*
        if ((PINB & (1<<swPin)) == 0)
          PORTB = 1<<PB0;
        else
          PORTB = 0<<PB0;
        */
        //_delay_ms(75);
    }
}