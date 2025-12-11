#ifndef __NP_Animations
#define __NP_Animations

#include "Consts.h"
#include <tinyNeoPixel_Static.h>

namespace Animator{


	// Shared vars
	uint8_t pixelBuffer[NUM_PIXELS*3] = {0};
	float fadePerc = 1;		// Fades from 1 to 0 (on to off)
	// Timers that can be used by the animation
	uint32_t timer_a;
	uint16_t timer_b;
	uint16_t timer_c;

	// Used by sparkling animations
	uint32_t sparkle_started[NUM_PIXELS] = {0};
	uint8_t seed = 0;

	uint8_t maxBrightness = 0x7F;
	float maxRed = 1.0;
	float maxGreen = 1.0;
	float maxBlue = 1.0;
	uint16_t animDuration = 4000;
	bool backwards = false;

	int16_t torchReach = 0;
	int16_t torchDist = 0;	// between 0 and 628 (two pi)*100
	bool torchDir;

	// Sets the max values for the colors for the animation
	void setColor( float r = 0, float g = 0, float b = 0 ){
		
		maxRed = r;
		maxGreen = g;
		maxBlue = b;

	}

	void setMaxBrightness( uint8_t brightness ){
		maxBrightness = brightness;
	}

	void setDuration( uint16_t dur ){
		animDuration = dur;
	}

	void setBackwards( bool back ){
		backwards = back;
	}



	tinyNeoPixel pixels = tinyNeoPixel(NUM_PIXELS, PIN_NEO_DTA, NEO_GRB, pixelBuffer);

	// Gets animation percent
	float getPerc( const uint32_t ms ){
		
		const uint16_t anim_dur = animDuration;
		float out = (float)(ms%anim_dur)/anim_dur;
		if( backwards )
			return 1.0-out;

		return out;
	}

	void output(){
		pixels.show();
	}

	// Trigger before and after sleep
	void onToggle( bool on = false ){
		pinMode(PIN_RAND, (on ? INPUT : OUTPUT));
	};

	void hsvToRgb(float h, float s, float v, uint8_t &r, uint8_t &g, uint8_t &b){
		int i;
		float f,p,q,t;
		
		h = max(0.0, min(360.0, h));
		s = max(0.0, min(100.0, s));
		v = max(0.0, min(100.0, v));
		
		s /= 100;
		v /= 100;
		
		if(s == 0) {
			// Achromatic (grey)
			r = g = b = round(v*255);
			return;
		}

		h /= 60; // sector 0 to 5
		i = floor(h);
		f = h - i; // factorial part of h
		p = v * (1 - s);
		q = v * (1 - s * f);
		t = v * (1 - s * (1 - f));
		switch(i) {
			case 0:
				r = round(255*v);
				g = round(255*t);
				b = round(255*p);
				break;
			case 1:
				r = round(255*q);
				g = round(255*v);
				b = round(255*p);
				break;
			case 2:
				r = round(255*p);
				g = round(255*v);
				b = round(255*t);
				break;
			case 3:
				r = round(255*p);
				g = round(255*q);
				b = round(255*v);
				break;
			case 4:
				r = round(255*t);
				g = round(255*p);
				b = round(255*v);
				break;
			default: // case 5:
				r = round(255*v);
				g = round(255*p);
				b = round(255*q);
		}
	}
	
	// Functions
	void setLED( uint8_t index = 0, uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t side = 3 ){

		uint8_t indexB = (NUM_PIXELS-index-1)*3;
		index *= 3;

		r = map(r, 0,255, 0,maxBrightness);
		g = map(g, 0,255, 0,maxBrightness);
		b = map(b, 0,255, 0,maxBrightness);

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
		output();

	}

	

	void setFadePerc( float perc ){
		fadePerc = perc;
	}

