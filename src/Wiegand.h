// Copyright  : Dennis Buis (2017)
// License    : MIT
// Platform   : Arduino
// Library    : Simple Wiegand Library for Arduino
// File       : Wiegand.h
// Purpose    : Receiving IDs from a Wiegand compatible device (reader / keypad)
// Repository : https://github.com/DennisB66/Simple-Wiegand-Library-for-Arduino

#ifndef _WIEGAND_H
#define _WIEGAND_H

#define PIN_D0_DEFAULT 2                                    // default pin for line D0
#define PIN_D1_DEFAULT 3                                    // default pin for line D1

enum               WiegandType { NONE, WTAG, WKEY};         // indicates Wiegand data type (WTAG = 26/34, WKEY = 4/8)
extern const char* WGTypeLabel[3];

class Wiegand {
public:
  Wiegand( int = PIN_D0_DEFAULT, int = PIN_D1_DEFAULT);     // initialize Wiegand device defining pins for line D0 / D1

  void begin();                                             // initializes the Wiegand device connection (line D0 / D1)
  void reset();                                             // reset active tag / key values
  bool hasDevice();                                         // checks is a Wiegand device is connected to pinD0 / pinD1
  bool available();                                         // checks if a new Wiegand ID has been received

  unsigned long getCode();                                  // returns the last active Wiegand code
  WiegandType   getType();                                  // returns the last active Wiegand type

  unsigned long getTagCode();                               // returns the last active Wiegand code
  unsigned long getKeyCode();                               // returns the last active Wiegand code

protected:
  int _pinD0;                                               // digital pin for reading line D0
  int _pinD1;                                               // digital pin for reading line D1

  unsigned long _code;                                      // last active Wiegand code
  int           _type;                                      // last active Wiegand type
  bool          _available;                                 // last active Wiegand code

  unsigned long _tagCode;                                   // last active Wiegand code
  unsigned long _keyCode;                                   // last active Wiegand code

  static volatile unsigned long _tick;                      // stopwatch for last received bit
  static volatile unsigned long _data;                      // buffer for storing received bits
  static volatile int           _bitCount;                  // number of bits received (so far)

  void _clrCodeValues();                                    // reset last tag / key code values
  void _clrDataBuffer();                                    // reset data read buffer

  static void _pulseD0();                                   // process bits coming on line D0
  static void _pulseD1();                                   // process bits coming on line D0
  static void _writeDx( volatile byte);                     // store received bits in read buffer

  bool _dataIsAvailable();                                  // read data from reader or key pad

  bool _validateTagData();                                  // validate reader based data (W26 / W34)
  bool _validateKeyData();                                  // validate keypad based data (4 / 8 bit)

  void _dataToTagCode();                                    // convert reader based data to tag code
  void _dataToKeyCode();                                    // convert keypad based data to key code
};

#endif
