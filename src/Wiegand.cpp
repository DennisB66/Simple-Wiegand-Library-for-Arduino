// Copyright  : Dennis Buis (2017)
// License    : MIT
// Platform   : Arduino
// Library    : Simple Wiegand Library for Arduino
// File       : Wiegand.cpp
// Purpose    : Receiving IDs from a Wiegand compatible device (reader / keypad)
// Repository : https://github.com/DennisB66/Simple-Wiegand-Library-for-Arduino

#include <Arduino.h>
#include "Wiegand.h"
#include "SimpleUtils.h"

#define NO_WIEGAND_DEBUG                                    // use WIEGAND_DEBUG for debug info

#define WIEGAND_BITCOUNT_WAIT   25                          // max ticks between bits recevied
#define WIEGAND_KEYPRESS_WAIT  200                          // max ticks between keys recevied

const char* WGTypeLabel[3] = { "--N/A--", "W26/W34", "W04/W08"};

volatile unsigned long  Wiegand::_tick     = 0;             // system time of last received bit
volatile unsigned long  Wiegand::_data     = 0;             // full bit stream of received bits
volatile int            Wiegand::_bitCount = 0;             // number of bits received

// initializes the Wiegand device connection (line D0 / D1)
Wiegand::Wiegand( int pinD0, int pinD1)
{
  _clrDataBuffer();                                             // prepare data buffer

  pinMode( _pinD0 = pinD0, INPUT);                          // Set pin used for line D0 as input
  pinMode( _pinD1 = pinD1, INPUT);                          // Set pin used for line D1 as input

  _code = 0;                                                // clear code buffer
  _type = 0;                                                // clear type (= bitcount)
  _available = false;                                       // false = no code available (yet)
}

// initializes the Wiegand device connection (line D0 / D1)
void Wiegand::begin()
{
  noInterrupts();                                           // disable interupts while attaching
  attachInterrupt( digitalPinToInterrupt( _pinD0), _pulseD0, FALLING);
  attachInterrupt( digitalPinToInterrupt( _pinD1), _pulseD1, FALLING);
  interrupts();                                             // hardware interrupt = high to low pulse on line D0/D1
}

// reset active tag / key values
void Wiegand::reset()
{
  _clrCodeValues();
}

// checks is a Wiegand device is connected to pinD0 / pinD1
bool Wiegand::hasDevice()
{                                                           // return true = Wiegand device connected
  return ( digitalRead( _pinD0) == HIGH) && ( digitalRead( _pinD1) == HIGH);
}

// checks if a new Wiegand ID has been received
bool Wiegand::available()
{
  return _dataIsAvailable();                                // call internal handler
}

// returns the last active Wiegand code
unsigned long Wiegand::getCode()
{
  return _code;                                             // code holds the last tag value or key value
}

// returns the last active Wiegand type
WiegandType Wiegand::getType()
{
  if (( _type == 26) || ( _type == 34)) return WTAG;        // code received via reader
  if (( _type ==  4) || ( _type ==  8)) return WKEY;        // code received via keypad

  return NONE;
}

// returns the last active Wiegand code
unsigned long Wiegand::getTagCode()
{
  return _tagCode;                                          // return last tag code (as presented to reader)
}

// returns the last active Wiegand code
unsigned long Wiegand::getKeyCode()
{
  return _keyCode;                                          // return last key code (as entered via key pad)
}

// initialize last tag / key value
void Wiegand::_clrCodeValues()
{
  _tagCode = 0;                                             // reset (last) tag code
  _keyCode = 0;                                             // reset (last) key code
}

// <internal function:> resets read buffer
void Wiegand::_clrDataBuffer()
{
  _tick     = millis();                                     // reset stopwatch
  _data     = 0;                                            // reset buffer
  _bitCount = 0;                                            // reset counter
}

// <internal function:> process '0' bits coming on line D0
void Wiegand::_pulseD0()
{
  #ifdef WIEGAND_DEBUG
  Serial.print( "0");                                       // bitstream debuf
  #endif

  _writeDx( 0x00);                                          // next bit = 0
}

