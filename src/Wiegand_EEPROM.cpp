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
#include "SimpleUtils.h"

#define NO_WIEGAND_EEPROM_DEBUG                             // use Wiegand_EEPROM_DEBUG for debug info

// constructor
Wiegand_EEPROM::Wiegand_EEPROM( int pinD0, int pinD1) : Wiegand( pinD0, pinD1)
{
  _slot = -1;                                               // reset last active slot (no active entry)

  _EEPROM2Tags();                                           // copy EEPROM memory to active memory
}

// checks if a new Wiegand code has been received
bool Wiegand_EEPROM::available()
{
    if ( Wiegand::available()) {
      searchTag( _tagCode);                                 // set last active slot if existing in EEPROM

      return true;                                          // new Wiegand data available
    }

    return false;                                           // no new Wiegand data available
}

// return last active slot
int Wiegand_EEPROM::getSlot()
{
  return _slot;                                             // last active slot (-1 = no active entry)
}

// return tag code from lEEPROM ist
unsigned long Wiegand_EEPROM::getTagCode( int slot)
{
  if (( slot >= 0) || ( slot < MAX_TAGS)) {
    return _tags[slot].tag;                                 // return tag code entry
  } else {
    return 0x00000000;                                      // failure: outside array boundaries
  }
}

// return key code from EEPROM list
unsigned long Wiegand_EEPROM::getKeyCode( int slot)
{
  if (( slot >= 0) || ( slot < MAX_TAGS)) {
    return _tags[slot].key;                                 // return tag code entry
  } else {
    return 0;                                               // failure: outside array boundaries
  }
}

// search for last active tag in EEPROM list (true = found)
bool Wiegand_EEPROM::searchTag()
{
  return searchTag( _tagCode);
}

// search for specic tag in EEPROM list (true = found)
bool Wiegand_EEPROM::searchTag( unsigned long tag)
{
  _slot = -1;                                               // not found = no success (yet)

  for ( int i = 0; i < MAX_TAGS; i++) {
    if ( tag == _tags[ i].tag) {                            // tag found in EEPROM list
      _slot = i;

      return true;                                          // success
    }
  }

  return false;                                             // failure
}

// return specic key in active EEPROM slot (true = found)
bool Wiegand_EEPROM::searchKey()
{
  if (( _slot >= 0) || ( _slot < MAX_TAGS)) {
    return ( _keyCode == _tags[ _slot].key);                // true = key found in EEPROM list
  } else {
    return false;                                           // failure: outside array boundaries
  }
}

// create tag entry for last active code in EEPROM
bool Wiegand_EEPROM::createTag()
{
  if ( getType() == WTAG) return createTag( _tagCode);      // create tag entry for last active code
  if ( getType() == WKEY) return createTag( _tagCode, _keyCode);
                                                            // create key entry for last active code
  return false;
}

// create tag / key entry in EEPROM list
bool Wiegand_EEPROM::createTag( unsigned long tag, unsigned long key)
{
  for ( int i = 0; i < MAX_TAGS; i++) {
    if ( _tags[i].tag == tag) {
      _tags[i].tag = tag;                                   // update tag entry at empty slot
      _tags[i].key = key;                                   // update tag entry at empty slot
      _tags2EEPROM();                                       // update EEPROM

      _slot = i;

      return true;                                          // tag already existing / key created
    }
  }

  for ( int i = 0; i < MAX_TAGS; i++) {
    if ( _tags[i].tag == 0x00000000) {                      // empty slot found
      _tags[i].tag = tag;                                   // create tag entry at empty slot
      _tags[i].key = key;                                   // create key entry at empty slot
      _tags2EEPROM();                                       // update EEPROM

      _slot = i;                                            // set active slot

      return true;                                          // tag already existing / key created
    }
  }

  return false;                                             // failure: no tag / key creation
}

// delete tag entry for last active code in EEPROM (true = deleted)
bool Wiegand_EEPROM::deleteTag()
{
  return deleteTag( _code);                                 // delete tag entry for last active code
}

// delete tag entry for a specific slot in EEPROM (true = deleted)
bool Wiegand_EEPROM::deleteTag( int slot)
{
  if (( slot >= 0) || ( slot < MAX_TAGS)) {
    _tags[ slot].tag = 0x00000000;                          // clear tag entry at slot = slot
    _tags[ slot].key = 0;                                   // clear tag entry at slot = slot
    _tags2EEPROM();                                         // update EEPROM

    return true;                                            // success = slot wiped
  } else {
    return false;                                           // failure: outside array boundaries
  }
}

// delete tag entry for a specific code in EEPROM (true = deleted)
bool Wiegand_EEPROM::deleteTag( unsigned long tag)
{
  if ( searchTag( tag)) {                                   // search tag (and set active slot)
    return deleteTag( _slot);                               // delete tag entry at active slot
  } else {
    return false;                                           // failure: tag not found
  }
}

// delete all tags in EEPROM
void Wiegand_EEPROM::deleteAll() {
  for ( int i = 0; i < MAX_TAGS; i++){
    deleteTag( i);                                          // delete a spcific tag (slot = i)
  }
}

// copy EEPROM memory to active memory
void Wiegand_EEPROM::_EEPROM2Tags()
{
  for ( int i = 0; i < MAX_TAGS; i++) {
    EEPROM.get( i * sizeof(AccessCode), _tags[i]);          // copy EEPROM to buffer (byte-wise)
  }
}

// copy active memory to EEPROM memory
void Wiegand_EEPROM::_tags2EEPROM()
{
  for ( int i = 0; i < MAX_TAGS; i++) {
    EEPROM.put( i * sizeof(AccessCode), _tags[i]);          // copy EEPROM to buffer (byte-wise)
  }
}
