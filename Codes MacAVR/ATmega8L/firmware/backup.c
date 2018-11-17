
/*------------------------------
 #define E 		PD7
 #define RS 		PB0
 
 

 void display(char string[16]);
 void displaybyte(char D);
 void dispinit(void);
 void epulse(void);
 void delay_ms(unsigned int de);
 void line1();
 void line2();
 --------------------------------*/



/**********************************************************************************
 //=================================================================
 //        Main Function
 //=================================================================
 int mainDS1307(void)
 {
	int set;
 
	DDRB = 0xE1;		//Set LCD Port Direction
	DDRD = 0xE0;
	PORTB = 0x06; //Pull up for switches
 
 delay_ms(500);		//Initiaize LCD
 dispinit();
 delay_ms(200);
 while(1)
 {
 //Change Date/Time
 if((PINB & 0x02)==0x00) //Time Set Switch is pressed
 {
 SetTime(11,11,11, 1);
 }
 
 if((PINB & 0x04)==0x00) //Date Switch is pressed
 {
 SetDate(21,12,12);
 }
 DisplayDateTime();
 
 }
 }
 
 //=================================================================
 //        LCD Display Initialization Function
 //=================================================================
 void dispinit(void)
 {
	int count;
	char init[]={0x43,0x03,0x03,0x02,0x28,0x01,0x0C,0x06,0x02,0x02};
 
	PORTB &= ~(1<<RS);           // RS=0
	for (count = 0; count <= 9; count++)
 {
	displaybyte(init[count]);
 }
	PORTB |= 1<<RS;				//RS=1
 }
 
 
 //=================================================================
 //        Enable Pulse Function
 //=================================================================
 void epulse(void)
 {
	PORTD |= 1<<E;
 delay_ms(10);	//Adjust delay if required
	PORTD &= ~(1<<E);
	delay_ms(10);	//Adjust delay if required
 }
 
 
 //=================================================================
 //        Send Single Byte to LCD Display Function
 //=================================================================
 void displaybyte(char D)
 {
 //D4=PD6
 //D5=PD5
 //D6=PB7
 //D7=PB6
	//data is in Temp Register
 char K1;
 K1=D;
 K1=K1 & 0xF0;
 K1=K1 >> 4;		//Send MSB
 
 PORTD &= 0x9F;  //Clear data pins
 PORTB &= 0x3F;
 
 if((K1 & 0x01)==0x01){PORTD |= (1<<PD6);}
 if((K1 & 0x02)==0x02){PORTD |= (1<<PD5);}
 if((K1 & 0x04)==0x04){PORTB |= (1<<PB7);}
 if((K1 & 0x08)==0x08){PORTB |= (1<<PB6);}
 
	epulse();
 
 K1=D;
 K1=K1 & 0x0F; 	//Send LSB
 PORTD &= 0x9F;  //Clear data pins
 PORTB &= 0x3F;
 
 if((K1 & 0x01)==0x01){PORTD |= (1<<PD6);}
 if((K1 & 0x02)==0x02){PORTD |= (1<<PD5);}
 if((K1 & 0x04)==0x04){PORTB |= (1<<PB7);}
 if((K1 & 0x08)==0x08){PORTB |= (1<<PB6);}
	epulse();
 }
 
 //=================================================================
 //        Display Line on LCD at desired location Function
 //=================================================================
 void display(char string[16])
 {
 int len,count;
 
	PORTB |= (1<<RS);           // RS=1 Data Mode
 len = strlen(string);
 
 for (count=0;count<len;count++)
 {
 displaybyte(string[count]);
	}
 }
 
 void line1()
 {
 PORTB &= ~(1<<RS);           // RS=0 Command Mode
 displaybyte(0x80);		//Move Coursor to Line 1
 PORTB |= (1<<RS);           // RS=1 Data Mode
 }
 void line2()
 {
 PORTB &= ~(1<<RS);           // RS=0 Command Mode
 displaybyte(0xC0);		//Move Coursor to Line 2
 PORTB |= (1<<RS);           // RS=1 Data Mode
 }
 //=================================================================
 //        Delay Function
 //=================================================================
 void delay_ms(unsigned int de)
 {
 unsigned int rr,rr1;
 for (rr=0;rr<de;rr++)
 {
 
 for(rr1=0;rr1<30;rr1++)   //395
 {
 asm("nop");
 }
 
 }
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
 }
 **********************************************************************************/

