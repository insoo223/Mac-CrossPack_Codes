/**************************************************************
 Target MCU & internal clock speed: ATtiny85 @ 8Mhz
 Name    : nRF_Tx_Tiny85.c
 Author  : Insoo Kim (insoo@hotmail.com)
 Date    : April 10, 2015
 
 Description:
 HEX size[Byte]:  out of 8096
 
 Ref:
 * RF_Tranceiver.c
 *
 * Created: 2012-08-10 15:24:35
 *  Author: Kalle
 *  Atmega88

 ** http://gizmosnack.blogspot.kr/2013/04/tutorial-nrf24l01-and-avr.html
 ** https://gist.github.com/klalle/5652658
 *****************************************************************/

#include <avr/io.h>
#include <stdio.h>
#define F_CPU 8000000UL  // 8 MHz
#include <util/delay.h>
#include <avr/interrupt.h>

#include "nRF24L01.h"

#define dataLen 3  //lÃ¤ngd pÃ¥ datapacket som skickas/tas emot
uint8_t *data;
uint8_t *arr;


/*****************Ã¤ndrar klockan till 8MHz ist fÃ¶r 1MHz*****************************/
void clockprescale(void)
{
    CLKPR = 0b10000000;	//Prepare the chip for a change of clock prescale (CLKPCE=1 and the rest zeros)
    CLKPR = 0b00000000;	//Wanted clock prescale (CLKPCE=0 and the four first bits CLKPS0-3 sets division factor = 1)
    //See page 38 in datasheet
}
////////////////////////////////////////////////////


/*****************USART*****************************/  //Skickar data frÃ¥n chip till com-port simulator pÃ¥ datorn
//Initiering

void usart_init(void)
{
    DDRD |= (1<<1);	//Set TXD (PD1) as output for USART
    
    unsigned int USART_BAUDRATE = 9600;		//Same as in "terminal.exe"
    unsigned int ubrr = (((F_CPU / (USART_BAUDRATE * 16UL))) - 1);	//baud prescale calculated according to F_CPU-define at top
    
    /*Set baud rate */
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;
    
    /*	Enable receiver and transmitter */
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
    
    /* Set frame format: 8data, 2stop bit, The two stop-bits does not seem to make any difference in my case!?*/
    UCSR0C = (1<<USBS0)|(3<<UCSZ00);
    
}

//Funktionen som skickar ivÃ¤g byten till datorn

void USART_Transmit(uint8_t data)
{
    /* Wait for empty transmit buffer */
    while ( !( UCSR0A & (1<<UDRE0)) );
    /* Put data into buffer, sends the data */
    UDR0 = data;
}

//Funktionen som Tar emot kommandon av datorn som senare ska skickas till transmittern


uint8_t USART_Receive( void )
{
    /* Wait for data to be received */
    while ( !(UCSR0A & (1<<RXC0)) );	//This loop is only needed if you not use the interrupt...
    
    /* Get and return received data from buffer */
    return UDR0; //Return the received byte
}

/*****************SPI*****************************/  //Skickar data mellan chip och nrf'ens chip
//initiering
void InitSPI(void)
{
    //Set SCK (PB5), MOSI (PB3) , CSN (SS & PB2) & C  as outport
    //OBS!!! MÃ¥ste sÃ¤ttas innan SPI-Enable neadn
    DDRB |= (1<<DDB5) | (1<<DDB3) | (1<<DDB2) |(1<<DDB1);
    
    /* Enable SPI, Master, set clock rate fck/16 .. kan Ã¤ndra hastighet utan att det gÃ¶r sÃ¥ mycket*/
    SPCR |= (1<<SPE)|(1<<MSTR);// |(1<<SPR0) |(1<<SPR1);
    
    SETBIT(PORTB, 2);	//CSN IR_High to start with, vi ska inte skicka nÃ¥t till nrf'en Ã¤nnu!
    CLEARBIT(PORTB, 1);	//CE low to start with, nrf'en ska inte sÃ¤nda/ta emot nÃ¥t Ã¤nnu!
}

