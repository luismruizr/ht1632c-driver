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

#if(ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include "HT1632C.h"

//#define swap(a, b) { uint16_t t = a; a = b; b = t; }
#define swap(a, b) { uint16_t t = a; a = b; b = t; }

HT1632::HT1632(byte data, byte wclock, byte chip0, byte chip1, byte chip2,
		byte chip3, byte rclock) {
	//Set the I/O Directions
	pinMode(data, OUTPUT);
	pinMode(wclock, OUTPUT);
	if (rclock) {
		pinMode(rclock, OUTPUT);
		_RCLOCK = rclock;
	}

	pinMode(chip0, OUTPUT);
	digitalWriteFast(chip0, HIGH);
	_CHIP0 = chip0;
	_NMODULES = 1;

	if (chip1) {
		pinMode(chip1, OUTPUT);
		digitalWriteFast(chip1, HIGH);
		_CHIP1 = chip1;
		_NMODULES = 2;
	}

	if (chip2) {
		pinMode(chip2, OUTPUT);
		digitalWriteFast(chip2, HIGH);
		_CHIP2 = chip2;
		_NMODULES = 3;
	}

	if (chip3) {
		pinMode(chip3, OUTPUT);
		digitalWriteFast(chip3, HIGH);
		_CHIP2 = chip2;
		_NMODULES = 4;
	}

	_DATA = data;
	_WCLOCK = wclock;
	_BUFFER_MALLOC = false;
}

void HT1632::init(byte chip, byte mode, byte module) {
	sendCommand(HT1632_CMD_SYSDIS); //Disable system
	sendCommand(mode); //PMOS drivers
	sendCommand(HT1632_CMD_RCMASTER); //Master mode
	sendCommand(HT1632_CMD_SYSEN); //System Enable
	sendCommand(HT1632_CMD_LEDON); //Enable the display
	chipClear(chip);

	_MODULE = module;
	if (_BUFFER_MALLOC == false) {
		//Max 192 Bytes =(((16x24)/8) * 4 modules)  ram memory used as buffer.
		if (module == HT1632_MODULE_8X32) {
			_SCREENSIZE = ((8 * 32) / 8) * _NMODULES;
			_SCREEN_BUFFER1 = (byte *) malloc(_SCREENSIZE); //8 x 32 pixels wide / 8 bits in a byte
			_SCREEN_BUFFER2 = (byte *) malloc(_SCREENSIZE); //8 x 32 pixels wide / 8 bits in a byte
			_WIDTH = 32;
			_HEIGHT = 8 * _NMODULES;
		} else {
			_SCREENSIZE = ((16 * 24) / 8) * _NMODULES;
			_SCREEN_BUFFER1 = (byte *) malloc(_SCREENSIZE); //16 x 24 pixels wide / 8 bits in a byte
			_SCREEN_BUFFER2 = (byte *) malloc(_SCREENSIZE); //16 x 24 pixels wide / 8 bits in a byte
			if (_NMODULES > 1) {
				_HEIGHT = 24;
				_WIDTH = 16 * _NMODULES;
			} else {
				_HEIGHT = 16;
				_WIDTH = 24;
			}
		}
	}
	_BUFFER_MALLOC = true;
	_BUFFER_ACTIVE = 0;
	clearScreen();
}

void HT1632::chipSelect(byte chip) {
	switch (chip) {
	case 0:
		digitalWriteFast(_CHIP0, LOW);
		break;
	case 1:
		digitalWriteFast(_CHIP1, LOW);
		break;
	case 2:
		digitalWriteFast(_CHIP2, LOW);
		break;
	case 3:
		digitalWriteFast(_CHIP3, LOW);
		break;
	}
}

void HT1632::chipRelease(byte chip) {
	switch (chip) {
	case 0:
		digitalWriteFast(_CHIP0, HIGH);
		break;
	case 1:
		digitalWriteFast(_CHIP1, HIGH);
		break;
	case 2:
		digitalWriteFast(_CHIP2, HIGH);
		break;
	case 3:
		digitalWriteFast(_CHIP3, HIGH);
		break;
	}
}

void HT1632::sendCommand(byte command, byte chip) {
	chipSelect(chip);
	writeBits(HT1632_CTL_COMMAND, 1 << 2); //3 bit command id
	writeBits(command, 1 << 7); //8 bit command
	writeBits(0, 1); //There's one extra bit in commands that dont matter...
	chipRelease(chip);
}

