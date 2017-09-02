// Copyright  : Dennis Buis (2017)
// License    : MIT
// Platform   : Arduino
// Library    : Simple Wiegand Library for Arduino
// File       : Wiegand.h
// Purpose    : Receiving IDs from a Wiegand compatible device (reader / keypad)
// Repository : https://github.com/DennisB66/Simple-Wiegand-Library-for-Arduino

#ifndef _WIEGAND_H
#define _WIEGAND_H

#define PIN_D0_DEFAULT 2  // default pin for D0
#define PIN_D1_DEFAULT 3  // default pin for D0

class Wiegand {
public:
  void begin( int pinD0 = PIN_D0_DEFAULT, int pinD1 = PIN_D1_DEFAULT);
                          // initializes the Wiegand device connection (line D0 / D1)
  bool hasDevice();       // checks is a Wiegand device is connected to pinD0 / pinD1
  bool available();       // checks if a new Wiegand ID has been received

  unsigned long getCode( void);              // returns the last active Wiegand code
  int           getType();                   // returns the last active Wiegand type

protected:
  int _pinD0;                                // digital pin for reading line D0
  int _pinD1;                                // digital pin for reading line D1

  unsigned long _code;                       // last active Wiegand code
  int           _type;                       // last active Wiegand type

  static volatile unsigned long _tick;       // stopwatch for last received bit
  static volatile unsigned long _data;       // buffer for storing received bits
  static volatile int           _bitCount;   // number of bits received (so far)

  void _clrBuffer();                         // resets read buffer

  static void _pulseD0();                    // process bits coming on line D0
  static void _pulseD1();                    // process bits coming on line D0
  static void _writeDx( volatile byte);      // store received bits in buffer

  bool _validateKeyData();                   // validate keypad based code (4 / 8 bit)
  bool _validateTagData();                   // validate reader based code (W26 / W34)
};

#endif
