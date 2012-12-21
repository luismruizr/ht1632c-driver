/*
 * HT1632C Driver for Arduino by Luis M. Ruiz - stendall@gmail.com
 * http://code.google.com/p/ht1632c-driver/
 * 
 * Licensed as : CC BY-NC-SA 3.0
 * For more details see:
 * http://creativecommons.org/licenses/by-nc-sa/3.0/
 * 
 * This Code is based on the work "Holtech HT1632 Driver Class" from
 * Steven Moughan.
 * http://hackdev.com/sourcecode/ht1632-driver-arduino/
 * 
 * As stated by the license (CC BY_NC_SA 3.0) of the Steven code,
 * and as this is a derived work, Steven it's not responsible, endorse
 * or support this code in any implicit or explicit way. 
 * Constructive criticism, ideas, corrections and patches are welcomed.
 * Any help to make the code support a broader type of modules will be
 * greatly appreciated.
 */

#ifndef HT1632_H_h
#define HT1632_H_h

#if(ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

/* This code makes use of the DigitalWriteFast Library for Arduino:
 * http://code.google.com/p/digitalwritefast/
 * GNU GPL v3
 * If you are low on flash storage and speed its not a must, the following
 * line can be removed. Proved that you replace all calls from DigitalWriteFast
 * to  DigitalWrite in HT1632C.cpp
 */

#include <digitalWriteFast.h>

//The file font.h contains various arrays of fonts. At this time you can one
//select one at compile time.
#define FONT_4x6
#include "font.h"

//Data Mode
#define HT1632_CTL_COMMAND      0x04    //Preceeds all _COMMANDS_ to the system
#define HT1632_CTL_WRITE        0x05    //Write data to the RAM
#define HT1632_CTL_READ         0x06    //Read data from the RAM
//Command Mode
#define HT1632_CMD_SYSDIS       0x00    //Turn off system oscillator and LED duty cycle generator
#define HT1632_CMD_SYSEN        0x01    //Turn on system oscillator
#define HT1632_CMD_LEDOFF       0x02    //Turn off LED duty cycle generator
#define HT1632_CMD_LEDON        0x03    //Turn on LED duty cycle generator
#define HT1632_CMD_BLINKOFF     0x08    //Turn off blinking function
#define HT1632_CMD_BLINKON      0x09    //Turn on blinking function
#define HT1632_CMD_SLAVEMODE    0x10    //Set slave mode and clock source from external clock, the system clock input from OSC pin and synchronous signal input from SYN pin
#define HT1632_CMD_RCMASTER     0x18    //Set master mode anc clock source from on-chip RC oscillator, the system clock output to OSC pin and synchronous signal output SYN pin
#define HT1632_CMD_EXTCLK       0x1C    //System clock source, external
#define HT1632_CMD_COM00        0x20    //N-MOS open drain output and 8 COM option
#define HT1632_CMD_COM01        0x24    //N-MOS open drain output and 8 COM option
#define HT1632_CMD_COM10        0x28    //P-MOS open drain output and 8 COM option
#define HT1632_CMD_COM11        0x2C    //P-MOS open drain output and 16 COM option
//Command Mode - PWM Settings
#define HT1632_PWM_CONTROL			0xA0
#define HT1632_CMD_PWM01        0xA0    //PWM 1/16 Duty
#define HT1632_CMD_PWM02        0xA1    //PWM 2/16 Duty
#define HT1632_CMD_PWM03        0xA2    //PWM 3/16 Duty
#define HT1632_CMD_PWM04        0xA3    //PWM 4/16 Duty
#define HT1632_CMD_PWM05        0xA4    //PWM 5/16 Duty
#define HT1632_CMD_PWM06        0xA5    //PWM 6/16 Duty
#define HT1632_CMD_PWM07        0xA6    //PWM 7/16 Duty
#define HT1632_CMD_PWM08        0xA7    //PWM 8/16 Duty
#define HT1632_CMD_PWM09        0xA8    //PWM 9/16 Duty
#define HT1632_CMD_PWM10        0xA9    //PWM 10/16 Duty
#define HT1632_CMD_PWM11        0xAA    //PWM 11/16 Duty
#define HT1632_CMD_PWM12        0xAB    //PWM 12/16 Duty
#define HT1632_CMD_PWM13        0xAC    //PWM 13/16 Duty
#define HT1632_CMD_PWM14        0xAD    //PWM 14/16 Duty
#define HT1632_CMD_PWM15        0xAE    //PWM 15/16 Duty
#define HT1632_CMD_PWM16        0xAF    //PWM 16/16 Duty
// TODO Support more modules sizes
#define HT1632_MODULE_8X32		0x00    //Each Module have 8x32 pixels wide.
#define HT1632_MODULE_16X24		0x01    //Each Module have 16x24 pixels wide.

