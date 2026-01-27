#ifndef __Tools
#define __Tools

#include "Consts.h"

namespace Tools{

	float getBatteryPercentage(){

		// Take a battery reading and output it.
		const uint16_t reading = analogRead(PIN_BAT_READ_IN);
		float voltage = (reading/1024.0)*2.5; // Get actual voltage on pin
		voltage *= 2;
		float perc = (voltage-2.5)/(4.15-2.5); // Estimate a percentage
		if( perc < 0 )
			perc = 0;
		else if( perc > 1 )
			perc = 1;
		return perc;
		
	}

}



#endif