//Skickar kommando till nrf'en Ã¥ fÃ¥r dÃ¥ tillbaka en byte
char WriteByteSPI(unsigned char cData)
{
    //Load byte to Data register
    SPDR = cData;
    
    /* Wait for transmission complete */
    while(!(SPSR & (1<<SPIF)));
    
    //Returnera det som sÃ¤nts tillbaka av nrf'en (fÃ¶rsta gÃ¥ngen efter csn-lÃ¥g kommer Statusregistert)
    return SPDR;
}
////////////////////////////////////////////////////


/*****************in/out***************************/  //stÃ¤ll in t.ex. LED
//sÃ¤tter alla I/0 portar fÃ¶r t.ex. LED
void ioinit(void)
{
    DDRB |= (1<<DDB0); //led
}
////////////////////////////////////////////////////


/*****************interrupt***************************/ //orsaken till att kÃ¶ra med interrupt Ã¤r att de avbryter koden var den Ã¤n Ã¤r och kÃ¶r detta som Ã¤r viktigast!
//nÃ¤r data tas emot/skickas sÃ¥ gÃ¥r interr uptet INT0 nÃ¤st lÃ¤ngst ner igÃ¥ng
void INT0_interrupt_init(void)
{
    DDRD &= ~(1<<DDD2);	//Extern interrupt pÃ¥ INT0, dvs sÃ¤tt den till input!
    
    EICRA |=  (1<<ISC01);// INT0 falling edge	PD2
    EICRA  &=  ~(1<<ISC00);// INT0 falling edge	PD2
    
    EIMSK |=  (1<<INT0);	//enablar int0
    //sei();              // Enable global interrupts gÃ¶rs sen
}

//nÃ¤r chipets RX (usart) fÃ¥r ett meddelande fÃ¥rn datorn gÃ¥r interruptet USART_RX igÃ¥ng lÃ¤ngst ner.

void USART_interrupt_init(void)
{
    UCSR0B |= (1<<RXCIE0);	//Enable interrupt that triggers on USART-data is received,
}

//////////////////////////////////////////////////////

//funktion fÃ¶r att hÃ¤mta nÃ¥t av nrf's register
uint8_t GetReg(uint8_t reg)
{
    //andvÃ¤ndning: USART_Transmit(GetReg(STATUS)); //dÃ¤r status Ã¤r registret du vill kolla
    _delay_us(10);
    CLEARBIT(PORTB, 2);	//CSN low
    _delay_us(10);
    WriteByteSPI(R_REGISTER + reg);	//Vilket register vill du lÃ¤sa (nu med R_Register fÃ¶r att inget ska skrivas till registret)
    _delay_us(10);
    reg = WriteByteSPI(NOP);	//Skicka NOP antalet byte som du vill hÃ¤mta (oftast 1gÃ¥ng, men t.ex addr Ã¤r 5 byte!) och spara isf inte i "reg" utan en array med en loop
    _delay_us(10);
    SETBIT(PORTB, 2);	//CSN IR_High
    return reg;	// Returnerar registret fÃ¶rhoppningsvis med bit5=1 (tx_ds=lyckad sÃ¤ndning)
}


