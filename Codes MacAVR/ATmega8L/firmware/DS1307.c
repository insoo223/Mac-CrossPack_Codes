//=================================================================
/* DS1307 Real Time Clock Software           */
/* 2nd Dec 2014                              */
/* Copyright 2015 Circuits4You.com           */
/* WWW - http://blog.circuits4you.com        */
/* Email - info@circuits4you.com             */

/* LCD Pin-5(R/W) must be connected to ground*/
//=================================================================

//Note: 1. Define Clock in Configuration Opetions
//      2. Define RTC Connections in i2c.h
#include <avr/io.h>
#include <string.h>
#include <util/delay.h>
#include <util/twi.h>

#include "defs.h"
#include "externs.h"
//#include "i2c.h"

#define adrsDS1307  0xD0      // device address of DS1307


/*uC Clock*/
#define uC_Clock 1000000L


/** defines the data direction (reading from I2C device) in i2c_start(),i2c_rep_start() */
#define I2C_READ    1

/** defines the data direction (writing to I2C device) in i2c_start(),i2c_rep_start() */
#define I2C_WRITE   0

/* I2C clock in Hz */
#define SCL_CLOCK  100000L


#define SECONDS_REGISTER   0x00
#define MINUTES_REGISTER   0x01
#define HOURS_REGISTER     0x02
#define DAYOFWK_REGISTER   0x03
#define DAYS_REGISTER      0x04
#define MONTHS_REGISTER    0x05
#define YEARS_REGISTER     0x06


/*************************************************************************
 Initialization of the I2C bus interface. Need to be called only once
 *************************************************************************/
void i2c_init(void)
{
    /* initialize TWI clock: 100 kHz clock, TWPS = 0 => prescaler = 1 */
    
    TWSR = 0;                         /* no prescaler */
    TWBR = ((uC_Clock/SCL_CLOCK)-16)/2;  /* must be > 10 for stable operation */
    
}/* i2c_init */


/*************************************************************************
 Issues a start condition and sends address and transfer direction.
 return 0 = device accessible, 1= failed to access device
 *************************************************************************/
unsigned char i2c_start(unsigned char address)
{
    uint8_t   twst;
    
    // send START condition
    TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
    
    // wait until transmission completed
    //ds:165, TWINT bit is set by hardware when the TWI has finished its current job
    //  and expects application software response.
    while(!(TWCR & (1<<TWINT)))
        ;

    // check value of TWI Status Register. Mask prescaler bits.
    twst = TW_STATUS & 0xF8;
    if ( (twst != TW_START) && (twst != TW_REP_START))
        return 1;
    
    // send device address
    TWDR = address;
    TWCR = (1<<TWINT) | (1<<TWEN);
    
    // wail until transmission completed and ACK/NACK has been received
    while(!(TWCR & (1<<TWINT)));
    
    // check value of TWI Status Register. Mask prescaler bits.
    twst = TW_STATUS & 0xF8;
    if ( (twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK) )
        return 1;
    
    return 0;
    
    
}/* i2c_start */


/*************************************************************************
 Issues a start condition and sends address and transfer direction.
 If device is busy, use ack polling to wait until device is ready
 
 Input:   address and transfer direction of I2C device
 *************************************************************************/
void i2c_start_wait(unsigned char address)
{
    uint8_t   twst;
    
    
    while ( 1 )
    {
        // send START condition
        TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
        
        // wait until transmission completed
        while(!(TWCR & (1<<TWINT)));
        
        // check value of TWI Status Register. Mask prescaler bits.
        twst = TW_STATUS & 0xF8;
        if ( (twst != TW_START) && (twst != TW_REP_START))
            continue;
        
        // send device address
        TWDR = address;
        TWCR = (1<<TWINT) | (1<<TWEN);
        
        // wail until transmission completed
        while(!(TWCR & (1<<TWINT)))
            ;
        
        // check value of TWI Status Register. Mask prescaler bits.
        twst = TW_STATUS & 0xF8;
        if ( (twst == TW_MT_SLA_NACK )||(twst ==TW_MR_DATA_NACK) )
        {
            /* device busy, send stop condition to terminate write operation */
            TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
            
            // wait until stop condition is executed and bus released
            while(TWCR & (1<<TWSTO))
                ;
            
            continue;
        }
        //if( twst != TW_MT_SLA_ACK) return 1;
        break;
    }
    
}/* i2c_start_wait */


