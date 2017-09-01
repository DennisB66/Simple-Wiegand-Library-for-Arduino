// Copyright	: Dennis Buis (2017)
// License  	: MIT
// Platform		: Arduino
// Library		:	Simple Wiegand Library for Arduino
// File				: Wiegand_EEPROM_Test.ino
// Purpose		: Example code for Wiegand_EEPROM library (see Wiegand_EEPROM.h)
// Repository	:	https://github.com/DennisB66/Simple-Wiegand-Library-for-Arduino

#include <Wiegand_EEPROM.h>

Wiegand_EEPROM wg;

int mode = NORMAL;  // active mode

void setup() {
  Serial.begin( 9600);

  Serial.println( "=================================");
  Serial.println( "-  Arduino RFID WG reader test  -");
  Serial.println( "=================================");
  Serial.println( "");
  Serial.println( "RFID reader initalizing... ");

  wg.begin();

  if ( wg.hasDevice()) {
    Serial.println( "RFID reader ready");
  } else {
    Serial.println( "No RFID reader attached!");
  }

  printTags();
  printMenu();
}

void loop() {
  if ( wg.available()) {
    Serial.print( "Wiegand HEX = ");
    Serial.print( wg.getCode(), HEX);
    Serial.print( " (W");
    Serial.print( wg.getType());
    Serial.print( ")");
    Serial.println();

    if ( mode == NORMAL) {
      if ( wg.getSlot() < 0) {
        Serial.println( "Warning: tag not authorized!");
      }
    }

    if ( mode == INSERT) {
      if ( wg.getSlot() < 0) {
        if ( wg.createTag()) {
          Serial.print( "Inserted: ");
          Serial.print( wg.getCode(), HEX);
          Serial.println();
        } else {
          Serial.println( "Warning: max tags stored!");
        }
      } else {
        Serial.println( "Warning: tag already existing!");
      }
      Serial.println();
    }

    if ( mode == DELETE) {
      if ( wg.getSlot() < 0) {
        Serial.println( "Warning: tag not existing!");
      } else {
        if ( wg.deleteTag()) {
        	Serial.print( "Deleted: ");
        	Serial.print( wg.getCode(), HEX);
        	Serial.println();
        }
      }
    }

    printTags();
    printMenu();
  }

  if ( Serial.available() > 0) {
    char c = Serial.read();

    if ( c == 'i') { mode = INSERT; printMenu();};
    if ( c == 'd') { mode = DELETE; printMenu();};
    if ( c == 'n') { mode = NORMAL; printMenu();};
  }
}

// show active mode plus options
void printMenu() {
  if ( mode == NORMAL ) {
    Serial.println( "MODE = NORMAL (i = insert / d = delete)");
  }
  if ( mode == INSERT ) {
    Serial.println( "MODE = INSERT (n = normal / d = delete)");
  }
  if ( mode == DELETE ) {
    Serial.println( "MODE = DELETE (n = normal / i = insert)");
  }
  Serial.println();
}

// print tag data in EEPROM database to the serial monitor for a specific slot = idx
void printTag( int slot, bool flag)
{
  if (( slot < 0) || ( slot > MAX_TAGS - 1)) {
    // error
    Serial.println( "error: index out of range");
  } else {
    Serial.print( "tag ");
    Serial.print( slot, HEX);
    Serial.print( ": ");

    unsigned long code = wg.getCode( slot);

    // check if tag data is valid
    if ( code > 0) {
      char tmp[16]; sprintf( tmp, "%08lX", code);
      Serial.print( tmp);
    } else {
      Serial.print( "--------");
    }
  }

  if ( flag) {
    if ( mode == NORMAL) { Serial.println( " (authorized)"); }
    if ( mode == INSERT) { Serial.println( " (inserted)"); }
    if ( mode == DELETE) { Serial.println( " (deleted)"); }
  } else {
    Serial.println();
  }
}

// print all tag data in EEPROM database to the serial monitor, active tag in slot = idx
void printTags()
{
  Serial.println();

  for ( int i = 0; i < MAX_TAGS; i++) {
    printTag( i, i == wg.getSlot());
  }
}
