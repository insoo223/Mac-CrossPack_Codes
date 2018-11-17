#include <avr/io.h>
#include <util/delay.h>

#define DS_PORT    PORTB
#define DS_PIN     0 //data
#define ST_CP_PORT PORTB
#define ST_CP_PIN  2 //latch
#define SH_CP_PORT PORTB
#define SH_CP_PIN  1 //clock

#define DS_low()  DS_PORT&=~_BV(DS_PIN)
#define DS_high() DS_PORT|=_BV(DS_PIN)
#define ST_CP_low()  ST_CP_PORT&=~_BV(ST_CP_PIN)
#define ST_CP_high() ST_CP_PORT|=_BV(ST_CP_PIN)
#define SH_CP_low()  SH_CP_PORT&=~_BV(SH_CP_PIN)
#define SH_CP_high() SH_CP_PORT|=_BV(SH_CP_PIN)

//Define functions
//======================
void ioinit(void);
void output_led_state(unsigned char __led_state);
//======================

int main (void)
{
    int i;
    ioinit(); //Setup IO pins and defaults
    
    while(1)
    {
        /*
         Output a knight rider pattern
         
         10000000
         01000000
         00100000
         00010000
         00001000
         00000100
         00000010
         00000001
         00000010
         00000100
         00001000
         00010000
         00100000
         01000000
         */
        
        for ( i=0;i<8;i++)
        {
            output_led_state(_BV(i));
            _delay_ms(500);
        }
        
    }
}


void ioinit (void)
{
    DDRB  = 0b00000111; //1 = output, 0 = input
    PORTB = 0b00000000;
}

void show7seg(unsigned char num)
{
    int i;
    SH_CP_low();
    ST_CP_low();
    for (i=0;i<8;i++)
    {
        if (bit_is_set(__led_state, i))
            DS_high();
        else
            DS_low();
        
        
        SH_CP_high();
        SH_CP_low();
    }
    ST_CP_high();
}
void output_led_state(unsigned char __led_state)
{
    int i;
    SH_CP_low();
    ST_CP_low();
    for (i=0;i<8;i++)
    {
        if (bit_is_set(__led_state, i))
            DS_high();
        else
            DS_low();
        
        
        SH_CP_high();
        SH_CP_low();
    }
    ST_CP_high();
}