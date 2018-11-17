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

#include "defs.h"
#include "externs.h"
#include "i2c.h"


char mystr[8];
int temp;
//=================================================================
//                    RTC1307_READ_WRITE                         //
//=================================================================
int Read_RTC(char add)
{
    int temp1;
    I2C_START_TX(0b11010000);
    i2c_transmit(add);
    i2c_start();
    I2C_START_RX(0b11010000);
    temp1 = i2c_receive(0);
    i2c_stop();
    return(temp1);
}

int Write_RTC(char add,char data1)
{
    I2C_START_TX(0b11010000);	//device add.
    i2c_transmit(add);	//Reg. add.
    i2c_transmit(data1);
    i2c_stop();
    return 0;
}

//=======================================================================
//                           SET TIME
//=======================================================================
void SetTime(char HH,char MM, char SS, char ampm)
{
    sprintf(mystr,"%03d",SS);
    Write_RTC(0x00,((mystr[1] - 0x30) << 4) | (mystr[2] - 0x30));
    
    sprintf(mystr,"%03d",MM);
    Write_RTC(0x01,((mystr[1] - 0x30) << 4) | (mystr[2] - 0x30));
    
    
    sprintf(mystr,"%03d",HH);
    if(ampm == 1)
    {
        Write_RTC(0x02,((((mystr[1] - 0x30) << 4) | (mystr[2] - 0x30)) | 0x40) | 0x20);
    }
    else
    {
        Write_RTC(0x02,((((mystr[1] - 0x30) << 4) | (mystr[2] - 0x30)) | 0x40));
    }
}
//=======================================================================
char GetHH()
{
    return Read_RTC(0x02);
}
//=======================================================================
char GetMM()
{
    return (Read_RTC(0x01) & 0x7F);
}
//=======================================================================
char GetSS()
{
    return Read_RTC(0x00);
}
//=======================================================================
//                   SET DATE
//=======================================================================
void SetDate(char DD,char MM, char YY)
{
    sprintf(mystr,"%03d",DD);
    Write_RTC(0x04,((mystr[1] - 0x30) << 4)  | (mystr[2] - 0x30));
    
    sprintf(mystr,"%03d",MM);
    Write_RTC(0x05,((mystr[1] - 0x30) << 4) | (mystr[2] - 0x30));
    
    sprintf(mystr,"%03d",YY);
    Write_RTC(0x06,((mystr[1] - 0x30) << 4)  | (mystr[2] - 0x30));
}
//=======================================================================
char GetDD()
{
    return Read_RTC(0x04);
}
//=======================================================================
char GetMonth()
{
    char j;
    j=Read_RTC(0x05);
    j=(j & 0x0F) + ((j >> 4) * 10);
    return j; //12/11
}
//=======================================================================
char GetYY()
{
    char k;
    k=Read_RTC(0x06);
    k=(k & 0x0F) + ((k >> 4) * 10);
    return k;
}

//=======================================================================
//                    Display Date and Time
//=======================================================================
void DisplayDateTime()
{
    temp = Read_RTC(0x00);
    
    mystr[7]=48+(temp & 0b00001111);
    mystr[6]=48+((temp & 0b01110000)>>4);
    mystr[5]=':';
    
    temp = Read_RTC(0x01);
    
    mystr[4]=48+(temp & 0b00001111);
    mystr[3]=48+((temp & 0b01110000)>>4);
    mystr[2]=':';
    
    temp = Read_RTC(0x02);
    
    mystr[1]=48+(temp & 0b00001111);
    mystr[0]=48+((temp & 0b00010000)>>4);
    dispClock7seg(1, DISP_DURATION, 0+mystr[0]);
    _delay_ms(2000);
    dispClock7seg(1, DISP_DURATION, 1000+mystr[1]);
    _delay_ms(2000);
    dispClock7seg(1, DISP_DURATION, 2000+mystr[2]);
    _delay_ms(2000);
    dispClock7seg(1, DISP_DURATION, 3000+mystr[3]);
    _delay_ms(2000);
    dispClock7seg(1, DISP_DURATION, 4000+mystr[4]);
    _delay_ms(2000);
    dispClock7seg(1, DISP_DURATION, 5000+mystr[5]); //?
    _delay_ms(2000);
    dispClock7seg(1, DISP_DURATION, 6000+mystr[6]); //?
    _delay_ms(2000);
    dispClock7seg(1, DISP_DURATION, 7000+mystr[7]); //?
    _delay_ms(2000);
    
    /*----------------------------------------------
     line1();
     display("Time:");
     
     displaybyte(mystr[0]);
     displaybyte(mystr[1]);
     displaybyte(mystr[2]);
     displaybyte(mystr[3]);
     displaybyte(mystr[4]);
     displaybyte(mystr[5]);
     displaybyte(mystr[6]);
     displaybyte(mystr[7]);
     
     temp = Read_RTC(0x02);
     temp = temp & 0x20;
     if(temp == 0x20)
     {
     display(" PM");
     }
     else
     {
     display(" AM");
     }
     temp = Read_RTC(0x06);
     
     mystr[7]=48+(temp & 0b00001111);
     mystr[6]=48+((temp & 0b01110000)>>4);
     mystr[5]=':';
     
     temp = Read_RTC(0x05);
     
     mystr[4]=48+(temp & 0b00001111);
     mystr[3]=48+((temp & 0b01110000)>>4);
     mystr[2]=':';
     
     temp = Read_RTC(0x04);
     
     mystr[1]=48+(temp & 0b00001111);
     mystr[0]=48+((temp & 0b00110000)>>4);
     
     line2();
     display("Date:");
     
     displaybyte(mystr[0]);
     displaybyte(mystr[1]);
     displaybyte(mystr[2]);
     displaybyte(mystr[3]);
     displaybyte(mystr[4]);
     displaybyte(mystr[5]);
     displaybyte(mystr[6]);
     displaybyte(mystr[7]);
     ----------------------------------------------*/
}