/*****************nrf-setup***************************/ //StÃ¤ller in nrf'en genoma att fÃ¶rst skicka vilket register, sen vÃ¤rdet pÃ¥ registret.
uint8_t *WriteToNrf(uint8_t ReadWrite, uint8_t reg, uint8_t *val, uint8_t antVal)	//tar in "ReadWrite" (W el R), "reg" (ett register), "*val" (en array) & "antVal" (antal integer i variabeln)
{
    cli();	//disable global interrupt
    
    if (ReadWrite == W)	//W=vill skriva till nrf-en (R=lÃ¤sa av den, R_REGISTER (0x00) ,sÃ¥ skiter i en else funktion)
    {
        reg = W_REGISTER + reg;	//ex: reg = EN_AA: 0b0010 0000 + 0b0000 0001 = 0b0010 0001
    }
    
    //Static uint8_t fÃ¶r att det ska gÃ¥ att returnera en array (lÃ¤gg mÃ¤rke till "*" uppe pÃ¥ funktionen!!!)
    static uint8_t ret[dataLen];	//antar att det lÃ¤ngsta man vill lÃ¤sa ut nÃ¤r man kallar pÃ¥ "R" Ã¤r dataleng-lÃ¥ngt, dvs anvÃ¤nder man bara 1byte datalengd Ã¥ vill lÃ¤sa ut 5byte RF_Adress sÃ¥ skriv 5 hÃ¤r ist!!!
    
    _delay_us(10);		//alla delay Ã¤r sÃ¥ att nrfen ska hinna med! (microsekunder)
    CLEARBIT(PORTB, 2);	//CSN low = nrf-chippet bÃ¶rjar lyssna
    _delay_us(10);
    WriteByteSPI(reg);	//fÃ¶rsta SPI-kommandot efter CSN-lÃ¥g berÃ¤ttar fÃ¶r nrf'en vilket av dess register som ska redigeras ex: 0b0010 0001 write to registry EN_AA
    _delay_us(10);
    
    int i;
    for(i=0; i<antVal; i++)
    {
        if (ReadWrite == R && reg != W_TX_PAYLOAD)
        {
            ret[i]=WriteByteSPI(NOP);	//Andra och resten av SPI kommandot sÃ¤ger Ã¥t nrfen vilka vÃ¤rden som i det hÃ¤r fallet ska lÃ¤sas
            _delay_us(10);
        }
        else
        {
            WriteByteSPI(val[i]);	//Andra och resten av SPI kommandot sÃ¤ger Ã¥t nrfen vilka vÃ¤rden som i det hÃ¤r fallet ska skrivas till
            _delay_us(10);
        }
    }
    SETBIT(PORTB, 2);	//CSN IR_High = nrf-chippet slutar lyssna
    
    sei(); //enable global interrupt
    
    return ret;	//returnerar en array
}