void HT1632::writeBits(byte bits, byte mask) {
	while (mask) {
		digitalWriteFast(_WCLOCK, LOW);
		if (bits & mask) {
			digitalWriteFast(_DATA, HIGH);
		} else {
			digitalWriteFast(_DATA, LOW);
		}
		digitalWriteFast(_WCLOCK, HIGH);
		mask >>= 1;
	}
}

void HT1632::chipClear(byte chip) {
	chipSelect(chip);
	for (int i = 0; i < 256; i++) {
		writeData(i, 0);
	}
	chipRelease(chip);
}

void HT1632::setBrightness(byte pwm, byte chip) {
	sendCommand(HT1632_PWM_CONTROL | (pwm & 0xF), chip);
}

byte HT1632::readData(byte address, byte chip) {
	//Do stuff here to read the data from the chip...
	return (false);
}

void HT1632::writeData(byte address, byte data, byte chip) {
	chipSelect(chip); //Select the chip...
	//Send the WRITE command...
	writeBits(HT1632_CTL_WRITE, 1 << 2);  //1<<2   3 bit command
	//Send the Address...
	writeBits(address, 1 << 6);   //1<<6  7 bit address
	//Send the data...
	writeBits(data, 1 << 3);    //1<<3    4 bit data
	chipRelease(chip);    //Release the chip...
}

void HT1632::writeSuccesive(byte data) {
	writeBits(data, 1 << 3);    //1<<3    4 bit data
}

void HT1632::writeSuccesiveStart(byte address, byte chip) {
	chipSelect(chip); //Select the chip...
	//Send the WRITE command...
	writeBits(HT1632_CTL_WRITE, 1 << 2);  //1<<2   3 bit command
	//Send the Address...
	writeBits(address, 1 << 6);   //1<<6  7 bit address
}

void HT1632::writeSuccesiveStop(byte chip) {
	chipRelease(chip);    //Release the chip...
}

void HT1632::blinkMode(bool blink, byte chip) {
	if (blink) {
		sendCommand(HT1632_CMD_BLINKON, chip);
	} else {
		sendCommand(HT1632_CMD_BLINKOFF, chip);
	}
}

/*********************************************************/

void HT1632::clearScreen() {
	if (_BUFFER_ACTIVE == 0)
		memset((void *) _SCREEN_BUFFER1, 0, _SCREENSIZE);
	else
		memset((void *) _SCREEN_BUFFER2, 0, _SCREENSIZE);
}

void HT1632::fillScreen() {
	if (_BUFFER_ACTIVE == 0)
		memset((void *) _SCREEN_BUFFER1, 255, _SCREENSIZE);
	else
		memset((void *) _SCREEN_BUFFER2, 255, _SCREENSIZE);
}

//TODO Check the posibility of doing with SPI instead of bitbanging.
// http://arduino.cc/en/Reference/SPI
void HT1632::writeScreen() { //TODO Support more than 1 module.
	byte data;
	byte * buffer;

	if (_BUFFER_ACTIVE == 0)
		buffer = _SCREEN_BUFFER1;
	else
		buffer = _SCREEN_BUFFER2;

	cli();

	writeSuccesiveStart(0); //0=Module
	if (_MODULE == HT1632_MODULE_8X32) {
		for (byte x = 0; x < 4; x++) {
			for (byte y = 0; y < _HEIGHT; y++) {
				data = *(byte *) (buffer + ((y << 2) + x));
				writeSuccesive(data >> 4);
				writeSuccesive(data & 0xF);
			}
		}
	} else {
		for (byte x = 0; x < 3; x++) {
			for (byte y = 0; y < _HEIGHT; y++) {
				data = *(byte *) (buffer + ((y << 2) + y + x));
				writeSuccesive(data >> 4);
				writeSuccesive(data & 0xF);
			}
		}
	}
	writeSuccesiveStop();
	sei();
}

void HT1632::setByte(int address, byte d) {
	byte * buffer;
	if (_BUFFER_ACTIVE == 0)
		buffer = _SCREEN_BUFFER1;
	else
		buffer = _SCREEN_BUFFER2;

	*(byte*) (buffer + address) = d;
}

