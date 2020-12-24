#include <avr/sleep.h>


ISR(PORTA_PORT_vect){
	uint8_t flags=PORTA.INTFLAGS;
  	PORTA.INTFLAGS=flags; //clear flags
}

ISR(RTC_PIT_vect){
  	RTC.PITINTFLAGS = RTC_PI_bm;          /* Clear interrupt flag by writing '1' (required) */
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


	for( uint8_t i =0; i<11; ++i ){
		pinMode(i, OUTPUT);
		digitalWrite(i, LOW);
	}

	pinMode(3, INPUT);

	delay(2000);

	PORTA.PIN7CTRL = 0b00000010;	// Trigger high


	sleep_cpu();

}

void loop(){

	delay(3000);
	sleep_cpu();
	sleep_cpu();
	sleep_cpu();

}

