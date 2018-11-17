
#include <avr/io.h>
#include <util/delay.h>

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
int main (void)
{
    char val;
    ioinit(); //Setup IO pins and defaults
    
    while(1)
    {
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
        
        _delay_ms(15);
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

