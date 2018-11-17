/**************************************************************
 Target MCU & internal clock speed: ATtiny13A @ 9.6Mhz
 Name    : RF_PCint_CD4021.c
 Author  : Insoo Kim (insoo@hotmail.com)
 Date    : April 11, 2015
 
 Description:
 1)
 2)
 
 HEX size[Byte]: 994 out of 1024
 
 Ref:
 ** Written for and tested with a custom board with ATtiny13A run on 9.6Mhz int osc
 ** Functions regarding DHT11, i.e."dht_getdata", are refered from the following link of David Gironi.
 ** But, specific time delays have been measured using Hantek USB digital oscilloscope (6022BE) and modified to fit into my custom ATtiny13A board.
 ** http://davidegironi.blogspot.kr/2013/02/reading-temperature-and-humidity-on-avr.html#.VSRiHSlCNSV
 *****************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>

#define halfSec 75

#define latchPort PORTB
#define latchPin  PB0 //latch
#define clockPort PORTB
#define clockPin  PB1 //clock
#define dataPort  PORTB
#define dataPin   PB2 //data

#define relayPin  PB3 //relay

#define latchLow()  latchPort&=~_BV(latchPin)
#define latchHi() latchPort|=_BV(latchPin)
#define clockLow()  clockPort&=~_BV(clockPin)
#define clockHi() clockPort|=_BV(clockPin)


//Define functions
//======================
void ioinit(void);
void readRF(char*);
//======================

ISR(PCINT0_vect)
{
    char val;

    readRF(&val);

    //if (val & 0x08) // always ON
    //if (val & 0x04) // always ON
    //if (val & 0x10) // Button D
    //if (val & 0x40) // always ON
    //if (val & 0x80) // always OFF
    if (val & 0x20) // Button B
        PORTB |= _BV(PB3);

    if (val & 0x10) // Button D
        PORTB &= ~_BV(PB3);
}//ISR(PCINT0_vect)

int main (void)
{
    ioinit(); //Setup IO pins and defaults

    // enable PC(Pin Change) interrupt
    GIMSK |= _BV(PCIE);  //Enable PC interrupt

    // Enable pin change interrupt for PBn
    PCMSK |= _BV(PCINT4);

    // enable global interrupt
    sei();

    // Use the Power Down sleep mode
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);

    while(1) {
        sleep_mode();   // go to sleep and wait for interrupt...
    }
}//main


void ioinit (void)
{
    DDRB  = 0x00; //1 = output, 0 = input
    DDRB |= _BV(latchPin); //output
    DDRB |= _BV(clockPin); //output
    DDRB &= ~_BV(dataPin); //input
    DDRB |= _BV(relayPin); //output

    PORTB = 0x00;
}//ioinit


void readRF(char* val)
{
    int i;
    char inputData=0;

    clockLow();
    latchLow();
    for (i=7;i>=0;i--)
    {
        inputData |= (PINB & _BV(dataPin)) << i;
        clockHi();
        clockLow();
    }
    latchHi();

    *val = inputData;

}//readRF