/*************************************************************************
 Issues a repeated start condition and sends address and transfer direction
 
 Input:   address and transfer direction of I2C device
 
 Return:  0 device accessible
 1 failed to access device
 *************************************************************************/
unsigned char i2c_rep_start(unsigned char address)
{
    return i2c_start( address );
    
}/* i2c_rep_start */


/*************************************************************************
 Terminates the data transfer and releases the I2C bus
 *************************************************************************/
void i2c_stop(void)
{
    /* send stop condition */
    TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
    
    // wait until stop condition is executed and bus released
    while(TWCR & (1<<TWSTO));
    
}/* i2c_stop */


/*************************************************************************
 Send one byte to I2C device
 
 Input:    byte to be transfered
 Return:   0 write successful
 1 write failed
 *************************************************************************/
unsigned char i2c_write( unsigned char data )
{
    uint8_t   twst;
    
    // send data to the previously addressed device
    TWDR = data;
    TWCR = (1<<TWINT) | (1<<TWEN);
    
    // wait until transmission completed
    while(!(TWCR & (1<<TWINT)));
    
    // check value of TWI Status Register. Mask prescaler bits
    twst = TW_STATUS & 0xF8;
    if( twst != TW_MT_DATA_ACK)
        return 1;
    return 0;
    
}/* i2c_write */

unsigned char i2c_write_reg(unsigned char devReg, unsigned char data )
{
    uint8_t   ret1, ret2;
    
    i2c_start(adrsDS1307);
    // send data to the previously addressed device
    ret1 = i2c_write(devReg);
    ret2 = i2c_write(data);
    i2c_stop();
    return (ret1 + ret2);
    
}/* i2c_write_reg */

/*************************************************************************
 Read one byte from the I2C device, request more data from device
 
 Return:  byte read from I2C device
 *************************************************************************/
unsigned char i2c_readAck(void)
{
    TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
    while(!(TWCR & (1<<TWINT)));
    
    return TWDR;
    
}/* i2c_readAck */


/*************************************************************************
 Read one byte from the I2C device, read is followed by a stop condition
 
 Return:  byte read from I2C device
 *************************************************************************/
unsigned char i2c_readNak(void)
{
    TWCR = (1<<TWINT) | (1<<TWEN);
    while(!(TWCR & (1<<TWINT)))
        ;
    
    return TWDR;
    
}/* i2c_readNak */

unsigned char i2c_read_reg(unsigned char devReg )
{
    uint8_t   ret1, data;
    
    i2c_start(adrsDS1307);
    // send data to the previously addressed device
    ret1 = i2c_write(devReg);

    i2c_start(adrsDS1307+I2C_READ);
    data = i2c_readNak();
    i2c_stop();
    if (ret1 == 0)
        return (data);
    else
        return (200);
    
}/* i2c_read_reg */

//----------------------------------------------------
// Convert normal decimal numbers to binary coded decimal
unsigned char decToBcd(unsigned char val)
{
    return( (val/10*16) + (val%10) );
}

//----------------------------------------------------
// Convert binary coded decimal to normal decimal numbers
unsigned char bcdToDec(unsigned char val)
{
    return( (val/16*10) + (val%16) );
}

