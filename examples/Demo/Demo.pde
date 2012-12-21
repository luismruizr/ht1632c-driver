#include "HT1632C.h"

#define PI_2 3.1415927*2

#define DATA_PIN 5
#define WR_PIN 4
#define CS_PIN 6

#define NUM 10 //Number of circles
const int width = 32;
const int height = 8;

//For pattern
int j = 0;

float x[NUM];
float y[NUM];
float xa[NUM];
float ya[NUM];

void initMatrix();
void circulo(int);
void flash();
void flash2();
void nieve();
void nieve2();
void patron(float, float);
void scanner();

HT1632 matrix = HT1632(DATA_PIN, WR_PIN, CS_PIN);

void setup() {
	matrix.init();
	matrix.clearScreen();
	matrix.blinkMode(false);
	matrix.setBrightness(15);
	Serial.begin(115200);
}

/*******************************************/

void loop() {
	long int i = 0;

	initMatrix();

	//Draw full charset and scroll
	for (i = 0; i < (92 * 5 - 32); i++) {
		for (int k = 0; k < 96; k++) {
			matrix.drawChar((k << 2) - i , 0, k + 32, 1);
		}
		matrix.writeScreen();
		matrix.clearScreen();
		delay(60);
	}

	//Same but faster.
	for (i = 0; i < (92 * 5 - 32); i++) {
		for (int k = 0; k < 96; k++) {
			matrix.drawChar((k << 2) - i , 0, k + 32, 1);
		}
		matrix.writeScreen();
		matrix.clearScreen();
		delay(5);
	}

	//Swap screen buffer test.
	matrix.setActiveBuffer(0);
	matrix.clearScreen();
	matrix.drawString(1, 1, "Test 123", 1);
	matrix.setActiveBuffer(1);
	matrix.fillScreen();
	matrix.drawString(1, 1, "456 Tested", 0);
	matrix.setActiveBuffer(0);

	int r = 10;
	for (i = 0; i < 300; i += r) {
		matrix.writeScreen();
		matrix.swapBuffers();
		delay(300 - i);
		if (i > 280)
			r = 1;
	}

	//Fast buffer swap test 296 fps
	nieve2();

	//Blink test
	flash();
	for (i = 0; i < 5; i++) {
		flash2();
	}

	matrix.setBrightness(15);

	for (i = 0; i < 6; i++)
		scanner();

	for (i = 0; i < 5; i++)
		for (float f = 0; f < PI_2; f += (PI_2 / 36))
			patron(cos(f) * 5, sin(f) * 5);

	j = 0;

	for (i = 0; i < 200; i++)
		circulo(0);

	for (i = 0; i < 200; i++)
		circulo(1);

	for (i = 0; i < 200; i++)
		nieve();
}

/*******************************************/

void initMatrix() {
	int i;
	randomSeed(123 + analogRead(0) + analogRead(1) + analogRead(2));
	for (i = 0; i < NUM; i++) {
		x[i] = random(1, 31);
		y[i] = random(1, 7);

		switch (random(0, 5)) {
		case 0:
			xa[i] = 1;
			break;
		case 1:
			xa[i] = 0.8;
			break;
		case 2:
			xa[i] = 0.7;
			break;
		case 3:
			xa[i] = 0.6;
			break;
		case 4:
			xa[i] = 0.5;
			break;
		case 5:
			xa[i] = 0.4;
			break;
		}
		switch (random(0, 5)) {
		case 0:
			ya[i] = 1;
			break;
		case 1:
			ya[i] = 0.8;
			break;
		case 2:
			ya[i] = 0.7;
			break;
		case 3:
			ya[i] = 0.6;
			break;
		case 4:
			ya[i] = 0.5;
			break;
		case 5:
			ya[i] = 0.4;
			break;
		}

		if (random(0, 1) == 1)
			xa[i] = -xa[i];
		if (random(0, 1) == 1)
			ya[i] = -ya[i];
	}
}
/*******************************************/
void circulo(int b) {
	int i;
	for (i = 0; i < NUM; i++) {
		if (b)
			matrix.fillCircle(x[i], y[i], 2, 1);
		else
			matrix.drawCircle(x[i], y[i], 5, 1);

		if (x[i] >= width - 1 || x[i] <= 0)
			xa[i] = -xa[i];
		if (y[i] >= height - 1 || y[i] <= 0)
			ya[i] = -ya[i];

		x[i] += xa[i];
		y[i] += ya[i];
	}
	matrix.writeScreen();
	delay(5);
	matrix.clearScreen();
}
/*******************************************/
void nieve() {
	uint16_t i;
	for (i = 0; i < 8 * 4; i++)
		matrix.setByte(i, random(0, 255));
	matrix.writeScreen();
	//delay(40);
	matrix.clearScreen();
}
/*******************************************/
void nieve2() {
	matrix.setBrightness(1);
	uint16_t i;
	matrix.setActiveBuffer(0);
	matrix.clearScreen();
	matrix.setActiveBuffer(1);
	matrix.fillScreen();
	matrix.drawString(3, 1, "Swap!!!!", 0);
	//matrix.drawString(18, 1, "awS", 0);

	for (i = 0; i < 1000; i++) {
		matrix.swapBuffers();
		matrix.writeScreen();
		//delay(1));
	}

	matrix.clearScreen();
	matrix.setBrightness(15);
}
/*******************************************/
void flash() {
	matrix.fillScreen();
	matrix.writeScreen();
	delay(300);
	matrix.blinkMode(1);
	delay(1000);
	matrix.blinkMode(0);
	matrix.clearScreen();
	matrix.writeScreen();
}
/*******************************************/
void flash2() {
	matrix.fillScreen();
	matrix.writeScreen();
	for (j = 0; j < 16; j++) {
		matrix.setBrightness(j);
		delay(20);
	}
	for (j = 15; j >= 0; j--) {
		matrix.setBrightness(j);
		delay(20);
	}
}
/*******************************************/
void patron(float b, float c) {
	int x, y;
	for (x = 0; x < 36; x++)
		for (y = 0; y < 8; y++) {
			matrix.drawLine(0, y, 31, y, (y+b)&3);
			matrix.drawLine(x, 0, x, 7, (x+c)&3);
		}
	matrix.writeScreen();
	matrix.clearScreen();
}
/*******************************************/
void scanner() {
	int i;
	for (i = 0; i < 32; i++) {
		matrix.drawLine(i - 1, 1, i - 1, 6, 1);
		matrix.drawLine(i, 0, i, 7, 1);
		matrix.drawLine(i + 1, 1, i + 1, 6, 1);
		matrix.writeScreen();
		delay(3);
		matrix.clearScreen();
	}

	for (i = 31; i > 0; i--) {
		matrix.drawLine(i - 1, 1, i - 1, 6, 1);
		matrix.drawLine(i, 0, i, 7, 1);
		matrix.drawLine(i + 1, 1, i + 1, 6, 1);
		matrix.writeScreen();
		delay(3);
		matrix.clearScreen();
	}
}