// <internal function:> process '1' bits coming on line D1
void Wiegand::_pulseD1()
{
  #ifdef WIEGAND_DEBUG
  Serial.print( "1");                                       // bitstream debuf
  #endif

  _writeDx( 0x01);                                          // next bit = 1
}

// <internal function:> store received bits in buffer
void Wiegand::_writeDx( byte bit)
{
  _tick = millis();                                         // keep ticks between bits received
  _bitCount++;

  if ( _bitCount >= 34) return;                             // ignore last patity bit for WG 34

  _data <<= 1;                                              // shift lo bits left, new lo bit 0 = 0
  _data |= ( bit & 0x01);                                   // add new bit 0 to lo bit 0
}

bool Wiegand::_dataIsAvailable()
{
  unsigned long tick   = millis();                          // look at stopwatch
  bool          result = false;                             // false = no valid tag available (yet)

  if (( tick - _tick) > WIEGAND_BITCOUNT_WAIT) {            // elapsed = last bit received
    noInterrupts();                                         // block (new) incoming bits
    result = _validateKeyData() | _validateTagData();       // process bits received (true = valid tag)

    if ( result) {
      #ifdef WIEGAND_DEBUG
      Serial.println();                                     // new line for bitstream debug
      #endif

      _code = _data;                                        // set wiegand type (4/8/26/34)
      _type = _bitCount;                                    // set wiegand type (4/8/26/34)

      _dataToTagCode();                                     // convert reader data to tag code
      _dataToKeyCode();                                     // convert keypad data to key code
    }

    _clrDataBuffer();                                           // prepare for next tag
    interrupts();                                           // allow (new) incoming bits
	}

  return result & _available;                               // true = data received & processed
}

// <internal function:> validate reader based data (W26 / W34)
bool Wiegand::_validateTagData()
{
  if (_bitCount == 26) {                                    // if type = W26
    _data >>= 1;                                            // ignore last  parity bit
    _data  &= 0x00FFFFFF;                                   // ignore first parity bit
    return true;                                            // W34 (24 bits = first / last parity bit ignored)
  }

  if (_bitCount == 34) {                                    // if type = W34
    return true;                                            // W34 (32 bits = first / last parity bit ignored)
  }

  return false;                                             // no valid reader data
}

// <internal function> validate keypad based code (4 / 8 bit)
bool Wiegand::_validateKeyData()
{
  byte loNibble =   (_data & 0x0F);                         // first nibble = key
  byte hiNibble = ~((_data & 0xF0) >> 4);                   // LO = ~HI (8 bit check)

  if (( _bitCount == 4) ||                                  // 4 bit = no error check
  (( _bitCount == 8) && ( loNibble == hiNibble))) {         // 8 bit = do error check
    _data = loNibble;                                       // ignore hiNibble
    return true;                                            // return success
  }

  return false;                                             // return failure
}

// <internal function> store tag entry in tag code
void Wiegand::_dataToTagCode()
{
  if ( getType() == WTAG) {                                 // check if tag data received
    _available = false;                                     // code not available (yet)

    _tagCode = _code;                                       // store tag code
    _keyCode = 0;                                           // key code as next step

    _available = true;                                      // publish code as available
  }
}

// <internal function> store key entry in key code
void Wiegand::_dataToKeyCode()
{
  static unsigned long data = 0;                            // local code buffer

  //IF_NOT_PASSED( WIEGAND_KEYPRESS_WAIT, true) return;       // debounce button

  if ( getType() == WKEY) {                                 // check if key data received
    _available = false;                                     // code not available (yet)

    if (( _data >= 0) && ( _data <= 9)) {                   // if digit pressed
      data = (( data * 10) + _data) % 1000000;              // add digit to code
    }

    if ( _data == 10) {                                     // key '*' = clear entry
    data = 0;
    }

    if ( _data == 11) {                                     // key '#' = confirm last digit
      _code      = data;                                    // store as last code received
      _keyCode   = data;                                    // store as key code
      _available = true;                                    // publish code as available

      data = 0;                                             // clear local code buffer (for new cylce)
    }
  } else {
    data = 0;                                               // clear local code buffer (no valid data)
  }
}
