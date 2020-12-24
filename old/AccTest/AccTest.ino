#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
#include <tinyNeoPixel_Static.h>
#include <avr/sleep.h>

#include "Animations.h"

// Constants
const uint16_t BATTERY_THR = 740;			// when below this value, battery is considered low
const uint8_t CLICKTHRESHHOLD_ON = 80;		// can't do less
const uint8_t CLICKTHRESHHOLD_OFF = 20;
const uint16_t CLICK_BUFFER = 1000;		// Force wait this duration between buffers
const uint32_t KEEPALIVE_DURATION = 150000; //150000;	// Use ON_DUR
const uint32_t FADE_TIME = 30000;			// This is appended to KEEPALIVE_DURATION
const float KEEPALIVE_THRESH = 3.0;			// Threshold to keeping the light alive. Lower is more sensitive.
#define ON_DUR (KEEPALIVE_DURATION+FADE_TIME)


// Variables
uint32_t first_wake = 0;		// Time of last turn on/off
uint32_t wake = 0;				// Keepalive timer
float cacheMag;					// Velocity cache, used to compare velocity reads
uint32_t last_read = 0;			// Used to stagger accelerometer reads for keepalive
uint32_t last_frame = 0;		// Last frame time (used for the animation ticker)
bool charging = false;
bool charging_done = false;
bool wokeByTimer = false;
uint32_t last_charge_check = 0;
bool sleepOnLoop = false;		// Used to prevent recursion

const uint16_t BRIGHTNESS_BUFFER = 3000;	// Time between initial knocks to change brightness mode


// Objects
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

// Woke due to pin interrupt
ISR(PORTA_PORT_vect){
	uint8_t flags=PORTA.INTFLAGS;
  	PORTA.INTFLAGS=flags; //clear flags
}

ISR(RTC_PIT_vect){
  	RTC.PITINTFLAGS = RTC_PI_bm;          /* Clear interrupt flag by writing '1' (required) */
	wokeByTimer = true;
}

// Support functions
float vecMag( float x, float y, float z ){
	return sqrt(x*x+y*y+z*z);
}


// Turns the lights on or off
void toggle( bool on = false, bool dly = false ){

	const uint32_t ms = millis();
	if( on ){
		wake = ms;
	}
	else{
		wake = 0;
		npAnim::setPixels();
	}

	// BIGPP is inverse, turning on at LOW
	digitalWrite(PIN_BIGPP, on ? LOW : HIGH);
	if( !on )
		digitalWrite(PIN_NEO_DTA, LOW);
	first_wake = ms;		// Track time of start/stop to limit it to 1s
	cacheMag = 0;			// Reset the mag reading

	if( !wake ){
		if( dly )
			delay(1000);
		sleep();
	}

}



void sleep(){
	
	lis.setDataRate(LIS3DH_DATARATE_100_HZ);
	lis.setClick(2, CLICKTHRESHHOLD_OFF, 5, 10, 255);
	PORTA.PIN7CTRL = 0b00000001;	// Trigger high
	sleep_cpu();

	// Just on to check if we're charging
	if( wokeByTimer ){

		wokeByTimer = false;
		checkCharging(true);
		
		// not plugged in, go back to sleep immediately
		if( !charging ){
			sleepOnLoop = true;
			return;
		}

	}

	// We're either charging or woke by a shake. In either case handle the sensor, otherwise it gets borked when you unplug the charger

	// woek up
	PORTA.PIN7CTRL = 0b00000000;	// Disable pin interrupt
	lis.setDataRate(LIS3DH_DATARATE_LOWPOWER_5KHZ);
	lis.setClick(2, CLICKTHRESHHOLD_ON);


	toggle(true);	// Woke, turn on
	
	// Show mode if we didn't wake up from charging
	if( !charging ){

		// Check battery
		digitalWrite(PIN_BATTERY_READ_EN, HIGH);
		delay(10);
		if( analogRead(PIN_BATTERY_READ_IN) <= BATTERY_THR ){
			npAnim::setPixels(10,0,0);
			delay(1000);
		}

		npAnim::showMode();
	}
	first_wake = millis();

}

void checkCharging( bool force ){

	const uint32_t ms = millis();
	if( !force && ms-last_charge_check < 1000 )
		return;
	
	last_charge_check = millis();

	charging = analogRead(PIN_CHARGE_EN) > 100;
	charging_done = digitalRead(PIN_CHARGE_STAT);


}




