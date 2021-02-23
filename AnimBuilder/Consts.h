#ifndef __neoPocketConf
#define __neoPocketConf

// Debug settings
//#define DEBUG_IGNORE_BATTERY

#define NUM_PIXELS 20

const uint16_t BATTERY_THR = 740;			// when below this value, battery is considered low

const float KEEPALIVE_THRESH = 3.0;			// Threshold to keeping the light alive. Lower is more sensitive.



#define PIN_BIGPP 10			// Turns on on LOW
#define PIN_RAND 4
#define PIN_INTERRUPT 2

#define PIN_CHRG_DET 0		// HIGH when charger is present, LOW when unplugged
#define PIN_CHRG_STAT 1		// LOW when charging, HIGH when full
#define PIN_BAT_READ_IN 15

#define PIN_EXT_BUTTON 11		// external chassis button
#define PIN_RESET 16
#define PIN_BOOT_LED 3

#define PIN_NEO_DTA 14



#endif
