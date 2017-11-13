// Copyright  : Dennis Buis (2017)
// License    : MIT
// Platform   : Arduino
// Library    : Simple Wiegand Library for Arduino
// File       : Wiegand_EEPROM.h
// Purpose    : Storing IDs from a Wiegand compatible device (reader / keypad) in EEPROM
// Repository : https://github.com/DennisB66/Simple-Wiegand-Library-for-Arduino

#ifndef Wiegand_EEPROM_h
#define Wiegand_EEPROM_h

#include <Wiegand.h>

#define MAX_TAGS 10                                         // max tags (4 bytes per tag) dependent on EEPROM size (512 bytes for Arduino Uno)
#define NORMAL    0                                         // normal mode = read tags / check authorization in EEPROM database
#define INSERT    1                                         // insert mode = insert tags to EEPROM database
#define DELETE    2                                         // delete mode = delete tags in EEPROM database

struct AccessCode {
  unsigned long tag;                                        // tag value
  unsigned long key;                                        // key value
};

class Wiegand_EEPROM : public Wiegand
{
  public:
    Wiegand_EEPROM(int = PIN_D0_DEFAULT, int = PIN_D1_DEFAULT);
                                                            // constructor
    bool available();                                       // checks if a new Wiegand code has been received

    int  getSlot();                                         // return current slot (in EEPROM list)

    using Wiegand::getTagCode;                              // return cuurent tag code
    using Wiegand::getKeyCode;                              // return current key code
    unsigned long  getTagCode( int);                        // return tag code from EEPROM list (indicated by slot)
    unsigned long  getKeyCode( int);                        // return key code from EEPROM list (indicated by slot)

    bool searchTag();                                       // search current tag in EEPROM list (true = found)
    bool searchTag( unsigned long);                         // search a given tag in EEPROM list (true = found)
    bool searchKey();

    bool createTag();                                       // create tag entry for current code in EEPROM (true = created)
    bool createTag( unsigned long, unsigned long = 0);      // create tag entry for a given code in EEPROM (true = created)

    bool deleteTag();                                       // delete tag entry for last active code in EEPROM (true = deleted)
    bool deleteTag( int);                                   // delete tag entry for a specific slot in EEPROM (true = deleted)
    bool deleteTag( unsigned long);                         // delete tag entry for a specific code in EEPROM (true = deleted)
    void deleteAll();                                       // delete all tag entries in EEPROM

  protected:
    int         _slot;                                      // current slot (-1 = not found)
    AccessCode  _tags[ MAX_TAGS];                           // tags in EEPROM

    void _EEPROM2Tags();                                    // copy EEPROM memory to active memory
    void _tags2EEPROM();                                    // copy active memory to EEPROM memory
};

#endif
