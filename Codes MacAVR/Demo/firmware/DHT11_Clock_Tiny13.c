/**************************************************************
 Target MCU & internal clock speed: ATtiny13A @ 9.6Mhz
 Name    : DHT11_Clock_Tiny13.c
 Author  : Insoo Kim (insoo@hotmail.com)
 Date    : April 08, 2015
 
 Description: Read temperature & humidity from DHT11 sensor,
 and display T&H on two digits 7 segment LEDs for around 1 second each, continuously.
 HEX size[Byte]: 728 out of 1024
 
 Ref:
 ** Written for and tested with a custom board with ATtiny13A run on 9.6Mhz int osc
 ** Functions regarding DHT11, i.e."dht_getdata", are refered from the following link of David Gironi.
 ** But, specific time delays have been measured using Hantek USB digital oscilloscope (6022BE) and modified to fit into my custom ATtiny13A board.
 ** http://davidegironi.blogspot.kr/2013/02/reading-temperature-and-humidity-on-avr.html#.VSRiHSlCNSV
 *****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "dht/dht.h"

volatile unsigned char timer_overflow_count = 0;

#define ST_CP_PORT PORTB
#define ST_CP_PIN  2 //latch
#define SH_CP_PORT PORTB
#define SH_CP_PIN  1 //clock
#define DS_PORT    PORTB
#define DS_PIN     0 //data

#define DHT11_PIN 3
#define debug_PIN 4
#define DP  10

#define DS_low()  DS_PORT&=~_BV(DS_PIN)
#define DS_high() DS_PORT|=_BV(DS_PIN)
#define ST_CP_low()  ST_CP_PORT&=~_BV(ST_CP_PIN)
#define ST_CP_high() ST_CP_PORT|=_BV(ST_CP_PIN)
#define SH_CP_low()  SH_CP_PORT&=~_BV(SH_CP_PIN)
#define SH_CP_high() SH_CP_PORT|=_BV(SH_CP_PIN)

//Define functions
//======================
void ioinit(void);
void show7seg(unsigned char);
int8_t dht_getdata(int8_t *, int8_t *);

//======================
char OnesDigit[12]= {
    0b01111110, //0
    0b00000110, //1
    0b10111100, //2
    0b10101110, //3
    0b11000110, //4
    0b11101010, //5
    0b11011010, //6
    0b00100110, //7
    0b11111110, //8
    0b11100110, //9
    0b00000001, //DP (10)
    0b00000000 //Blank (11)
};//OnesDigit[n]

char TensDigit[12] = {
    0b11111010, //0
    0b10010000, //1
    0b01111100, //2
    0b11011100, //3
    0b10010110, //4
    0b11001110, //5
    0b11100110, //6
    0b10011000, //7
    0b11111110, //8
    0b10011110, //9
    0b00000001, //DP (10)
    0b00000000 //Blank (11)
};//TensDigit[n]

void ioinit (void);
void show7seg(unsigned char);


//----------------------------------------------------------
ISR(TIM0_OVF_vect)
{
    if (++timer_overflow_count > 35)
    {   // with 1024/256/64 prescaler, a timer overflow occurs 4.6/18/73 times per second accordingly.
        // Toggle PBn output state
        PORTB ^= 1<<debug_PIN;
        timer_overflow_count = 0;
    }
}//ISR(TIM0_OVF_vect)

//----------------------------------------------------------
int main(void)
{
    
    int8_t temperature = 0;
    int8_t humidity = 0;
    
    ioinit();
    
    // prescale timer to 1/64th the clock rate
    TCCR0B |= (1<<CS01) | (1<<CS00);
    
    // enable timer overflow interrupt
    TIMSK0 |=1<<TOIE0;
    sei();
    
    
    while(1)
    {
        if(dht_gettemperaturehumidity(&temperature, &humidity) != -1)
        {
            show7seg(OnesDigit[temperature % 10] | OnesDigit[DP]); //Ones
            show7seg(TensDigit[temperature / 10]); //Tens
            _delay_ms(170);
            
            show7seg(OnesDigit[humidity % 10]); //Ones
            show7seg(TensDigit[humidity / 10]); //Tens
        } else
        {
            show7seg(OnesDigit[temperature % 10] | OnesDigit[DP]); //Ones
            show7seg(TensDigit[temperature / 10]); //Tens
        }
        
        _delay_ms(90);
    }
    
    return 0;
}//main

//----------------------------------------------------------
void ioinit (void)
{
    //DDRB  = 0xff; //1 = output, 0 = input
    DDRB  = 0x00; //1 = output, 0 = input
    DDRB |= _BV(ST_CP_PIN); //output
    DDRB |= _BV(SH_CP_PIN); //output
    DDRB |= _BV(DS_PIN); //output
    DDRB |= _BV(debug_PIN); //output
    
    DDRB &= ~_BV(DHT11_PIN); //input
    
    PORTB = 0x00;
}//ioinit

//----------------------------------------------------------
void show7seg(unsigned char num)
{
    int i;
    SH_CP_low();
    ST_CP_low();
    for (i=7;i>=0;i--)
    {
        if (bit_is_set(num, i))
            DS_high();
        else
            DS_low();
        
        SH_CP_high();
        SH_CP_low();
    }
    ST_CP_high();
}//show7seg

/*
 * get data from sensor
 */
