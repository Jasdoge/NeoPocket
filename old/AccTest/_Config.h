#ifndef __neoPocketConf
#define __neoPocketConf

#define ANIM_COLOR 0x2		// Bitwise combination of RGB for color of the theme
#define NUM_PIXELS 20

#define PIN_BIGPP 0			// Turns on on LOW
#define PIN_RAND 1
#define PIN_CHARGE_EN 2		// > 0A when charged
#define PIN_INTERRUPT 3
#define PIN_UNUSED_A 4
#define PIN_BATTERY_READ_EN 5	// Turns on battery reading
// 6 & 7 are I2C pins
#define PIN_CHARGE_STAT 8		// internal pullup, LOW when charging
#define PIN_BATTERY_READ_IN 9
#define PIN_NEO_DTA 10

#define ANIM_FRAME() npAnim::animKryptonite()



#endif
