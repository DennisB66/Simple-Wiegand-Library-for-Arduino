// Copyright  : Dennis Buis (2017)
// License    : MIT
// Platform   : Arduino
// Library    : Simple Wiegand Library for Arduino
// File       : Wiegand.cpp
// Purpose    : Receiving IDs from a Wiegand compatible device (reader / keypad)
// Repository : https://github.com/DennisB66/Simple-Wiegand-Library-for-Arduino

#include <Arduino.h>
#include "Wiegand.h"

#define NO_WIEGAND_DEBUG  // use WIEGAND_DEBUG for debug info

#define MAX_WAIT  25      // max ticks between bits recevied

volatile unsigned long  Wiegand::_tick     = 0;
volatile unsigned long  Wiegand::_data     = 0;
volatile int            Wiegand::_bitCount = 0;

// initializes the Wiegand device connection (line D0 / D1)
void Wiegand::begin( int pinD0, int pinD1)
{
  _clrBuffer();

  pinMode( _pinD0 = pinD0, INPUT);    // Set pin used for line D0 as input
  pinMode( _pinD1 = pinD1, INPUT);    // Set pin used for line D1 as input

  noInterrupts();
  attachInterrupt( digitalPinToInterrupt( pinD0), _pulseD0, FALLING);
  attachInterrupt( digitalPinToInterrupt( pinD1), _pulseD1, FALLING);
  interrupts();                       // Hardware interrupt = high to low pulse on line D0/D1
}

// checks is a Wiegand device is connected to pinD0 / pinD1
bool Wiegand::hasDevice()
{
  return ( digitalRead( _pinD0) == HIGH) && ( digitalRead( _pinD1) == HIGH);
}

// checks if a new Wiegand ID has been received
bool Wiegand::available()
{
  unsigned long tick   = millis();    // look at stopwatch
  bool          result = false;       // false = no valid tag available (yet)

  if (( tick - _tick) > MAX_WAIT) {   // elapsed = last bit received
    noInterrupts();                   // block (new) incoming bits
    result = _validateKeyData() | _validateTagData();
                                      // process bits received (true = valid tag)
    if ( result) {
      #ifdef WIEGAND_DEBUG
      Serial.println();               // new line for bitstream debug
      #endif

      _code = _data;                  // copy bit buffer
      _type = _bitCount;              // set wiegand type (4/8/26/34)
    }

    _clrBuffer();                     // prepare for next tag
    interrupts();                     // allow (new) incoming bits
	}

	return result;
}

// returns the last active Wiegand code
unsigned long Wiegand::getCode()
{
  return _code;                       // code has max 32 bits (= W32 without parity bits)
}

// returns the last active Wiegand type
int Wiegand::getType()
{
  return _type;                       // type ~ nr bit received (4 / 8 / 26 / 34)
}

// <internal function:> resets read buffer
void Wiegand::_clrBuffer()
{
  _tick     = millis();               // reset stopwatch
  _data     = 0;                      // reset buffer
  _bitCount = 0;                      // reset counter
}

// <internal function:> process '0' bits coming on line D0
void Wiegand::_pulseD0()
{
  #ifdef WIEGAND_DEBUG
  Serial.print( "0");                 // bitstream debuf
  #endif

  _writeDx( 0x00);                    // next bit = 0
}

// <internal function:> process '1' bits coming on line D1
void Wiegand::_pulseD1()
{
  #ifdef WIEGAND_DEBUG
  Serial.print( "1");                 // bitstream debuf
  #endif

  _writeDx( 0x01);                    // next bit = 1
}

// <internal function:> store received bits in buffer
void Wiegand::_writeDx( byte bit)
{
  _tick = millis();                   // keep ticks between bits received
  _bitCount++;

  if ( _bitCount >= 34) return;       // ignore last patity bit for WG 34

  _data <<= 1;                        // shift lo bits left, new lo bit 0 = 0
  _data |= ( bit & 0x01);             // add new bit 0 to lo bit 0
}

// <internal function:> validate keypad based code (4 / 8 bit)
bool Wiegand::_validateKeyData()
{
  byte loNibble =   (_data & 0x0F);                     // first nibble = key
  byte hiNibble = ~((_data & 0xF0) >> 4);               // LO = ~HI (8 bit check)

  if (( _bitCount == 4) ||                              // 4 bit = no error check
     (( _bitCount == 8) && ( loNibble == hiNibble))) {  // 8 bit = do error check
    _data = loNibble;                                   // ignore hiNibble
    return true;
  }

  return false;
}

// <internal function:> validate reader based data (W26 / W34)
bool Wiegand::_validateTagData()
{
  if (_bitCount == 26) {
    _data >>= 1;                      // ignore last  parity bit
    _data  &= 0x00FFFFFF;             // ignore first parity bit
    return true;                      // W34 (24 bits = first / last parity bit ignored)
  }

  if (_bitCount == 34) {
    return true;                      // W34 (32 bits = first / last parity bit ignored)
  }

  return false;                       // no valid reader data
}
