#define overrideError
#include <Shibetek-KXTJ3.h>

#define PIN_LED 4
bool on;


void setup(){
	pinMode(PIN_LED, OUTPUT);
	Serial.begin(9600);

	delay(100);
	Serial.println("IT BEGINS");

	if( !Accelerometer::begin() ){
		Serial.print("Fail");
		return;
	}

	Serial.print("Succ");

}

void loop(){

	on = !on;

	digitalWrite(PIN_LED, on ? HIGH : LOW);
	delay(500);


}

