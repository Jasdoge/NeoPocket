#include "SparkFunLIS3DH.h"
#include "Wire.h"
#include "SPI.h"
#include <tinyNeoPixel_Static.h>


const uint8_t PIN_BIGPP = 0;			// Turns on on LOW
const uint8_t PIN_NEO_DTA = 10;

#define NUM_PIXELS 20
uint8_t pixelBuffer[NUM_PIXELS*3] = {0};
tinyNeoPixel pixels(NUM_PIXELS, PIN_NEO_DTA, NEO_GRB, pixelBuffer);

void setLED( byte index = 0, byte r = 0, byte g = 0, byte b = 0, byte side = 3 ){

	byte indexB = (NUM_PIXELS-index-1)*3;
	index *= 3;

	if( side & 1 ){
		pixelBuffer[index] = g;
		pixelBuffer[index+1] = r;
		pixelBuffer[index+2] = b;
	}
	if( side & 2 ){
		pixelBuffer[indexB] = g;
		pixelBuffer[indexB+1] = r;
		pixelBuffer[indexB+2] = b;
	}

}

void setPixels( byte r = 0, byte g = 0, byte b = 0 ){

	for( byte i=0; i<NUM_PIXELS/2; ++i )
		setLED(i, r, g, b);
	pixels.show();

}

void setup(){

	pinMode(PIN_BIGPP, OUTPUT);
	digitalWrite(PIN_BIGPP, LOW);
	pinMode(PIN_NEO_DTA, OUTPUT);
	delay(10);
	setPixels(100,0,0);
	
}


void loop(){

	//setPixels(100,0,0);
	delay(1000);

}
