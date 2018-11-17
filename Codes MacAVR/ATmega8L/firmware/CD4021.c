
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "externs.h"
#include "defs.h"

uint8_t shiftIn_INSOO(uint8_t myDataPin, uint8_t myClockPin)
{
    uint8_t value = 0;
    uint8_t i;
    
    for (i = 0; i < 8; ++i)
    {
        //digitalWrite(myClockPin, HIGH);
        PORTB |= _BV(myClockPin);
        
        //value |= digitalRead(myDataPin) << (7 - i);
        value |=  (PINB & _BV(myDataPin)) << (7 - i);
        
        //digitalWrite(myClockPin, LOW);
        PORTB &= ~_BV(myClockPin);
        //_delay_us(1);
    }
    return value;
}