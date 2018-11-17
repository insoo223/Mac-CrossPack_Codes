#include <avr/io.h>
#include <util/delay.h>

int main(void)
{ 
    char i;
    DDRB = 0b00000111;           /* make the LED pin an output */

    for(;;)
    {
        for(i=0; i<3; i++)
        {
            PORTB ^= 1 << i;    /* toggle the LED */
            _delay_ms(50);  /* max is 262.14 ms / F_CPU in MHz */
        }
    }
    
    return 0; // never reach here
}