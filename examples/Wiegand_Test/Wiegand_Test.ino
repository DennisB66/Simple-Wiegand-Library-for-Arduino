// Copyright  : Dennis Buis (2017)
// License    : MIT
// Platform   : Arduino
// Library    : Simple Wiegand Library for Arduino
// File       : Wiegand_Test.ino
// Purpose    : Example code for Wiegand library (see Wiegand.h)
// Repository : https://github.com/DennisB66/Simple-Wiegand-Library-for-Arduino

#include <Arduino.h>
#include "Wiegand.h"
#include "SimpleUtils.h"

Wiegand wg( 2, 3);    // D0 = pin 2 & D1 = pin 3

void setup() {
  BEGIN( 9600);

  PRINT( F( "# =========================")) LF;
  PRINT( F( "# -  RFID WG reader test  -")) LF;
  PRINT( F( "# =========================")) LF;
  PRINT( F( "# RFID reader initalizing... ")) LF;

  wg.begin();

  if ( wg.hasDevice()) {
    PRINT( F( "# RFID reader ready")) LF;
  } else {
    PRINT( F( "> No RFID reader attached!")) LF;
  }
}

void loop() {
  if ( wg.available()) {
    if ( wg.getType() == WTAG) {
      LABEL( F( "# Wiegand HEX = "), hex( wg.getTagCode(), 8));
    }

    if ( wg.getType() == WKEY) {
      LABEL( F( "# Wiegand DEC = "), dec( wg.getKeyCode(), 8));
    }

    LABEL( F( " "), WGTypeLabel[ wg.getType()]) LF;
  }
}