void setup(){

	/* Initialize RTC: */
	while (RTC.STATUS > 0){
		;                                   /* Wait for all register to be synchronized */
	}
	RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;    /* 32.768kHz Internal Ultra-Low-Power Oscillator (OSCULP32K) */
	RTC.PITINTCTRL = RTC_PI_bm;           /* PIT Interrupt: enabled */
	RTC.PITCTRLA = RTC_PERIOD_CYC32768_gc /* RTC Clock Cycles 32k, 1Hz ( */
	| RTC_PITEN_bm;                       /* Enable PIT counter: enabled */

	analogReference(INTERNAL2V5);

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	pinMode(PIN_BIGPP, OUTPUT);
	pinMode(PIN_INTERRUPT, INPUT);
	pinMode(PIN_NEO_DTA, OUTPUT);
	pinMode(PIN_RAND, OUTPUT);
	pinMode(PIN_CHARGE_EN, INPUT);
	pinMode(PIN_CHARGE_STAT, INPUT_PULLUP);
	pinMode(PIN_UNUSED_A, OUTPUT);
	pinMode(PIN_BATTERY_READ_EN, OUTPUT);
	pinMode(PIN_BATTERY_READ_IN, INPUT);
	
	digitalWrite(PIN_BATTERY_READ_EN, LOW);
	digitalWrite(PIN_BIGPP, LOW);
	digitalWrite(PIN_CHARGE_EN, LOW);
	digitalWrite(PIN_CHARGE_STAT, LOW);
	digitalWrite(PIN_RAND, LOW);
	digitalWrite(PIN_UNUSED_A, LOW);


	npAnim::setPixels(5);

  	delay(100);
	bool on;
  	while( !lis.begin(on ? 0x19 : LIS3DH_DEFAULT_ADDRESS) ) {   // change this to 0x19 for alternative i2c address
		npAnim::setPixels(on ? 5 : 20);
		on = !on;
		delay(1000);
  	}

	lis.setRange(LIS3DH_RANGE_16_G);

	// 0 = turn off click detection & interrupt
	// 1 = single click only interrupt output
	// 2 = double click only interrupt output, detect single click
	// Adjust threshhold, higher numbers are less sensitive
	
	delay(100);

	npAnim::setPixels(5);
	delay(200);
	npAnim::setPixels(0,5);
	delay(200);
	npAnim::setPixels(0,0,5);
	delay(200);
	npAnim::setPixels();
	
	toggle();

}

void loop(){

	// This is a recursion breaker
	if( sleepOnLoop ){

		sleepOnLoop = false;
		toggle(false);
		return;

	}

	checkCharging(false);
	
	// While charging. Only animate charge, regular functionality is disabled
	if( charging ){
		
		npAnim::animCharging(charging_done);
		return;

	}


	// Not charging, do normal stuff
	const uint32_t ms = millis();

	// Animate
	if( ms-last_frame > 16 && wake ){
		last_frame = ms;
		
		const float fadePerc = millis()-wake < KEEPALIVE_DURATION ? 
			1.0 : 
			max(0.0, 1.0-(float)(millis()-wake-KEEPALIVE_DURATION)/FADE_TIME )
		;
		npAnim::setFadePerc(fadePerc);
		ANIM_FRAME();

	}

	
	// If a doubleclick was received, always toggle.
	if( lis.getClick()&0x20 ){

		// Brightness level switch
		if( wake && ms-first_wake < BRIGHTNESS_BUFFER && ms-first_wake > 300 ){
			
			first_wake = ms;
			npAnim::increaseMode();

		}
		else if( ms-first_wake > CLICK_BUFFER ){
			
			toggle(!wake, true);
			return;

		}
		

	}

	// Read every 250 ms while awake
	if( ms-last_read > 250 && wake ){
		last_read = ms;

		// We're still alive
		if( ms-wake < ON_DUR ){
			
			sensors_event_t event; 
			lis.getEvent(&event);
			
			float x = event.acceleration.x;
			float y = event.acceleration.y;
			float z = event.acceleration.z;
			float mag = vecMag(x, y, z);
			float dist = fabs(mag-cacheMag);
			
			cacheMag = mag;
			if( dist > KEEPALIVE_THRESH )
				wake = ms;
		
			
		}
		// Timer has expired, shut down
		else
			toggle();

	}




}
