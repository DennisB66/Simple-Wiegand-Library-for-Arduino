// Copyright  : Dennis Buis (2017)
// License    : MIT
// Platform   : Arduino
// Library    : Simple Wiegand Library for Arduino
// File       : Wiegand_EEPROM.cpp
// Purpose    : Storing IDs from a Wiegand compatible device (reader / keypad) in EEPROM
// Repository : https://github.com/DennisB66/Simple-Wiegand-Library-for-Arduino

#include <Arduino.h>
#include <EEPROM.h>
#include "Wiegand_EEPROM.h"

#define NO_WIEGAND_EEPROM_DEBUG // use Wiegand_EEPROM_DEBUG for debug info

// constructor
Wiegand_EEPROM::Wiegand_EEPROM()
{
  _code = 0x00000000;           // reset last active code (no active code)
  _type =  0;                   // reset last active type (no active type)
  _slot = -1;                   // reset last active slot (no active entry)

  _EEPROM2Tags();               // copy EEPROM memory to active memory
}

// checks if a new Wiegand code has been received
bool Wiegand_EEPROM::available()
{
    if ( Wiegand::available()) {
      _slot = -1;               // default = not existing in EEPROM
      searchTag( _code);        // set last active slot if existing in EEPROM

      return true;              // new Wiegand data available
    }

    return false;               // no new Wiegand data available
}

// return last active code
unsigned long Wiegand_EEPROM::getCode( int slot)
{
  if (( slot < 0) || ( slot > MAX_TAGS - 1)) {
    return 0x00000000;          // failure: outside array boundaries
  } else {
    return _tags[ slot];        // return tag entry
  }
}

// return last active slot (in EEPROM)
int Wiegand_EEPROM::getSlot()
{
  return _slot;                 // last active slot (-1 = no active entry)
}

// create tag entry for last active code in EEPROM
bool Wiegand_EEPROM::createTag()
{
  createTag( _code);            // create tag entry for last active code
}

// create tag entry for a specific code in EEPROM
bool Wiegand_EEPROM::createTag( unsigned long code)
{
  for ( int slot = 0; slot < MAX_TAGS; slot++) {
    if ( _tags[ slot] == code) {
      return false;             // failure: tag entry already existing
    }

    if (( _tags[ slot] == 0x00000000) || ( _tags[ slot] == 0xFFFFFFFF)) {
      _tags[ slot] = code;      // create tag entry at empty slot
      _slot = slot;             // update last active slot
      _tags2EEPROM();           // update EEPROM
      return true;              // success
    }
  }

  return false;                 // failure: no empty tag entry
}

// delete tag entry for last active code in EEPROM (true = deleted)
bool Wiegand_EEPROM::deleteTag()
{
  return deleteTag( _code);     // delete tag entry for last active code
}

// delete tag entry for a specific slot in EEPROM (true = deleted)
bool Wiegand_EEPROM::deleteTag( int slot)
{
  if (( slot < 0) || ( slot > MAX_TAGS - 1)) {
    return false;               // failure: outside array boundaries
  } else {
    _tags[ slot] = 0x00000000;  // clear tag entry at slot = slot
    _tags2EEPROM();             // update EEPROM
    return true;                // success
  }
}

// delete tag entry for a specific code in EEPROM (true = deleted)
bool Wiegand_EEPROM::deleteTag( unsigned long code)
{
  searchTag(  code);            // determine slot for a specific code
  deleteTag( _slot);            // delete tag entry at last active slot
}


// delete all tags in EEPROM
void Wiegand_EEPROM::deleteAll() {
  for ( int i = 0; i < MAX_TAGS; i++){
    deleteTag( i);              // delete a spcific tag at slot = i
  }
}

// search tag entry for last active code in EEPROM (true = found)
bool Wiegand_EEPROM::searchTag()
{
  searchTag( _code);             // determine slot for last active code
}

// search tag entry for a specific code in EEPROM (true = found)
bool Wiegand_EEPROM::searchTag( unsigned long code)
{
  _slot = -1;                     // not found = no success (yet)

  for ( int slot = 0; slot < MAX_TAGS; slot++) {
    if ( code == _tags[ slot]) {  // code found in array
       // tag found
       _slot = slot;              // set last active slot
       return true;               // success
    }
  }

  return false;                   // failure
}

// object used to convert tag IDs from 4 byte array to unsigned long (and vv)
union TagData {
   unsigned long l;               // unsigned long (= 4 bytes)
   byte          b[ 4];           // 4 bytes (= unsigned long)
};

// copy EEPROM memory to active memory
void Wiegand_EEPROM::_EEPROM2Tags()
{
  TagData tag;                                    // temporary byte buffer

  for ( int slot = 0; slot < MAX_TAGS; slot++) {
    for ( int i = 0; i < 4; i++) {
      tag.b[ i] = EEPROM.read( slot * 4 + i);     // copy EEPROM to buffer (byte-wise)
    }

    _tags[ slot] = tag.l;                         // copy bytes to array entry
  }
}

// copy active memory to EEPROM memory
void Wiegand_EEPROM::_tags2EEPROM()
{
  TagData tag;                                    // temporary byte buffer

  for ( int slot = 0; slot < MAX_TAGS; slot++) {
    tag.l = _tags[ slot];                         // copy array entry to buffer

    for ( int i = 0; i < 4; i++) {
      EEPROM.update( slot * 4 + i, tag.b[ i]);    // copy buffer to EEPROM (byte-wise)
    }
  }
}