//----------------------------------------------------
void setDS1307time(unsigned char second, unsigned char minute, unsigned char hour, unsigned char dayOfWeek, unsigned char dayOfMonth, unsigned char month, unsigned char year)
{
    i2c_write_reg(SECONDS_REGISTER, decToBcd(second)); // set seconds
    i2c_write_reg(MINUTES_REGISTER, decToBcd(minute)); // set minutes
    i2c_write_reg(HOURS_REGISTER, decToBcd(hour)); // set hours
    i2c_write_reg(DAYOFWK_REGISTER, decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
    i2c_write_reg(DAYS_REGISTER, decToBcd(dayOfMonth)); // set date (1 to 31)
    i2c_write_reg(MONTHS_REGISTER, decToBcd(month)); // set month
    i2c_write_reg(YEARS_REGISTER, decToBcd(year)); // set year (0 to 99)
}

//----------------------------------------------------
void readDS1307time(unsigned char *second,
                    unsigned char *minute,
                    unsigned char *hour,
                    unsigned char *dayOfWeek,
                    unsigned char *dayOfMonth,
                    unsigned char *month,
                    unsigned char *year)
{
    // request seven unsigned chars of data from DS1307 starting from register 00h
    *second = bcdToDec(i2c_read_reg(SECONDS_REGISTER) & 0x7f);
    *minute = bcdToDec(i2c_read_reg(MINUTES_REGISTER));
    *hour = bcdToDec(i2c_read_reg(HOURS_REGISTER) & 0x3f);
    if ((*hour >= 17) || (*hour <= 7))
    {
        gNightMode = 1;
        gSingleDigitDelay = 0;
    }
    else
    {
        gNightMode = 0; //false true
        gSingleDigitDelay = SINGLE_DIGIT_DELAY;
    }
    
    *dayOfWeek = bcdToDec(i2c_read_reg(DAYOFWK_REGISTER));
    *dayOfMonth = bcdToDec(i2c_read_reg(DAYS_REGISTER));
    *month = bcdToDec(i2c_read_reg(MONTHS_REGISTER));
    *year = bcdToDec(i2c_read_reg(YEARS_REGISTER));

}//readDS1307time

int mainDS1307(void)
{
    //unsigned char ret;
    unsigned char s,m,h,dw,d,mo,y;

    /*
    //i2c_start(0x9A);       		// set device address and write mode
    ret = i2c_start(adrsDS1307);       		// set device address and write mode
    //dispClock7seg(0, DISP_DURATION, ret); //successfuly checked as 0, Nov22,2015
    ret = i2c_write(0x00);
    //dispClock7seg(0, DISP_DURATION, ret);
    ret = i2c_write_reg(0x01, 2);
    //dispClock7seg(0, DISP_DURATION, ret);
    ret = i2c_read_reg(0x01);
    //dispClock7seg(0, DISP_DURATION, ret);
     ret = i2c_read_reg(HOURS_REGISTER);
     */
    //setDS1307time(40, 21, 10, 2, 23, 11, 15);
    //dispClock7seg(0, DISP_DURATION, ret);
    readDS1307time(&s,&m,&h,&dw,&d,&mo,&y);
    //dispClock7seg(0, DISP_DATE_DURATION, y);
    dispClock7seg(0, DISP_DATE_DURATION, mo*100+d);
    dispClock7seg(0, DISP_DATE_DURATION, dw-1); //Chinese order of numbered week day
    dispClock7seg(0, DISP_CLK_DURATION, h*100+m);
    //dispClock7seg(0, DISP_DATE_DURATION, m*100+s);

    /*
    //i2c_rep_start(0x9B);       // set device address and read mode
    
    PORTB = i2c_readNak();
    
    i2c_stop();
    
    
    
    for(;;)
    {
        
        i2c_start(0x9A);       // set device address and write mode
        
        
        i2c_write(0x00);
        
        i2c_rep_start(0x9B);       // set device address and read mode
        
        PORTB = i2c_readNak();      //Temperature can be read on PORTB pins
        
        i2c_stop();
        
        _delay_ms(1000);
        
        
    }
     */
    return 0;
    
}