
#ifndef HIGH
#define HIGH 1
#endif

#ifndef LOW
#define LOW 0
#endif

//----------------------------------------------
//--- Pin assignments for tripple 74HC595s
//----------------------------------------------

// Bit location of 74HC595 sector
// A "sector" tells which of three 74HC595 chips holds a corresponding signal: top, middle, or bottom
#ifndef SECTOR_BIT_LOCATION_74HC595
#define SECTOR_BIT_LOCATION_74HC595   4
#endif

// bit mask for a corresponding signal ID
#ifndef DATA_BITS_MASK_74HC595
#define DATA_BITS_MASK_74HC595   0x07
#endif

#ifndef halfSec
#define halfSec 75
#endif

#ifndef latchPort
#define latchPort PORTB
#endif

#ifndef clockPort
#define clockPort PORTB
#endif

#ifndef dataPort
#define dataPort  PORTB
#endif

#ifndef BUZZ
#define BUZZ 0x04
#endif

#ifndef ONES_GND
#define ONES_GND ~(0x01) // grounding of the Ones digit of common cathde 7 segment LED
#endif

#ifndef TENS_GND
#define TENS_GND ~(0x02) // grounding of the Tens digit of common cathde 7 segment LED
#endif

#ifndef TWO_DIGIT_GND
#define TWO_DIGIT_GND ~(0x03) // grounding of the Ones & Tens digit of common cathde 7 segment LED
#endif

#ifndef OFF_7SEG
#define OFF_7SEG 255
#endif

#ifndef DS_low
#define DS_low()  dataPort&=~_BV(datPin)
#endif

#ifndef DS_high
#define DS_high() dataPort|=_BV(datPin)
#endif

#ifndef ST_CP_low
#define ST_CP_low()  latchPort&=~_BV(latPin)
#endif

#ifndef ST_CP_high
#define ST_CP_high() latchPort|=_BV(latPin)
#endif

#ifndef SH_CP_low
#define SH_CP_low()  clockPort&=~_BV(ckPin)
#endif

#ifndef SH_CP_high
#define SH_CP_high() clockPort|=_BV(ckPin)
#endif

#ifndef buzzPin
#define buzzPin 0x02    // 5V buzzer
#endif

#ifndef DISP_DURATION
#define DISP_DURATION 100
#endif

#ifndef DISP_DURATION_LONG
#define DISP_DURATION_LONG 200
#endif

#ifndef NIGHTMODE_DURATION
#define NIGHTMODE_DURATION 1 //[ms]
#endif