//initierar nrf'en (obs nrfen mÃ¥ste vala i vila nÃ¤r detta sker CE-lÃ¥g)
void nrf24L01_init(void)
{
    _delay_ms(100);	//allow radio to reach power down if shut down
    
    uint8_t val[5];	//en array av integers som skickar vÃ¤rden till WriteToNrf-funktionen
    
    //EN_AA - (auto-acknowledgements) - Transmittern fÃ¥r svar av recivern att packetet kommit fram, grymt!!! (behÃ¶ver endast vara enablad pÃ¥ Transmittern!)
    //KrÃ¤ver att Transmittern Ã¤ven har satt SAMMA RF_Adress pÃ¥ sin mottagarkanal nedan ex: RX_ADDR_P0 = TX_ADDR
    val[0]=0x01;	//ger fÃ¶rsta integern i arrayen "val" ett vÃ¤rde: 0x01=EN_AA pÃ¥ pipe P0.
    WriteToNrf(W, EN_AA, val, 1);	//W=ska skriva/Ã¤ndra nÃ¥t i nrfen, EN_AA=vilket register ska Ã¤ndras, val=en array med 1 till 32 vÃ¤rden  som ska skrivas till registret, 1=antal vÃ¤rden som ska lÃ¤sas ur "val" arrayen.
    
    //SETUP_RETR (the setup for "EN_AA")
    val[0]=0x2F;	//0b0010 00011 "2" sets it up to 750uS delay between every retry (at least 500us at 250kbps and if payload >5bytes in 1Mbps, and if payload >15byte in 2Mbps) "F" is number of retries (1-15, now 15)
    WriteToNrf(W, SETUP_RETR, val, 1);
    
    //VÃ¤ljer vilken/vilka datapipes (0-5) som ska vara igÃ¥ng.
    val[0]=0x01;
    WriteToNrf(W, EN_RXADDR, val, 1); //enable data pipe 0
    
    //RF_Adress width setup (hur mÃ¥nga byte ska RF_Adressen bestÃ¥ av? 1-5 bytes) (5bytes sÃ¤krare dÃ¥ det finns stÃ¶rningar men lÃ¥ngsammare dataÃ¶verfÃ¶ring) 5addr-32data-5addr-32data....
    val[0]=0x03;
    WriteToNrf(W, SETUP_AW, val, 1); //0b0000 00011 motsvarar 5byte RF_Adress
    
    //RF channel setup - vÃ¤ljer frekvens 2,400-2,527GHz 1MHz/steg
    val[0]=0x01;
    WriteToNrf(W, RF_CH, val, 1); //RF channel registry 0b0000 0001 = 2,401GHz (samma pÃ¥ TX Ã¥ RX)
    
    //RF setup	- vÃ¤ljer effekt och Ã¶verfÃ¶ringshastighet
    val[0]=0x07;
    WriteToNrf(W, RF_SETUP, val, 1); //00000111 bit 3="0" ger lÃ¤gre Ã¶verfÃ¶ringshastighet 1Mbps=LÃ¤ngre rÃ¤ckvidd, bit 2-1 ger effektlÃ¤ge hÃ¶g (-0dB) ("11"=(-18dB) ger lÃ¤gre effekt =strÃ¶msnÃ¥lare men lÃ¤gre range)
    
    //RX RF_Adress setup 5 byte - vÃ¤ljer RF_Adressen pÃ¥ Recivern (MÃ¥ste ges samma RF_Adress om Transmittern har EN_AA pÃ¥slaget!!!)
    int i;
    for(i=0; i<5; i++)
    {
        val[i]=0x12;	//RF channel registry 0b10101011 x 5 - skriver samma RF_Adress 5ggr fÃ¶r att fÃ¥ en lÃ¤tt och sÃ¤ker RF_Adress (samma pÃ¥ transmitterns chip!!!)
    }
    WriteToNrf(W, RX_ADDR_P0, val, 5); //0b0010 1010 write registry - eftersom vi valde pipe 0 i "EN_RXADDR" ovan, ger vi RF_Adressen till denna pipe. (kan ge olika RF_Adresser till olika pipes och dÃ¤rmed lyssna pÃ¥ olika transmittrar)
    
    //TX RF_Adress setup 5 byte -  vÃ¤ljer RF_Adressen pÃ¥ Transmittern (kan kommenteras bort pÃ¥ en "ren" Reciver)
    //int i; //Ã¥teranvÃ¤nder fÃ¶regÃ¥ende i...
    for(i=0; i<5; i++)
    {
        val[i]=0x12;	//RF channel registry 0b10111100 x 5 - skriver samma RF_Adress 5ggr fÃ¶r att fÃ¥ en lÃ¤tt och sÃ¤ker RF_Adress (samma pÃ¥ Reciverns chip och pÃ¥ RX-RF_Adressen ovan om EN_AA enablats!!!)
    }
    WriteToNrf(W, TX_ADDR, val, 5);
    
    // payload width setup - Hur mÃ¥nga byte ska skickas per sÃ¤ndning? 1-32byte
    val[0]=dataLen;		//"0b0000 0001"=1 byte per 5byte RF_Adress  (kan vÃ¤lja upp till "0b00100000"=32byte/5byte RF_Adress) (definierat hÃ¶gst uppe i global variabel!)
    WriteToNrf(W, RX_PW_P0, val, 1);
    
    //CONFIG reg setup - Nu Ã¤r allt instÃ¤llt, boota upp nrf'en och gÃ¶r den antingen Transmitter lr Reciver
    val[0]=0x1E;  //0b0000 1110 config registry	bit "1":1=power up,  bit "0":0=transmitter (bit "0":1=Reciver) (bit "4":1=>mask_Max_RT,dvs IRQ-vektorn reagerar inte om sÃ¤ndningen misslyckades.
    WriteToNrf(W, CONFIG, val, 1);
    
    //device need 1.5ms to reach standby mode
    _delay_ms(100);
    
    //sei();
}

