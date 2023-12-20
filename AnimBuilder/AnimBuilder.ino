#include <tinyNeoPixel_Static.h>
#include "Animator.h"
#include "_Configuration.h"


void setup(){

	Serial.begin(9600);
	pinMode(PIN_BIGPP, OUTPUT);
	digitalWrite(PIN_BIGPP, HIGH);	// It's actually flipped by an N-FET
	pinMode(PIN_NEO_DTA, OUTPUT);

	Animator::onToggle(true);
	delay(10);
	Animator::setPixels(10,0,0);
	delay(100);
	
	Serial.println("IT BEGINS");

}

uint8_t step = 0;

void loop(){

	if( ++step > 3 )
		step = 0;

	Animator::setPixels(
		step == 0 ? 10 : 0,
		step == 1 ? 10 : 0,
		step == 2 ? 10 : 0
	);
	//npAnim::animWaveSparkle();
	delay(1000);

}