void HT1632::drawPixel(int x, int y, byte color) {
	unsigned int address;
	byte * buffer;

	if (x < 0 || x >= _WIDTH || y < 0 || y >= _HEIGHT)
		return;

	if (_BUFFER_ACTIVE == 0)
		buffer = _SCREEN_BUFFER1;
	else
		buffer = _SCREEN_BUFFER2;

	if (_MODULE == HT1632_MODULE_8X32)
		address = (x >> 3) + (y << 2); 		//32x8  -> y * (_WIDTH/8) = 4
	else
		address = (x >> 3) + ((y << 1) + y); 	//24x16 -> y * (_WIDTH/8) = 3

	if (color == 1)
		*(byte *) (buffer + address) |= 1 << (7 - (x % 8));
	else
		*(byte *) (buffer + address) &= ~(1 << (7 - (x % 8)));

}

byte HT1632::getPixel(int x, int y) {
	unsigned int address;
	byte * buffer;

	if (x < 0 || x >= _WIDTH || y < 0 || y >= _HEIGHT)
		return (0);

	if (_BUFFER_ACTIVE == 0)
		buffer = _SCREEN_BUFFER1;
	else
		buffer = _SCREEN_BUFFER2;

	if (_MODULE == HT1632_MODULE_8X32)
		address = (x >> 3) + (y << 2); 		//32x8  -> y * (_WIDTH/8) = 4
	else
		address = (x >> 3) + ((y << 1) + y); 	//24x16 -> y * (_WIDTH/8) = 3

	if (((*(byte *) (buffer + address)) & (1 << (7 - (x % 8)))) != 0)
		return (1);
	else
		return (0);

}

void HT1632::setPixel(int x, int y) {
	drawPixel(x, y, 1);
}

void HT1632::clearPixel(int x, int y) {
	drawPixel(x, y, 0);
}

void HT1632::drawLine(int x1, int y1, int x2, int y2, byte color) {
	int F, x, y;

	if (x1 > x2)  // Swap points if p1 is on the right of p2
			{
		swap(x1, x2);
		swap(y1, y2);
	}

	// Handle trivial cases separately for algorithm speed up.
	// Trivial case 1: m = +/-INF (Vertical line)
	if (x1 == x2) {
		if (y1 > y2)  // Swap y-coordinates if p1 is above p2
				{
			swap(y1, y2);
		}

		x = x1;
		y = y1;
		while (y <= y2) {
			drawPixel(x, y, color);
			y++;
		}
		return;
	}
	// Trivial case 2: m = 0 (Horizontal line)
	else if (y1 == y2) {
		x = x1;
		y = y1;

		while (x <= x2) {
			drawPixel(x, y, color);
			x++;
		}
		return;
	}

	int dy = y2 - y1;  // y-increment from p1 to p2
	int dx = x2 - x1;  // x-increment from p1 to p2
	int dy2 = (dy << 1);  // dy << 1 == 2*dy
	int dx2 = (dx << 1);
	int dy2_minus_dx2 = dy2 - dx2;  // precompute constant for speed up
	int dy2_plus_dx2 = dy2 + dx2;

	if (dy >= 0)    // m >= 0
			{
		// Case 1: 0 <= m <= 1 (Original case)
		if (dy <= dx) {
			F = dy2 - dx;    // initial F

			x = x1;
			y = y1;
			while (x <= x2) {
				drawPixel(x, y, color);
				if (F <= 0) {
					F += dy2;
				} else {
					y++;
					F += dy2_minus_dx2;
				}
				x++;
			}
		}
		// Case 2: 1 < m < INF (Mirror about y=x line
		// replace all dy by dx and dx by dy)
		else {
			F = dx2 - dy;    // initial F

			y = y1;
			x = x1;
			while (y <= y2) {
				drawPixel(x, y, color);
				if (F <= 0) {
					F += dx2;
				} else {
					x++;
					F -= dy2_minus_dx2;
				}
				y++;
			}
		}
	} else    // m < 0
	{
		// Case 3: -1 <= m < 0 (Mirror about x-axis, replace all dy by -dy)
		if (dx >= -dy) {
			F = -dy2 - dx;    // initial F

			x = x1;
			y = y1;
			while (x <= x2) {
				drawPixel(x, y, color);
				if (F <= 0) {
					F -= dy2;
				} else {
					y--;
					F -= dy2_plus_dx2;
				}
				x++;
			}
		}
		// Case 4: -INF < m < -1 (Mirror about x-axis and mirror
		// about y=x line, replace all dx by -dy and dy by dx)
		else {
			F = dx2 + dy;    // initial F

			y = y1;
			x = x1;
			while (y >= y2) {
				drawPixel(x, y, color);
				if (F <= 0) {
					F += dx2;
				} else {
					x++;
					F += dy2_plus_dx2;
				}
				y--;
			}
		}
	}
}

