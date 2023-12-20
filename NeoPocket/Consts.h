#ifndef __neoPocketConf
#define __neoPocketConf

// Debug settings
//#define DEBUG_IGNORE_BATTERY

#define NUM_PIXELS 20

const uint16_t BATTERY_THR = 740;			// when below this value, battery is considered low

const float KEEPALIVE_THRESH = 3.0;			// Threshold to keeping the light alive. Lower is more sensitive.



#define PIN_BIGPP PIN_PC0			// Turns on on LOW
#define PIN_RAND PIN_PB5
#define PIN_INTERRUPT PIN_PA6

#define PIN_CHRG_DET PIN_PA4		// HIGH when charger is present, LOW when unplugged
#define PIN_CHRG_STAT PIN_PA5		// LOW when charging, HIGH when full
#define PIN_BAT_READ_IN PIN_PA2

#define PIN_EXT_BUTTON PIN_PC1		// external chassis button
#define PIN_BOOT_LED PIN_PA7

#define PIN_NEO_DTA PIN_PA1



#endif
