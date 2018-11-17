#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef F_CPU
    #define F_CPU 8000000L	// Oscillator frequency.
#endif

#define BaudRate 9600
#define MYUBRR (F_CPU / 16 / BaudRate ) - 1

void USARTInit(unsigned int ubrr_value, uint8_t x2, uint8_t stopbits)
{
    // Set baud rate
    UBRRL = ubrr_value & 255;
    UBRRH = ubrr_value >> 8;
    
    // Frame Format: asynchronous, 8 data bits, no parity, 1/2 stop bits
    UCSRC = _BV(UCSZ1) | _BV(UCSZ0);
    if(stopbits == 2) UCSRC |= _BV(USBS);
    
    if(x2) UCSRA = _BV(U2X); // 2x
    
    // USART Data Register Empty Interrupt Enable
    UCSRB = _BV(UDRIE);
    
    // Enable The receiver and transmitter
    UCSRB |= _BV(RXEN) | _BV(TXEN);
}

void initSerial()
{
    //Serial Initialization
    
    /*Set baud rate */
    UBRRH = (unsigned char)( (MYUBRR) >> 8);
    UBRRL = (unsigned char) MYUBRR;
    /* Enable receiver and transmitter   */
    UCSRB = (1<<RXEN)|(1<<TXEN);
    /* Frame format: 8data, No parity, 1stop bit */
    UCSRC = (3<<UCSZ0);  
    
}

void USART_Transmit( unsigned char data )
{
    /* Wait for empty transmit buffer */
    while ( !( UCSRA & (1<<UDRE)) )
        ;
    /* Put data into buffer, sends the data */
    UDR = data;
}

void delayLong()
{
    unsigned int delayvar;
    delayvar = 0;
    while (delayvar <=  65500U)
    {
        asm("nop");
        delayvar++;
    }
}


unsigned char serialCheckRxComplete(void)
{
    return( UCSRA & _BV(RXC)) ;		// nonzero if serial data is available to read.
}

unsigned char serialCheckTxReady(void)
{
    return( UCSRA & _BV(UDRE) ) ;		// nonzero if transmit register is ready to receive new data.
}

unsigned char serialRead(void)
{
    while (serialCheckRxComplete() == 0)		// While data is NOT available to read
    {;;}
    return UDR;
}

void serialWrite(unsigned char DataOut)
{
    while (serialCheckTxReady() == 0)		// while NOT ready to transmit
    {;;}
    UDR = DataOut;
}



void establishContact() {
    while (serialCheckRxComplete() == 0) {
        serialWrite('A');
        delayLong();
        delayLong();
        delayLong();
        delayLong();
        delayLong();
        delayLong();
        delayLong();
    }
}

