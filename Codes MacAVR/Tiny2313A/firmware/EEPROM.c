/**************************************************************
 Target MCU & internal clock speed: ATtiny2313A @ 8Mhz
 Name    : EEPROM.c (used by main.c)
 Author  : Insoo Kim (insoo@hotmail.com)
 Date    : Nov. 12, 2015
 UPDATE  : Nov. 12, 2015 (based on _74HC595_7seg.c)
 
 Description: 7 segment LED on ATtiny2313A cumstom board
 *****************************************************************/
#include <avr/EEPROM.h>
#include <util/delay.h>
#include "defs.h"
#include "externs.h"

void initEEPROM()
{
    uint8_t i;
    
    if (!doneINITeeprom)
    {
        for (i=0; i<128; i++)
        {
            EEPROM_write(i, 0);
            _delay_ms(10);
        }

        for (i=0; i<128; i++)
        {
            dispTwo7seg(gNightMode, DISP_DURATION, i%100);
            dispTwo7seg(gNightMode, DISP_DURATION, EEPROM_read(i));
            //_delay_ms(10);
        }

        doneINITeeprom = 1;
    }
}//initEEPROM

void EEPROM_write(unsigned int ucAddress, unsigned char ucData)
{
    /* Wait for completion of previous write */
    while(EECR & (1<<EEPE))
        ;
    /* Set Programming mode */
    EECR = (0<<EEPM1)|(0<<EEPM0);
    /* Set up address and data registers */
    EEAR = ucAddress;
    EEDR = ucData;
    /* Write logical one to EEMPE */
    EECR |= (1<<EEMPE);
    /* Start eeprom write by setting EEPE */
    EECR |= (1<<EEPE);
}

unsigned char EEPROM_read(unsigned int ucAddress)
{
    /* Wait for completion of previous write */
    while(EECR & (1<<EEPE))
        ;
    /* Set up address register */
    EEAR = ucAddress;
    /* Start eeprom read by writing EERE */
    EECR |= (1<<EERE);
    /* Return data from data register */
    return EEDR;
}