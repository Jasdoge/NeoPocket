/*
	
	Todo:
	- HW update is needed to get the charge value
	

	- Programmer settings:
		- 20mhz clock
		- 2.6V BOD
		- BOD enabled hold wakeup/disabled
		- Rest default

*/


#include <tinyNeoPixel_Static.h>
#include <avr/sleep.h>
#include "Consts.h"
#include "_Configuration.h"
#include "Animator.h"

#define ON_DUR (Configuration::KEEPALIVE_DURATION+Configuration::FADE_TIME)

#define overrideError
#include <KXTJ3-Min.h>

bool wokeByTimer = false;		// Tracks if it woke by timer. Used for reading battery charge status

// Reset button
void reset(){
	_PROTECTED_WRITE(RSTCTRL.SWRR,1);
}
void onError( uint8_t err ){

	for( uint8_t i = 0; i<err; ++i ){
		
		Animator::setPixels(10);
		delay(400);
		Animator::setPixels(0);
		delay(100);

	}
	
	delay(1000);
}
ISR(RTC_PIT_vect){
  	RTC.PITINTFLAGS = RTC_PI_bm;          /* Clear interrupt flag by writing '1' (required) */
	wokeByTimer = true;
}
void intHandler(){
	wokeByTimer = false;
}


// Variables
uint32_t wake = 0;				// Keepalive timer
uint32_t last_frame = 0;		// Last frame time (used for the animation ticker)
bool charging = false;
bool charging_done = false;
uint32_t last_charge_check = 0;	// Limits how often to read the battery
bool sleepOnLoop = false;		// Used to prevent recursion

// Chassis button
uint32_t button_change_time = 0;	// When was the button first held	
bool button_pressed = false;			// false = LOW
bool button_long_press = false;	// Tracks whether a long press has been registered

// Turns the lights on or off (including the bigpp fet)
void toggle( bool on = false, bool dly = false ){

	const uint32_t ms = millis();
	if( on ){

		wake = ms;

	}
	else{

		wake = 0;
		Animator::setPixels();

	}

	//toggleInputPins(on);

	// BIGPP is inverse, turning on at LOW
	digitalWrite(PIN_BIGPP, on ? HIGH : LOW);
	if( !on )
		digitalWrite(PIN_NEO_DTA, LOW);

	if( !wake ){
		if( dly )
			delay(1000);
		sleep();
	}

}


void sleep(){
	
	attachInterrupt(digitalPinToInterrupt(PIN_INTERRUPT), intHandler, RISING);
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
	detachInterrupt(digitalPinToInterrupt(PIN_INTERRUPT));

	toggle(true);	// Woke, turn on

}

void checkCharging( bool force ){

	#ifdef DEBUG_IGNORE_BATTERY

	charging = false;

	#else

	const uint32_t ms = millis();
	if( !force && ms-last_charge_check < 1000 )
		return;
	
	last_charge_check = ms;

	 
	charging_done = digitalRead(PIN_CHRG_STAT);		// goes LOW when charging
	
	// If no charger is plugged in, both these will be HIGH, otherwise one at a time will be low
	charging = !charging_done || !digitalRead(PIN_CHRG_DET);			// goes LOW when battery is fully charged
	

	#endif

}




