
bool on = true;
const uint8_t pin = 3;
//const uint8_t interruptPin = 16;	// boot pin
const uint8_t interruptPin = 2;	// acceleometer pin
#define overrideError
#include <Shibetek-KXTJ3.h>

void onPushed(){
	on = true;
	detachInterrupt(digitalPinToInterrupt(interruptPin));
}

void setup(){

	pinMode(pin, OUTPUT);
	digitalWrite(pin, LOW);

	pinMode(interruptPin, INPUT);
	//pinMode(interruptPin, OUTPUT);
	//attachInterrupt(digitalPinToInterrupt(interruptPin), onPushed, CHANGE);
	
	delay(10);
  	if( !Accelerometer::begin() ){
		for( uint8_t i = 0; i<4; ++i ){
			delay(500);
		}
		while(true){}
	}

	// sensitivity, duration to trigger, duration to reset
	if( !Accelerometer::setInterrupt( 40, 10, 255 ) ){
		for( uint8_t i = 0; i<20; ++i ){
			delay(100);
		}
		while(true){}
	}

	digitalWrite(pin, HIGH);
	delay(1000);
	digitalWrite(pin, LOW);
	delay(1000);


}


void loop(){

	digitalWrite(pin, on ? HIGH : LOW);
	if( on ){
		delay(1000);
		on = false;
		attachInterrupt(digitalPinToInterrupt(interruptPin), onPushed, RISING);

	}
	else
		delay(10);
}