	int quickRand( int min, int max ){
		
		int out = (analogRead(PIN_RAND)+millis()+seed)%(max-min+1);
		++seed;
		return out+min;

	}

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
		output();

	};



	// Animations you can use
	void animKryptonite(){


		const uint32_t ms = millis();				// Current time
		const float perc = getPerc(ms);		// Percent through animation
		const uint8_t numSets = NUM_PIXELS/2;			// 
		const float pixelDist = 1.0/numSets;
		
		

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
			)*fadePerc*255;

			g = out*maxGreen;
			b = out*maxBlue;
			r = out*maxRed;


			setLED(i, r, g, b);

		}
		
		output();

	}
	

	// waveWidth : How wide should the waves be? In full strip lengths
	void animWave( float waveWidth = 4.0 ){

		const uint32_t ms = millis();				// Current time
		const float perc = getPerc(ms);		// Percent through animation
		const uint8_t numSets = NUM_PIXELS/2;			// 
		//const float pixelDist = 1.0/numSets;
		
		// run on each pixel
		for( uint8_t i=0; i<numSets; ++i ){
			
			const float myOffset = (float)i/numSets;		// Value between 0 and 1
			uint8_t r = 0, g = 0, b = 0;
			
			
			float s = sin(
				perc*PI*2+
				myOffset*PI*2/waveWidth
			)/2+0.5;

			float out = (
				0.05 + s
			)*fadePerc*255;

			out = min(out, 255);
			g = out*maxGreen;
			b = out*maxBlue;
			r = out*maxRed;

			setLED(i, r, g, b);

		}
		
		output();
		
	}

	
	// saturation : How saturated should the rainbow be? Between 0 and 100. A lower value generates a more bleached rainbow.
	void animRainbowFlat( uint8_t saturation = 100 ){

		const uint32_t ms = millis();				// Current time
		const float perc = getPerc(ms);		// Percent through animation
		const uint8_t numSets = NUM_PIXELS/2;			// 
		//const float pixelDist = 1.0/numSets;

		uint8_t ri = 5, gi = 5, bi = 5;
		hsvToRgb(perc*360, saturation, 100, ri, gi, bi);
		ri *= fadePerc;
		gi *= fadePerc;
		bi *= fadePerc;
		// run on each pixel
		for( uint8_t i=0; i<numSets; ++i )			
			setLED(i, ri, gi, bi);
		
		output();
		
	}
	

	// saturation : How saturated should the rainbow be? Between 0 and 100. A lower value generates a more bleached rainbow.
	// waveWidth : How wide should the waves be? In full strip lengths
	void animRainbowWave( uint8_t saturation = 100, float waveWidth = 4.0 ){

		const uint32_t ms = millis();				// Current time
		const float perc = getPerc(ms);		// Percent through animation
		const uint8_t numSets = NUM_PIXELS/2;			// 
		//const float pixelDist = 1.0/numSets;

		// run on each pixel
		for( uint8_t i=0; i<numSets; ++i ){

			const float myOffset = (float)i/numSets;
			uint8_t ri = 5, gi = 5, bi = 5;
			float p = perc*360+myOffset*360/waveWidth;
			p = (uint16_t)p%360;
			hsvToRgb(p, saturation, 100, ri, gi, bi);

			setLED(i, ri, gi, bi);

		}
		
		output();
		
	}

	// waveWidth : How wide should the waves be? In full strip lengths
	// sparkleChance : A lower value results in more sparkles
	// sparkleDuration : How long should each sparkle last? In milliseconds.
	// sparkleBrightness : How bright should the sparkle be? This value is added to the color of the wave. Between 0-255
	void animWaveSparkle( float waveWidth = 4.0, uint8_t sparkleChance = 100, uint16_t sparkleDuration = 500, uint8_t sparkleBrightness = 64 ){


		const uint32_t ms = millis();				// Current time
		const float perc = getPerc(ms);		// Percent through animation
		const uint8_t numSets = NUM_PIXELS/2;			// 
		//const float pixelDist = 1.0/numSets;
		
		// run on each pixel
		for( uint8_t i=0; i<numSets; ++i ){

			if( !quickRand(0, sparkleChance) )
				sparkle_started[i] = ms;

			const float myOffset = (float)i/numSets;		// Value between 0 and 1
			uint8_t r = 0, g = 0, b = 0;
			
			float s = sin(
				perc*PI*2+
				myOffset*PI*2/waveWidth
			)/2+0.5;

			float out = (
				0.05 + s
			)*fadePerc*255;

			float sparklePerc = 0;
			// There's a sparkle here
			if( ms-sparkle_started[i] < sparkleDuration )
				sparklePerc = 1.0-(float)(ms-sparkle_started[i])/sparkleDuration;

			r = min(out*maxRed+sparkleBrightness*sparklePerc, 255);
			g = min(out*maxGreen+sparkleBrightness*sparklePerc, 255);
			b = min(out*maxBlue+sparkleBrightness*sparklePerc, 255);

			setLED(i, r, g, b);

		}
		
		output();
		
	}


	// Animations you can use
	void animFire(){


		const uint32_t ms = millis();				// Current time
		//const float perc = getPerc(ms);		// Percent through animation	
		const uint8_t numSets = NUM_PIXELS/2;
		//const float pixelDist = 1.0/numSets;

		// Trigger a flare
		if( ms-timer_a > timer_b+timer_c ){

			timer_a = ms;	// Started
			timer_b = quickRand(200,500);	// Duration
			timer_c = quickRand(1,200);	// Cooldown

		}
		// Percentage of the flash
		
		const float flashPerc = (float)(ms-timer_a)/timer_b;

		int16_t offs = quickRand(5, 15);	// Global illumination
		delay(1);
		int16_t tr = quickRand(5, 25);		// Equalizer looking thing
		if( !quickRand(0, 10) )
			torchDir = !torchDir;

		if( torchDir )
			offs = -offs;

		torchDist = (torchDist+offs) % 628;
		torchReach = (torchReach+tr) % 628;
		
		// Can be calculated for ALL leds
		float globalBrightness = (sin((float)torchDist/100)/2+0.5)*.75+0.25;
		
		float sineBrite = (sin((float)torchReach/100)/2+0.5);
		const uint8_t sineStart = 3;	// Always have 3 LEDs on
		const uint8_t sineLength = 3;	// Fade in and out 2 LEDs on top of that
		
		// run on each pixel
		for( uint8_t i=0; i<numSets; ++i ){
			
			const uint8_t li = numSets-i-1;					// Reverse direction
			const float myOffset = (float)li/numSets;		// Value between 0 and 1
			uint8_t r = 0, g = 0, b = 0;
			
			// Calculate flashes
			float flashAdd = 0; // 0->1.0
			const float dist = (float)(flashPerc-myOffset)*4;		// How far away are we from the center of the flash
			if( 
				dist > 0 && 	// Allow only if the LED isn't before the flash
				dist <= 1.0 	// Distance is not greater than the tail length
			)flashAdd = 1.0-dist;	// Add 1 if close, drop off as you get further



			/*
			float out = (
				0.05 +
				(sin(perc*PI*2)/2 + 0.5) * 0.2+	// 0->0.2
				flashAdd * 0.75					// 0->0.75
			)*fadePerc*maxBrightness;
			*/
			
			float sineBrightness = 1.0;	// Fire going back and forth like an equalizer
			if( li >= sineStart+sineLength )
				sineBrightness = 0;
			else if( li >= sineStart && li < sineStart+sineLength ){

				float step = 1.0/sineLength;
				float local = (float)(li+1-sineStart)/sineLength;	// Value between 0 and 1
				if( sineBrite < local-step )	// We're under this LED
					sineBrightness = 0;
				else if( sineBrite < local ){	// This LED should be faded in/out
					sineBrightness = (local-sineBrite)/step;
				}

			}

			

			uint8_t out = 255*(globalBrightness*sineBrightness+flashAdd*3);

			g = min(255, out*maxGreen);
			b = min(255, out*maxBlue);
			r = min(255, out*maxRed);


			setLED(i, r, g, b);

		}
		
		output();

	}



	

}



#endif