// draw a rectangle
void HT1632::drawRect(int x, int y, int w, int h, byte color) {
	drawLine(x, y, x + w - 1, y, color);
	drawLine(x, y + h - 1, x + w - 1, y + h - 1, color);

	drawLine(x, y, x, y + h - 1, color);
	drawLine(x + w - 1, y, x + w - 1, y + h - 1, color);
}

// fill a rectangle
// TODO Use lines (maybe fastest instead pixel writes)
void HT1632::fillRect(int x, int y, int w, int h, byte color) {
	for (int i = x; i < x + w; i++) {
		for (int j = y; j < y + h; j++) {
			drawPixel(i, j, color);
		}
	}
}

// draw a circle outline
void HT1632::drawCircle(int x0, int y0, byte r, byte color) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	drawPixel(x0, y0 + r, color);
	drawPixel(x0, y0 - r, color);
	drawPixel(x0 + r, y0, color);
	drawPixel(x0 - r, y0, color);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		drawPixel(x0 + x, y0 + y, color);
		drawPixel(x0 - x, y0 + y, color);
		drawPixel(x0 + x, y0 - y, color);
		drawPixel(x0 - x, y0 - y, color);

		drawPixel(x0 + y, y0 + x, color);
		drawPixel(x0 - y, y0 + x, color);
		drawPixel(x0 + y, y0 - x, color);
		drawPixel(x0 - y, y0 - x, color);

	}
}

// fill a circle
void HT1632::fillCircle(int x0, int y0, byte r, byte color) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	drawLine(x0, y0 - r, x0, y0 + r + 1, color);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		drawLine(x0 + x, y0 - y, x0 + x, y0 + y + 1, color);
		drawLine(x0 - x, y0 - y, x0 - x, y0 + y + 1, color);
		drawLine(x0 + y, y0 - x, x0 + y, y0 + x + 1, color);
		drawLine(x0 - y, y0 - x, x0 - y, y0 + x + 1, color);
	}
}

void HT1632::drawChar(int x, int y, char c, byte color) {
	byte bit;
	int x1, y1;

	for (x1 = 0; x1 < FONT_WIDTH; x1++) {
		//detect if we're drawing row outside screen and jump next cycle
		if (x1 + x > _WIDTH || x1 + x < 0)
			continue;
		bit = pgm_read_byte(&font[c-32][x1]);
		for (y1 = 0; y1 < FONT_HEIGHT; y1++) {
			//detect if we're drawing column outside screen and jump next cycle
			if (y1 + y > _HEIGHT || y1 + y < 0)
				continue;
			if ((bit & (1 << (FONT_HEIGHT - y1))) != 0)
				drawPixel(x + x1, y + y1 - 1, color);
		}
	}
}
void HT1632::drawString(int x, int y, const char* str, byte color) {
	int i = 0;
	int x1 = 0;
	//char newline=10;
	while (str[i] != 0) {
		drawChar(x + (x1 * FONT_WIDTH), y, str[i], color);
		x1++;
		i++;
		if (str[i] == 10) { //check against "\n"
			y += FONT_HEIGHT;
			x1 = 0;
			i++;
		}
	}
}

void HT1632::setActiveBuffer(byte b) {
	if (b == 0)
		_BUFFER_ACTIVE = 0;
	else
		_BUFFER_ACTIVE = 1;
}

void HT1632::swapBuffers() {
	byte * temp = _SCREEN_BUFFER1;
	_SCREEN_BUFFER1 = _SCREEN_BUFFER2;
	_SCREEN_BUFFER2 = temp;
}

byte HT1632::getActiveBuffer() {
	return (_BUFFER_ACTIVE);
}

void HT1632::animateDown() {
	byte * buffer1;
	byte * buffer2;
	byte pixel;

	if (_BUFFER_ACTIVE == 0) {
		buffer1 = _SCREEN_BUFFER1;
		buffer2 = _SCREEN_BUFFER2;
	} else {
		buffer1 = _SCREEN_BUFFER2;
		buffer2 = _SCREEN_BUFFER1;
	}

	for (int y = _HEIGHT - 1; y >= 0; y--) {
		for (byte x = 0; x < _WIDTH; x++) {
			pixel = getPixel(x, y - 1);
			if (pixel != 0)
				drawPixel(x, y, 1);
			else
				drawPixel(x, y, 0);
		}

	}

}