class HT1632 {
public:
	HT1632(byte data, byte wclock, byte chip0, byte chip1 = NULL, byte chip2 =
			NULL, byte chip3 = NULL, byte rclock = NULL);
	void init(byte chip = 0, byte mode = HT1632_CMD_COM00, byte module =
			HT1632_MODULE_8X32);


	void setBrightness(byte pwm, byte chip = 0);// Low level command to change the PWM dutycycle in HT1632
	void blinkMode(bool blink = false, byte chip = 0);// Low level command to change the blink attribute in the HT1632
	void chipClear(byte chip = 0); 			// Low level command to clear HT1632 internal buffer
	void writeScreen();						//Dumps the whole screen buffer (Arduino memory)  (1 module or more) to the buffer of HT1632's used.

	/*
	 * All x,y screen coordinates can be negatives and therefore use it to make scroll/displace effects
	 * All following functions draw to Screen buffer (active)
	 * -->
	 */

	void clearScreen(); 					//Clear memory of active buffer. Fill with zeros
	void fillScreen();						//Fill memory of active buffer. Fill with 0xFF
	void setByte(int address, byte d);		//Write a byte 'd' at the memory 'address' of the active screen buffer
	void drawPixel(int x, int y, byte color = 1); //Draws a pixel with the choosed color.
	byte getPixel(int x, int y);			//Return the pixel value at x,y position in screen buffer.
	void setPixel(int x, int y);			//Same that DrawPixel but always lit pixel.
	void clearPixel(int x, int y);			//Same that DrawPixel but always dark pixel.
	void drawLine(int x1, int y1, int x2, int y2, byte color = 1);
	void drawRect(int x, int y, int w, int h, byte color = 1);
	void fillRect(int x, int y, int w, int h, byte color = 1);
	void drawCircle(int x0, int y0, byte r, byte color = 1);
	void fillCircle(int x0, int y0, byte r, byte color = 1);
	void drawChar(int x, int y, char c, byte color = 1); // Puts character 'c' at x,y position.
	void drawString(int x, int y, const char* str, byte color); // Write string 'str' at x,y position.
	void animateDown();						//Move down 1 pixel at time the whole screen buffer content.
	/*
	 * <--
	 */
	void setActiveBuffer(byte b);//Change the active buffer where draw functions operate.
	void swapBuffers();			//Exchange Active (front buffer on DumpScreen) and back buffer. Double buffer.
	byte getActiveBuffer();		//Returns the number of the current buffer;


private:
	byte _DATA;
	byte _WCLOCK;
	byte _RCLOCK;
	byte _CHIP0;
	byte _CHIP1;
	byte _CHIP2;
	byte _CHIP3;
	byte _NMODULES; //number of modules
	byte _MODULE; //model 32x8 or 24x16
	byte _WIDTH; //Sum of all modules Width.
	byte _HEIGHT; //Sum of all modules Height.
	byte _SCREENSIZE; //Bytes, not pixels
	byte * _SCREEN_BUFFER1;
	byte * _SCREEN_BUFFER2;
	byte _BUFFER_ACTIVE;
	byte _BUFFER_MALLOC;

	void chipSelect(byte chip);
	void chipRelease(byte chip);
	void sendCommand(byte command, byte chip = 0);
	void writeBits(byte bits, byte mask);
	byte readData(byte address, byte chip);
	void writeData(byte address, byte data, byte chip = 0);

	void writeSuccesive(byte data);
	void writeSuccesiveStart(byte address, byte chip = 0);
	void writeSuccesiveStop(byte chip = 0);
};

#endif