void ChangeAddress(uint8_t adress)
{
    _delay_ms(100);
    uint8_t val[5];
    //RX RF_Adress setup 5 byte - vÃ¤ljer RF_Adressen pÃ¥ Recivern (MÃ¥ste ges samma RF_Adress om Transmittern har EN_AA pÃ¥slaget!!!)
    int i;
    for(i=0; i<5; i++)
    {
        val[i]=adress;	//RF channel registry 0b10101011 x 5 - skriver samma RF_Adress 5ggr fÃ¶r att fÃ¥ en lÃ¤tt och sÃ¤ker RF_Adress (samma pÃ¥ transmitterns chip!!!)
    }
    WriteToNrf(W, RX_ADDR_P0, val, 5); //0b0010 1010 write registry - eftersom vi valde pipe 0 i "EN_RXADDR" ovan, ger vi RF_Adressen till denna pipe. (kan ge olika RF_Adresser till olika pipes och dÃ¤rmed lyssna pÃ¥ olika transmittrar)
    
    //TX RF_Adress setup 5 byte -  vÃ¤ljer RF_Adressen pÃ¥ Transmittern (kan kommenteras bort pÃ¥ en "ren" Reciver)
    //int i; //Ã¥teranvÃ¤nder fÃ¶regÃ¥ende i...
    for(i=0; i<5; i++)
    {
        val[i]=adress;	//RF channel registry 0b10111100 x 5 - skriver samma RF_Adress 5ggr fÃ¶r att fÃ¥ en lÃ¤tt och sÃ¤ker RF_Adress (samma pÃ¥ Reciverns chip och pÃ¥ RX-RF_Adressen ovan om EN_AA enablats!!!)
    }
    WriteToNrf(W, TX_ADDR, val, 5);
    _delay_ms(100);
}
/////////////////////////////////////////////////////

/*****************Funktioner***************************/ //Funktioner som anvÃ¤nds i main
//Resettar nrf'en fÃ¶r ny kommunikation
void reset(void)
{
    _delay_us(10);
    CLEARBIT(PORTB, 2);	//CSN low
    _delay_us(10);
    WriteByteSPI(W_REGISTER + STATUS);	//
    _delay_us(10);
    WriteByteSPI(0b01110000);	//radedrar alla irq i statusregistret (fÃ¶r att kunna lyssna igen)
    _delay_us(10);
    SETBIT(PORTB, 2);	//CSN IR_High
}

//Reciverfunktioner
/*********************Reciverfunktioner********************************/
//Ã¶ppnar Recivern och "Lyssnar" i 1s
void receive_payload(void)
{
    sei();		//Enable global interrupt
    
    SETBIT(PORTB, 1);	//CE IR_High = "Lyssnar"
    _delay_ms(1000);	//lyssnar i 1s och om mottaget gÃ¥r int0-interruptvektor igÃ¥ng
    CLEARBIT(PORTB, 1); //ce lÃ¥g igen -sluta lyssna
    
    cli();	//Disable global interrupt
}

