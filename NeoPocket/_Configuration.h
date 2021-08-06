#ifndef __Configuration
#define __Configuration

#include "Consts.h"
#include "Animator.h"
#include "Tools.h"

// Unquote to ignore battery charge detection
//#define DEBUG_IGNORE_BATTERY


namespace Configuration{

	// Configuration
	const uint32_t KEEPALIVE_DURATION = 30000; 	// On duration in milliseconds before starting to fade out.
	const uint32_t FADE_TIME = 30000;			// Time in milliseconds to fade out. Added to KEEPALIVE_DURATION.
	
	uint8_t mode = 0;							// Tracks the nr of button clicks
	uint8_t brightness = 0;						// Current max brightness of the pixels. Between 0 (off) and 255 (max brightness)


	// This is run when the device starts. Configure the default animation here
	void onSetup(){

		Animator::setColor(1.0,0.25,0.0);	// Set what color you want to use. 1.0 = max, and 0.0 is off. Red/green/blue.
		Animator::setDuration(4000);		// How long each cycle should be. In milliseconds. 4000 = 4 seconds
		Animator::setMaxBrightness(50);		// How bright it should shine, between 0 and 255 where 255 is max brightness.


	}

	// Raised 60 times per second(ish)
	// This is where you trigger your animation
	void onFrame(){

		// If mode is 3, set ALL pixels to max brightness
		if( mode == 3 ){
			
			Animator::setPixels(255, 255, 255);
			return;

		}

		// Otherwise animate
		//Animator::animKryptonite();			// See the Animator.h documentation for built in animations
		//Animator::animWave();
		//Animator::animRainbowFlat();
		Animator::animRainbowWave();
		//Animator::animWaveSparkle();
		//Animator::animFire();

	}


	// Button has been held for a while
	void onButtonLongPress(){

		// Get a battery percentage reading
		const float perc = Tools::getBatteryPercentage();

		// Calculate nr of steps to show
		uint8_t steps = NUM_PIXELS/2*perc;

		float r = 1.0, g = 1.0;
		if( perc < 0.5 )
			g = perc*2;
		else
			r = 1.0-(perc-0.5)*2;
			
		for( uint8_t i = 0; i<NUM_PIXELS/2; ++i ){
			
			if( i < steps )
				Animator::setLED(i, r*50, g*50);
			else
				Animator::setLED(i);

		}

		Animator::output();

		delay(2000);

	}

	// Raised when a user clicks the button
	void onButtonPress( bool longPress = false ){

		// Add 1 to mode each time
		++mode;
		if( mode > 3 )
			mode = 0;
		
		// In our case, we have 3 brightness modes plus one that's custom
		if( mode == 0 )
			Animator::setMaxBrightness(50);
		else if( mode == 1 )
			Animator::setMaxBrightness(128);
		else
			Animator::setMaxBrightness(255);

	}


	void onChargeStart(){
		Animator::setMaxBrightness(255);
	}

	// Replaces onFrame while a charger is plugged in
	void onChargeFrame( bool fullyCharged ){

		// Use the default charge animation
		Animator::animCharging(fullyCharged);

	}



}


#endif
