#include <tinyNeoPixel_Static.h>


#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

const uint8_t CLICKTHRESHHOLD_ON = 50;

const uint8_t PIN_INTERRUPT = 3;
const uint8_t PIN_NEO_EN = 0;			// Turns on on LOW
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


void setup() {

	pinMode(PIN_NEO_DTA, OUTPUT);	// Important
	pinMode(PIN_NEO_EN, OUTPUT);
	digitalWrite(PIN_NEO_EN, LOW);
	delay(100);

	setPixels(10,5);
	delay(500);
	setPixels();


	pinMode(PIN_INTERRUPT, INPUT);

	setPixels(0,0,5);
	delay(50);
	setPixels(0);

	if( !lis.begin(0x19) ){
		setPixels(20,20,0);
		if( !lis.begin() ){
			setPixels(5);
			while(true){delay(1000);}
		}

	}

	
	setPixels(10,10,10);
	delay(1000);
	setPixels();

	lis.setRange(LIS3DH_RANGE_16_G);   // 2, 4, 8 or 16 G!
	lis.setClick(2, CLICKTHRESHHOLD_ON, 10, 200);


	

}


void loop(){

	//digitalRead(PIN_INTERRUPT)

	if( digitalRead(PIN_INTERRUPT) ){

		setPixels(0,50,0);
		delay(500);
		setPixels();
		
	}
	
  
}