//SÃ¤nd data
void transmit_payload(uint8_t * W_buff)
{
    WriteToNrf(R, FLUSH_TX, W_buff, 0); //skickar 0xE1 som flushar registret fÃ¶r att gammal data inte ska ligga Ã¥ vÃ¤nta pÃ¥ att bli skickad nÃ¤r man vill skicka ny data! R stÃ¥r fÃ¶r att W_REGISTER inte ska lÃ¤ggas till. skickar inget kommando efterrÃ¥t eftersom det inte behÃ¶vs! W_buff[]stÃ¥r bara dÃ¤r fÃ¶r att en array mÃ¥ste finnas dÃ¤r...
    
    WriteToNrf(R, W_TX_PAYLOAD, W_buff, dataLen);	//skickar datan i W_buff till nrf-en (obs gÃ¥r ej att lÃ¤sa w_tx_payload-registret!!!)
    
    sei();	//enable global interrupt- redan pÃ¥!
    //USART_Transmit(GetReg(STATUS));
    
    _delay_ms(10);		//behÃ¶ver det verkligen vara ms Ã¥ inte us??? JAAAAAA! annars funkar det inte!!!
    SETBIT(PORTB, 1);	//CE hÃ¶g=sÃ¤nd data	INT0 interruptet kÃ¶rs nÃ¤r sÃ¤ndningen lyckats och om EN_AA Ã¤r pÃ¥, ocksÃ¥ svaret frÃ¥n recivern Ã¤r mottagen
    _delay_us(20);		//minst 10us!
    CLEARBIT(PORTB, 1);	//CE lÃ¥g
    _delay_ms(10);		//behÃ¶ver det verkligen vara ms Ã¥ inte us??? JAAAAAA! annars funkar det inte!!!
    
    //cli();	//Disable global interrupt... ajabaja, dÃ¥ stÃ¤ngs USART_RX-lyssningen av!
    
}


/////////////////////////////////////////////////////

int main(void)
{
    clockprescale();
    usart_init();
    InitSPI();
    ioinit();
    INT0_interrupt_init();
    USART_interrupt_init();
    
    nrf24L01_init();
    
    SETBIT(PORTB,0);		//FÃ¶r att se att dioden fungerar!
    _delay_ms(1000);
    CLEARBIT(PORTB,0);
    
    while(1)
    {
        //Wait for USART-interrupt to send data...
        
    }
    return 0;
}




ISR(INT0_vect)	//vektorn som gÃ¥r igÃ¥ng nÃ¤r transmit_payload lyckats sÃ¤nda eller nÃ¤r receive_payload fÃ¥tt data OBS: dÃ¥ Mask_Max_rt Ã¤r satt i config registret sÃ¥ gÃ¥r den inte igÃ¥ng nÃ¤r MAX_RT Ã¤r uppnÃ¥d Ã¥ sÃ¤ndninge nmisslyckats!
{
    cli();	//Disable global interrupt
    CLEARBIT(PORTB, 1);		//ce lÃ¥g igen -sluta lyssna/sÃ¤nda
    
    SETBIT(PORTB, 0); //led on
    _delay_ms(500);
    CLEARBIT(PORTB, 0); //led off
    
    //Receiver function to print out on usart:
    //data=WriteToNrf(R, R_RX_PAYLOAD, data, dataLen);	//lÃ¤s ut mottagen data
    //reset();
    //
    //for (int i=0;i<dataLen;i++)
    //{
    //USART_Transmit(data[i]);
    //}
    //
    sei();
    
}

ISR(USART_RX_vect)	///Vector that triggers when computer sends something to the Atmega88
{
    uint8_t W_buffer[dataLen];	//Creates a buffer to receive data with specified length (ex. dataLen = 5 bytes)
    
    int i;
    for (i=0;i<dataLen;i++)
    {
        W_buffer[i]=USART_Receive();	//receive the USART
        USART_Transmit(W_buffer[i]);	//Transmit the Data back to the computer to make sure it was correctly received
        //This probably should wait until all the bytes is received, but works fine in to send and receive at the same time... =)
    }
    
    reset();	//reset irq - kan skicka data pÃ¥ nytt
    
    if (W_buffer[0]=='9')	//om projektorduk
    {
        ChangeAddress(0x13);	//change address to send to different receiver
        transmit_payload(W_buffer);	//SÃ¤nder datan
        ChangeAddress(0x12);	//tillbaka till ultimata fjÃ¤rrisen
    } 
    else
    {
        transmit_payload(W_buffer);	//SÃ¤nder datan
    }
    
    USART_Transmit('#');	//visar att chipet mottagit datan...
    
    
}