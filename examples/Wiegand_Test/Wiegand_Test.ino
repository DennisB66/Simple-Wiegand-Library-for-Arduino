// Copyright	: Dennis Buis (2017)
// License  	: MIT
// Platform		: Arduino
// Library		:	Simple Wiegand Library for Arduino
// File				: Wiegand_EEPROM_Test.ino
// Purpose		: Example code for Wiegand library (see Wiegand.h)
// Repository	:	https://github.com/DennisB66/Simple_Wiegand_Library_for_Arduino

#include <Wiegand.h>

Wiegand wg;

void setup() {
  Serial.begin( 9600);

  Serial.println( "RFID reader initalizing... ");
	wg.begin();

  if ( wg.hasDevice()) {
		Serial.println( "RFID reader ready");
	} else {
		Serial.println( "No RFID reader attached!");
	}
}

void loop() {
  if ( wg.available()) {
    Serial.print  ( "Tag = ");
    Serial.println( wg.getCode(), HEX);
  }
}
