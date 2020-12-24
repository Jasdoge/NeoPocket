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


const uint8_t PIN_INTERRUPT = 3;
// Objects
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

#define debugPin(state) digitalWrite(10, state)

void setup(){

	/* 
	while (RTC.STATUS > 0){
		;                                 
	}
	RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;    
	RTC.PITINTCTRL = RTC_PI_bm;           
	RTC.PITCTRLA = RTC_PERIOD_CYC32768_gc 
	| RTC_PITEN_bm;                       
	*/
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();

	pinMode(10, OUTPUT);
	digitalWrite(10, HIGH);
	//pinMode(8, OUTPUT);
	//digitalWrite(8, HIGH);	// This is also bigpp fet so it should start high
	delay(100);

	bool on;
  	while( !lis.begin(on ? 0x19 : LIS3DH_DEFAULT_ADDRESS) ) {   // change this to 0x19 for alternative i2c address
		on = !on;
		delay(1000);
  	}
	
	debugPin(LOW);

	lis.setDataRate(LIS3DH_DATARATE_LOWPOWER_1K6HZ);
	lis.setClick(2, 20, 5, 10, 255);

	delay(3000);
	debugPin(HIGH);
	delay(100);
	debugPin(LOW);
	
	for( uint8_t i =0; i<11; ++i ){
		pinMode(i, OUTPUT);
		digitalWrite(i, LOW);
	}

	
	
	//digitalWrite(8, HIGH);	// Chrg STAT is pulled up
	//digitalWrite(0, HIGH);	// BigPP is pulled up
	
	debugPin(LOW);

	// SDA/SCL
	digitalWrite(7, HIGH);		// Needed because the FET is pulled up
	digitalWrite(6, HIGH);

	// Neo EN pulled up
	digitalWrite(0, HIGH);

	//pinMode(A7, INPUT);

	sleep_cpu();

}


void loop(){

	digitalWrite(8, CHANGE);
	delay(500);

}

