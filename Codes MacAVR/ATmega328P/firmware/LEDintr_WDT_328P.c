/**************************************************************
 Target MCU & clock speed: ATmega328P @ 8Mhz internal
 Name    : LEDintr_WDT_328P.c
 Author  : Insoo Kim (insoo@hotmail.com)
 Date    : Sun April 12, 2015
 
 Description: Blink LED of PB0, using timer1 interrupt (T=1s)
 
 HEX size[Byte]: 218 out of 32K
 
 Ref:
 https://sites.google.com/site/qeewiki/books/avr-guide/timers-on-the-atmega328
 Note:
 Strange stuff resolved!: Refer LEDintr_TMR0.c
 *****************************************************************/
//#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>

ISR(WDT_vect)
{
    PORTB ^= 1<<PB0;
}

void check_wdt(void){
    if(MCUSR & _BV(WDRF)){                  // If a reset was caused by the Watchdog Timer...
        MCUSR &= ~_BV(WDRF);                // Clear the WDT reset flag
        WDTCSR |= (_BV(WDCE) | _BV(WDE));   // Enable the WD Change Bit
        WDTCSR = 0x00;                      // Disable the WDT
    }
}

void setup_wdt(void){
    // Set up Watch Dog Timer for Inactivity
    // Enable the WD Change Bit
    WDTCSR |= (_BV(WDCE) | _BV(WDE));
    // Enable WDT interrupt
    // Set Timeout to ~1 seconds
    WDTCSR =   _BV(WDIE) |  _BV(WDP2) | _BV(WDP1);
    // Set Timeout to ~500 ms
    //WDTCSR =   _BV(WDIE) | _BV(WDP2)| _BV(WDP0);
}

//call this routine to initialize all peripherals
void init_devices(void){
    //stop errant interrupts until set up
    cli(); //disable all interrupts
    
    //timer0_init();
    
    MCUCR = 0x00;
    EICRA = 0x00; //extended ext ints
    EIMSK = 0x00;
    
    TIMSK0 = 0x02; //timer 0 interrupt sources
    
    PRR = 0x00; //power controller
    sei(); //re-enable interrupts
    //all peripherals are now initialized
}

//TIMER0 initialize - prescale:1024
// WGM: CTC
// desired value: 10mSec
// actual value: 10.048mSec (-0.5%)
/*
void timer0_init(void){
    TCCR0B = 0x00; //stop
    TCNT0 = 0x00; //set count
    TCCR0A = 0x02; //CTC mode
    OCR0A = 0xFF;
    //OCR0A = 0x9C;
    TCCR0B = 0x05; //start timer
}
*/

void init_io(void){
    DDRB  = 0xff;                              // use all pins on port B for output
    PORTB = 0x00;                              // (LED's low & off)
}

int main(void) {
    init_io();
    
    cli();
    check_wdt();
    setup_wdt();
    sei();                                      // Enables interrupts

    /*
    //init I/O
    // Set up Port B pin 0 mode to output
    DDRB = 1<<DDB0;

    TIMSK0 = 0x02; //timer 0 interrupt sources

    //interrupt condition for Watch Dog Timer
    MCUSR &= ~_BV(WDRF);
    // temporarily prescale timer to 4s so we can measure current
    //WDTCSR |= (1<<WDP3); // (1<<WDP2) | (1<<WDP0);
    WDTCSR |= (1<<WDP2) | (1<<WDP1);
    
    // Enable watchdog timer interrupts
    WDTCSR |= (1<<WDIE);// | (1<<WDCE) | (1<<WDE);

    sei();
    */
    
    // Use the Power Down sleep mode
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    
    while(1) {
        // let ISR handle the LED forever
        sleep_mode();   // go to sleep and wait for interrupt...
    }
    
}