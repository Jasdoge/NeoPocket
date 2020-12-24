/*

	Test steps:
	- Attach neos
	- Add neos to this code
	- attach battery charger

*/

#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
#include <tinyNeoPixel_Static.h>
#include <avr/sleep.h>

const uint8_t NUM_PIXELS = 20;
const uint8_t PIN_NEO_DTA = 10;
const uint8_t PIN_BIGPP = 0;
const uint8_t PIN_INTERRUPT = 3;

uint8_t pixelBuffer[NUM_PIXELS*3] = {0};

tinyNeoPixel pixels = tinyNeoPixel(NUM_PIXELS, PIN_NEO_DTA, NEO_GRB, pixelBuffer);

// Objects
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

//#define debugPin(state) digitalWrite(10, state)

// Woke due to pin interrupt

ISR(PORTA_PORT_vect){
	uint8_t flags=PORTA.INTFLAGS;
  	PORTA.INTFLAGS=flags; //clear flags
}


ISR(RTC_PIT_vect){
  	RTC.PITINTFLAGS = RTC_PI_bm;          /* Clear interrupt flag by writing '1' (required) */
}

// Functions
void setLED( uint8_t index = 0, uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t side = 3 ){

	uint8_t indexB = (NUM_PIXELS-index-1)*3;
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

void setPixels( uint8_t r = 0, uint8_t g = 0, uint8_t b = 0 ){

	for( uint8_t i=0; i<NUM_PIXELS/2; ++i )
		setLED(i, r, g, b);
	pixels.show();

}

void onInterrupt(){

}

void sleep(){
	
	/*
	lis.setClick(2, 20, 5, 10, 255);

	digitalWrite(7, HIGH);		// Needed because the FET is pulled up
	digitalWrite(6, HIGH);
	*/

	PORTA.PIN7CTRL = 0b00000010;	// Trigger high

	sleep_cpu();
	sleep_cpu();
	sleep_cpu();
	sleep_cpu();
	
	PORTA.PIN7CTRL = 0b00000000;

}

void setup(){

	
	while (RTC.STATUS > 0){
		;                                 
	}
	RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;    
	RTC.PITINTCTRL = RTC_PI_bm;           
	RTC.PITCTRLA = RTC_PERIOD_CYC32768_gc 
	| RTC_PITEN_bm;

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	pinMode(PIN_NEO_DTA, OUTPUT);
	pinMode(PIN_BIGPP, OUTPUT);
	digitalWrite(PIN_BIGPP, LOW);

	delay(10);

	setPixels(5);

	bool on;
  	while( !lis.begin(on ? 0x19 : LIS3DH_DEFAULT_ADDRESS) ) {   // change this to 0x19 for alternative i2c address
		on = !on;
		delay(1000);
  	}


	lis.setDataRate(LIS3DH_DATARATE_LOWPOWER_1K6HZ);
	lis.setClick(2, 20, 5, 10, 255);

	setPixels(0, 5);
	delay(3000);
	for( uint8_t i =0; i<11; ++i ){
		pinMode(i, OUTPUT);
		digitalWrite(i, LOW);
	}

	
	
	//digitalWrite(8, HIGH);	// Chrg STAT is pulled up

	// SDA/SCL
	digitalWrite(7, HIGH);		// Needed because the FET is pulled up
	digitalWrite(6, HIGH);

	// Neo EN pulled up
	digitalWrite(PIN_BIGPP, HIGH);

	pinMode(PIN_INTERRUPT, INPUT);

	
	sleep();

}


void loop(){

	/*
	digitalWrite(PIN_BIGPP, LOW);
	delay(10);
	setPixels(5,5,5);
	delay(3000);
	digitalWrite(PIN_BIGPP, HIGH);
	*/
	PORTA.PIN7CTRL = 0b00000010;	// Trigger high
	lis.setDataRate(LIS3DH_DATARATE_LOWPOWER_1K6HZ);
	digitalWrite(7, HIGH);		// Needed because the FET is pulled up
	digitalWrite(6, HIGH);

	sleep();

}