//----------------------------------------------------------
int8_t dht_getdata(int8_t *temperature, int8_t *humidity)
{
    uint8_t bits[5];
    uint8_t i,j = 0;
    
    //memset(bits, 0, sizeof(bits));
    
    //reset port
    DHT_DDR |= (1<<DHT_INPUTPIN); //output
    DHT_PORT |= (1<<DHT_INPUTPIN); //high
    
    _delay_ms(15); // 124ms by real measurement of OSC
    
    //send request for at least 18ms from MCU
    DHT_PORT &= ~(1<<DHT_INPUTPIN); //low
    _delay_ms(3);//25ms by real measurement of OSC
    
    //check start condition 1
    if((DHT_PIN & (1<<DHT_INPUTPIN)))
    {
        *temperature = 91;
        return -1;
    }
    
    
    DHT_PORT |= (1<<DHT_INPUTPIN); //high
    _delay_us(20);
    //check start condition 2
    if(!(DHT_PIN & (1<<DHT_INPUTPIN)))
    {
        *temperature = 92;
        return -1;
    }
    
    DHT_DDR &= ~(1<<DHT_INPUTPIN); //input
    
    //DHT_PORT |= _BV(debug_PIN); //high debug pin
    //_delay_us(20);
    //DHT_PORT &= ~_BV(debug_PIN); //low debug pin
    
    //read the data
    uint16_t timeoutcounter = 0;
    for (j=0; j<5; j++)
    { //read 5 byte
        uint8_t result=0;
        
        for(i=0; i<8; i++)
        {//read every bit
            timeoutcounter = 0;
            while(!(DHT_PIN & (1<<DHT_INPUTPIN)))
            { //wait for an high input (non blocking)
                timeoutcounter++;
                if(timeoutcounter > DHT_TIMEOUT)
                {
                    *temperature = 93;
                    return -1; //timeout
                }
            }
            
            _delay_us(6); // this is critical time delay to read correct data
            //if input is high after 30 us, get result
            if(DHT_PIN & (1<<DHT_INPUTPIN))
                result |= (1<<(7-i));
            
            timeoutcounter = 0;
            while(DHT_PIN & (1<<DHT_INPUTPIN))
            { //wait until input get low (non blocking)
                timeoutcounter++;
                if(timeoutcounter > DHT_TIMEOUT)
                {
                    *temperature = 94;
                    return -1; //timeout
                }
            }
        }
        bits[j] = result;
    }
    
    //reset port
    DHT_DDR |= (1<<DHT_INPUTPIN); //output
    DHT_PORT |= (1<<DHT_INPUTPIN); //low
    _delay_ms(15);
    
    //check checksum
    if ((uint8_t)(bits[0] + bits[1] + bits[2] + bits[3]) == bits[4])
    {
        //return temperature and humidity
        if (bits[2] !=0)
        {
            *temperature = bits[2];
            *humidity = bits[0];
        }
        else
        {
            *temperature = 97;
            *humidity = 98;
        }
        
        return 0;
    }
    
    return -1;
}//dht_getdata

/*
 * get temperature
 */
//----------------------------------------------------------
int8_t dht_gettemperature(int8_t *temperature)
{
    int8_t humidity = 0;
    return dht_getdata(temperature, &humidity);
}//dht_gettemperature

/*
 * get humidity
 */
int8_t dht_gethumidity(int8_t *humidity)
{
    int8_t temperature = 0;
    return dht_getdata(&temperature, humidity);
}//dht_gethumidity

/*
 * get temperature and humidity
 */
//----------------------------------------------------------
int8_t dht_gettemperaturehumidity(int8_t *temperature, int8_t *humidity)
{
    return dht_getdata(temperature, humidity);
}//dht_gettemperaturehumidity



