/**************************************************************
 Target MCU & internal clock speed: ATmega8L @ 8Mhz
 Name    : main.c
 Author  : Insoo Kim (insoo@hotmail.com)
 Date    : Nov. 18, 2015
 UPDATE  : Nov. 18, 2015 (based on _74HC595_7seg.c)
 
 Description: 7 segment LED on ATtiny2313A cumstom board
 
 How to upload hex to target MCU using usbtiny programmer:
 
cd "/Users/insoo/Google Drive/Coding/Ard/AVR/ATmega8L/firmware"
 
avrdude -c usbtiny -P usb -p atmega8 -U flash:w:main.hex:i
 
 For reading hex from an AVR MCU
 avrdude -c usbtiny -P usb -p attiny2313 -U flash:r:usbtinyFW.hex:i
 
 HEX size[char]:  out of 8K
 
 Ref:
 ** Written for and tested with a custom board with ATtiny13A run on 9.6Mhz internal osc
 
 When "Build" doesn't seem to work...
 Open the “Project” menu from the menubar
 Choose “Manage Schemes…” 
 Your corrupt scheme should be in red, remove it
 Click the “Autocreate Schemes Now” button
 Done! (Ref: http://mathijskadijk.nl/post/14637472099/xcode-build-button-disabled)
 *****************************************************************/
#include <avr/io.h> 
#include <util/delay.h>

#include "defs.h"
#include "externs.h"
#include "i2c.h"

//#include "uart.h"  

/*-------------- 
#include <avr/interrupt.h>   
#include <avr/sleep.h> 
#include <avr/wdt.h> 

#include "ISRwdtExtInt.h"  
 --------------*/
  
//--------------------------------------------------
int main (void)
{
    //uint8_t i;
    
    ioinit(); //Setup IO pins and defaults
    i2c_init(); // init I2C interface
    //init uart
    //uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
 
    while(1) 
    {
        mainDS1307();
        //_delay_ms(3000);
        //SetTime(9,34,56, 1);
        //SetDate(19,11,15); 
        //DisplayDateTime();

        /*********************
         dispClock7seg(1, DISP_DURATION, 1234);
         _delay_ms(2000);
         dispClock7seg(1, DISP_DURATION, 5678);
         _delay_ms(2000);

         PORTB = 0xFF;
         for (i=PD4; i<=PD7; i++)
         {
         PORTD &= ~_BV(i);
         _delay_ms(1);
         PORTD |= _BV(i);
         _delay_ms(1);
         }
 
         PORTC = 0x00;

         for (i=PD4; i<=PD7; i++)
         {
         PORTD &= ~_BV(i); 
         _delay_ms(1000);
         PORTD |= _BV(i);
         _delay_ms(1000);
         }
         *********************/
         
    }   
}//main

//--------------------------------------------------
void ioinit (void)
{
    swPin  = PC1; //INT0, tactile switch pin, Active Low
    buzzPin  = PC2; // buzzer pin positive

    _7segDigit_K_Pin = PD4;
    _7segDigit_H_Pin = PD5;
    _7segDigit_T_Pin = PD6;
    _7segDigit_U_Pin = PD7;
    
    _7segApin = PB0;
    _7segBpin = PB1;
    _7segCpin = PB2;
    _7segDpin = PB3;
    _7segEpin = PB4;
    _7segFpin = PB5;
    _7segGpin = PB6;
    _7segDPpin  = PB7;
    
    DDRB  = 0xFF; //1 = output, 0 = input
    DDRD  = 0xFF; //1 = output, 0 = input
    DDRC  = 0xFF; //1 = output, 0 = input
    
    PORTB = 0x00; 
    PORTD = 0xFF;
    PORTC = 0x00;

    prevMin = 0; //for RT clock display update

}//ioinit