void setup(){

	pinMode(PIN_RESET, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(PIN_RESET), reset, CHANGE);

	Accelerometer::onError = &onError;

	Serial.begin(9600);

	// The permanent output pins can be set here
	pinMode(PIN_BIGPP, OUTPUT);
	pinMode(PIN_NEO_DTA, OUTPUT);
	pinMode(PIN_RAND, OUTPUT);
	pinMode(PIN_CHRG_DET, INPUT);
	pinMode(PIN_CHRG_STAT, INPUT);
	pinMode(PIN_BAT_READ_IN, INPUT);	// Analog input, checks wheter it's charging or not
	pinMode(PIN_BOOT_LED, OUTPUT);
	pinMode(PIN_EXT_BUTTON, INPUT_PULLUP);
	pinMode(SDA, INPUT_PULLUP);
	pinMode(SCL, INPUT_PULLUP);
	digitalWrite(PIN_BIGPP, HIGH);		// Enables bigpp
	digitalWrite(PIN_RAND, LOW);
	

	// Interrupt ALWAYS needs to be an input
	pinMode(PIN_INTERRUPT, INPUT);

	toggle(true);

	Serial.println("IT BEGINS!");

	bool on = false;
	/* Initialize RTC: */
	while (RTC.STATUS > 0){
		                                   /* Wait for all register to be synchronized */
		delay(100);
		Animator::setPixels(0,0,(on ? 20 : 5));
		on = !on;
	}

	RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;    /* 32.768kHz Internal Ultra-Low-Power Oscillator (OSCULP32K) */
	RTC.PITINTCTRL = RTC_PI_bm;           /* PIT Interrupt: enabled */
	RTC.PITCTRLA = RTC_PERIOD_CYC32768_gc /* RTC Clock Cycles 32k, 1Hz ( */
	| RTC_PITEN_bm;                       /* Enable PIT counter: enabled */

	analogReference(INTERNAL2V5);

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	
	delay(10);

	Serial.println("Searching accelerometer");

  	if( !Accelerometer::begin() ){

		Serial.println("No accelerometer");
		for( uint8_t i = 0; i<4; ++i ){
			Animator::setPixels(i%2 ? 5 : 20);
			delay(500);
		}
		digitalWrite(PIN_BOOT_LED, HIGH);
		while(true){}

	}

	Serial.println("Accelerometer found!");

	Animator::setPixels(10,0,10);
	delay(50);

	Serial.println("Setting interrupt");
	// sensitivity, duration to trigger, duration to reset
	if( !Accelerometer::setInterrupt( 40, 10, 255 ) ){

		for( uint8_t i = 0; i<20; ++i ){
			Animator::setPixels(i%2 ? 5 : 20);
			delay(100);
		}
		while(true){}

	}

	Serial.println("Starting up");

	Animator::setPixels(5);
	delay(200);
	Animator::setPixels(0,5);
	delay(200);
	Animator::setPixels(0,0,5);
	delay(200);
	Animator::setPixels();
	
	
	digitalWrite(PIN_BOOT_LED, HIGH);
	delay(1000);
	digitalWrite(PIN_BOOT_LED, LOW);

	Configuration::onSetup();

	toggle(true);


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
		
		Configuration::onChargeFrame( charging_done );
		return;

	}


	// Not charging, do normal stuff
	const uint32_t ms = millis();


	// First off handle the button
	const bool buttonPressed = !digitalRead(PIN_EXT_BUTTON);
	// debounce
	if( ms-button_change_time > 100 && button_pressed != buttonPressed ){

		button_change_time = ms;
		button_pressed = buttonPressed;

		// Track short press on release
		if( !button_long_press && !button_pressed )
			Configuration::onButtonPress();

		// Clear long press
		if( !button_pressed )
			button_long_press = false;

	}
	
	if( button_pressed && ms-button_change_time > 1000 && !button_long_press ){

		Configuration::onButtonLongPress();
		button_long_press = true;

	}


	// Animate
	if( ms-last_frame > 16 && wake ){
		last_frame = ms;
		
		const float fadePerc = ms-wake < Configuration::KEEPALIVE_DURATION ? 
			1.0 : 
			max(0.0, 1.0-(float)(ms-wake-Configuration::KEEPALIVE_DURATION)/Configuration::FADE_TIME )
		;
		Animator::setFadePerc(fadePerc);
		Configuration::onFrame();

	}


	// Refresh timer if interrupt pin is high
	if( ms-wake < ON_DUR ){

		if( digitalRead(PIN_INTERRUPT) ){
			wake = ms;
		}
	}
	// Timer has expired, shut down
	else
		toggle();





}
