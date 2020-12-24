#ifndef __NP_Animations
#define __NP_Animations

#include "_Config.h"
#include <tinyNeoPixel_Static.h>


namespace npAnim{

	
	// Brightness mode for conf
	uint8_t mode = 1;							// Sets the brightness level. 1 is default. Use a const below
		const uint8_t BRIGHTNESS_HIGH = 2;
		const uint8_t BRIGHTNESS_MED = 1;
		const uint8_t BRIGHTNESS_LOW = 0;

	// Shared vars
	uint8_t pixelBuffer[NUM_PIXELS*3] = {0};
	float fadePerc;
	// Timers that can be used by the animation
	uint32_t timer_a;
	uint16_t timer_b;
	uint16_t timer_c;
	tinyNeoPixel pixels = tinyNeoPixel(NUM_PIXELS, PIN_NEO_DTA, NEO_GRB, pixelBuffer);

	
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

	void showMode(){
		for( uint8_t i =0; i<NUM_PIXELS/2; ++i ){
			uint8_t intens = ((i < (mode+1)*NUM_PIXELS/2/3) || mode == 2) * 20;
			setLED(i, intens, intens, intens);
		} 
		pixels.show();
		delay(500);
	}

	void increaseMode(){
		delay(10);
		++mode;
		mode = mode%3;
		showMode();
	}

	

	void setFadePerc( float perc ){
		fadePerc = perc;
	}

	int quickRand( int min, int max ){
		
		pinMode(PIN_RAND, INPUT);
		int out = (analogRead(PIN_RAND)+millis())%(max-min);
		pinMode(PIN_RAND, OUTPUT);
		return out+min;

	}

	uint8_t getMaxBright(){
		if( mode == BRIGHTNESS_LOW )
			return 50;
		else if( mode == BRIGHTNESS_MED )
			return 100;
		return 255;
	};

	void animCharging( bool full ){

		const uint32_t ms = millis();
		const float perc = (ms%2000)/2000.0;		// Percent through animation
		const uint8_t numSets = NUM_PIXELS/2;			// 
		
		for( uint8_t i = 0; i < numSets; ++i ){

			const float myPerc = (float)i/numSets;
			const float dist = fabs(myPerc-perc);
			const uint8_t col = dist <= 0.1 ? 100*(0.1-dist) : 0;
			setLED(
				i, 
				(!full ? col : 0),
				(full ? col : 0),
				0
			);

		}
		pixels.show();

	};


	void animKryptonite(){

		const uint8_t ANIM_COLOR_RED = 0x4;
		const uint8_t ANIM_COLOR_GREEN = 0x2;
		const uint8_t ANIM_COLOR_BLUE = 0x1;

		const uint32_t ms = millis();				// Current time
		const float perc = (ms%2000)/2000.0;		// Percent through animation
		const uint8_t numSets = NUM_PIXELS/2;			// 
		const float pixelDist = 1.0/numSets;

		
		uint8_t maxBright = getMaxBright();

		// Trigger a flash
		if( ms-timer_a > timer_b+timer_c ){

			timer_a = ms;	// Started
			timer_b = quickRand(100,500);	// Duration
			timer_c = quickRand(1,1500);	// Cooldown

		}
		// Percentage of the flash
		const float flashPerc = (float)(ms-timer_a)/timer_b;

		
		// run on each pixel
		for( uint8_t i=0; i<numSets; ++i ){
			
			const float myOffset = (float)i/numSets;		// Value between 0 and 1
			uint8_t r = 0, g = 0, b = 0;
			
			// Flash in progress
			float flashAdd = 0; // 0->1.0
			if( ms-timer_a < timer_b ){
				
				const float dist = 1.0-(float)(flashPerc-myOffset)/pixelDist/8;
				if( dist > 0 && dist <= 1.0 )
					flashAdd = dist;

			}

			
			float out = (
				0.05 +
				(sin(perc*PI*2)/2 + 0.5) * 0.2+	// 0->0.2
				flashAdd * 0.75					// 0->0.75
			)*fadePerc*maxBright;

			if( ANIM_COLOR&ANIM_COLOR_GREEN )
				g = out;
			if( ANIM_COLOR&ANIM_COLOR_BLUE )
				b = out;
			if( ANIM_COLOR&ANIM_COLOR_RED )
				r = out;


			setLED(i, r, g, b);

		}
		
		pixels.show();

	}



}



#endif

