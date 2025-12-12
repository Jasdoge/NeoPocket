#ifndef __Tools
#define __Tools

#include "Consts.h"

namespace Tools{

	float getBatteryPercentage(){

		// Take a battery reading and output it.
		const uint16_t reading = analogRead(PIN_BAT_READ_IN);
		Serial.print("Battery raw: ");
		Serial.println(reading);
		float perc = ((reading+0.5)*2.5/1024.0); // Get actual voltage on pin
    	perc -= 1.7;		// 1.7 is the value at 3.4V, which can be considered bottom
		perc /= (2.5-1.7);	// Convert to a percentage
		if( perc < 0 )
			perc = 0;
		else if( perc > 1 )
			perc = 1;
		return perc;

		//voltage *= (107.87/7.87);    // Ratio is 1:1 on the resistors, so this isn't needed

	}

}



#endif



