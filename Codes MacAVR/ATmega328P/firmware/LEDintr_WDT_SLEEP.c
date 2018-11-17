/**************************************************************
 Target MCU & clock speed: ATmega328P @ 8Mhz internal
 Name    : LEDintr_WDT_Sleep.c
 Author  : Insoo Kim (insoo@hotmail.com)
 Date    : Sun April 12, 2015
 
 Description: Blink LED of PB0, using WDT interrupt (T=1s)
 
 HEX size[Byte]: 282 out of 32K
 
 Ref:
     http://elegantcircuits.com/2014/10/14/introduction-to-the-avr-watchdog-timer/
     https://github.com/thaletterb/watchdog_timer/blob/master/watchdog_interrupt.c
    https://github.com/thaletterb/watchdog_timer
 Note:
 *****************************************************************/
#include <avr/interrupt.h>
#include <avr/sleep.h>

ISR(WDT_vect) {
    // Toggle PB0 output state
    PORTB ^= 1<<PB0;
}

int main(void) {
    // Set up PB0 mode to output
    DDRB = 1<<DDB0;
    
    // temporarily prescale timer to 4s so we can measure current
    WDTCR |= (1<<WDP3); // (1<<WDP2) | (1<<WDP0);
    
    // Enable watchdog timer interrupts
    WDTCR |= (1<<WDTIE);
    
    sei(); // Enable global interrupts
    
    // Use the Power Down sleep mode
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    
    for (;;) {
        sleep_mode();   // go to sleep and wait for interrupt...
    }
}