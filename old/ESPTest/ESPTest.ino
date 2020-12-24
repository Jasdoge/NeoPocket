#include "LIS3DH_Lite.h"
#include <Wire.h>

void setup(){
	/*
	Serial.begin(74880);
	delay(100);
	Serial.println("IT BEGINS");
	Serial.println();
	//pinMode(D3, INPUT);

	Serial.println("Running i2c only version");
*/

	bool addr;
  	while( !LIS3DH_Lite::begin(addr ? 0x19 : LIS3DH_DEFAULT_ADDRESS) ) {   // change this to 0x19 for alternative i2c address
		//Serial.print(".");
		addr = !addr;
		delay(1000);
  	}

	LIS3DH_Lite::setRange(LIS3DH_RANGE_16_G);
	//Serial.println("Hokay we're running!");
	

}

void loop(){

	LIS3DH_Lite::read();
	/*
	float x = LIS3DH_Lite::x_g;
	float y = LIS3DH_Lite::y_g;
	float z = LIS3DH_Lite::z_g;
	*/
	//Serial.printf("%ix %iy %iz\n", x, y, z);


	delay(1000);


}