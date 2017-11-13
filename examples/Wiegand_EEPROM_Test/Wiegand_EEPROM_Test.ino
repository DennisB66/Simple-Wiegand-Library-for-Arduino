// Copyright  : Dennis Buis (2017)
// License    : MIT
// Platform   : Arduino
// Library    : Simple Wiegand Library for Arduino
// File       : Wiegand_EEPROM_Test.ino
// Purpose    : Example code for Wiegand_EEPROM library (see Wiegand_EEPROM.h)
// Repository : https://github.com/DennisB66/Simple-Wiegand-Library-for-Arduino

#include <Arduino.h>
#include <Wiegand_EEPROM.h>
#include "SimpleUtils.h"

Wiegand_EEPROM wg;

enum AccessMode { WAITING_TAG, WAITING_KEY, ACCESS_AUTH };  // states for state machine

int mode    = NORMAL;                                       // check on valid Access
         // = INSERT;                                       // insert tags + key codes
         // = DELETE;                                       // delete tags + key codes

void printMenu();                                           // show menu options
void printTag( int);                                        // show single tag / value (slot)
void printTags();                                           // show all tag / key values
void deleteAll();                                           // wipe all tag / key values

void setup() {
  BEGIN( 9600);

  PRINT( F( "# ================================")) LF;      // show header
  PRINT( F( "# -  RFID WG EEPROM reader test  -")) LF;
  PRINT( F( "# ================================")) LF;
  PRINT( F( "# RFID reader initalizing... ")) LF;

  wg.begin();                                               // start Wiegand device

  if ( wg.hasDevice()) {                                    // check on valid device
    PRINT( F( "# RFID reader ready")) LF;
  } else {
    PRINT( F( "> No RFID reader attached!")) LF;
  }

  printTags();                                              // show all tag / key values
  printMenu();                                              // show menu options
}

void loop() {
  if ( wg.available()) {                                    // if new tag / key value available
    if ( wg.getType() == WTAG) {                            // if tag value (W26 / W32)
      LABEL( F( "> Wiegand Tag Code = "), hex( wg.getTagCode(), 8));
      PRINT( F( " (")); PRINT( WGTypeLabel[wg.getType()]); PRINT( F( ")")) LF;
    }

    if ( wg.getType() == WKEY) {                            // if key value (W4 / W8)
      LABEL( F( "> Wiegand Key Code = "), dec( wg.getKeyCode(), 6));
      PRINT( F( " (")); PRINT( WGTypeLabel[wg.getType()]); PRINT( F( ")")) LF;
    }

    if ( mode == NORMAL) {                                  // if normal model
      static AccessMode mode = WAITING_TAG;                 // state for state machine

      switch ( mode) {
      case WAITING_TAG:                                     // next read must be a tag value
        if ( wg.searchTag()) {                              // if valid tag value found
          printTags();
          printMenu();
          mode = WAITING_KEY;                               // goto next state
        } else {
          PRINT( F( "> Access denied!!!!!")) LF;
          break;
        }

      case WAITING_KEY:                                     // next read must be a key value
        if ( wg.searchKey()) {                              // if no valid key value required
        } else {
          PRINT( F( "> Enter key code... ")) LF;
          mode = ACCESS_AUTH;                               // goto next state
          break;
        }

      case ACCESS_AUTH:
        if ( wg.searchTag() && wg.searchKey()) {            // if valid tag & key value found
          PRINT( F( "> Access authorized!")) LF;
        } else {
          PRINT( F( "> Access denied!!!!!")) LF;
        }

      default:
        wg.reset();                                         // clear tag / key values
        mode = WAITING_TAG;                                 // restart state machine
      }
    }

    if ( mode == INSERT) {
      // if ( wg.searchTag()) {
      //   PRINT( F( "> Warning: tag already existing!")) LF;
      // } else
      if ( wg.createTag()) {
        printTags();
        printMenu();
      } else {
        PRINT( F( "> Warning: max tags stored!")) LF;
      }
    }

    if ( mode == DELETE) {
      if ( wg.deleteTag()) {
        printTags();
        printMenu();
      } else {
        PRINT( F( "> Warning: tag not existing!")) LF;
      }
    }
  }

  if ( Serial.available() > 0) {                            // check for new menu entry
    char c = Serial.read();                                 // read key entry

    if ( c == 'n') { mode = NORMAL; printMenu(); }          // change to normal mode
    if ( c == 'i') { mode = INSERT; printMenu(); }          // change to insert mode
    if ( c == 'd') { mode = DELETE; printMenu(); }          // change to delete mode
    if ( c == 'x') { mode = NORMAL; deleteAll(); }          // wipe all tag / key values
  }
}

// show active mode plus options
void printMenu() {
  if ( mode == NORMAL ) {
    PRINT( F( "# MODE = NORMAL (i = insert / d = delete)")) LF;
  }
  if ( mode == INSERT ) {
    PRINT( F( "# MODE = INSERT (n = normal / d = delete)")) LF;
  }
  if ( mode == DELETE ) {
    PRINT( F( "# MODE = DELETE (n = normal / i = insert)")) LF;
  }
}

// print all tag data in EEPROM database to the serial monitor, active tag in slot = idx
void printTags()
{
  for ( int i = 0; i < MAX_TAGS; i++) {
    printTag( i);
  }
}

// print tag data in EEPROM database to the serial monitor for a specific slot = idx
void printTag( int slot)
{
  if (( slot < 0) || ( slot > MAX_TAGS - 1)) {
    PRINT( F( "> error: index out of range")) LF;
  } else {
    LABEL( F( "# entry "), slot); PRINT( F( " > "));

    unsigned long tag = wg.getTagCode( slot);

    if ( tag >= 0) {
      LABEL( F( "tag = "), hex( tag, 8));
    } else {
      LABEL( F( "tag = "), F( "--------"));
    }

    unsigned long key = wg.getKeyCode( slot);

    if ( key >= 0) {
      LABEL( F( " & key = "), dec( key, 6));
    } else {
      LABEL( F( " & key = "), F( "--------"));
    }
  }

  if ( slot == wg.getSlot()) PRINT( F( " (*)")) LF;         // indicate active slot
}

void deleteAll()
{
  wg.deleteAll();
}
