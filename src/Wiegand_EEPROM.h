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

#define MAX_TAGS 12     // max tags (4 bytes per tag) dependent on EEPROM size (512 bytes for Arduino Uno)
#define NORMAL    0     // normal mode = read tags / check authorization in EEPROM database
#define INSERT    1     // insert mode = insert tags to EEPROM database
#define DELETE    2     // delete mode = delete tags in EEPROM database

class Wiegand_EEPROM : public Wiegand
{
  public:
    Wiegand_EEPROM();               // constructor
    bool available();               // checks if a new Wiegand code has been received
                                    // sets last active slot if existing (in EEPROM)
    using Wiegand::getCode;         // return last active code
    unsigned long  getCode( int);   // return code in EEPROM at last active slot
    int            getSlot();       // return last active slot (in EEPROM)

    bool createTag();               // create tag entry for last active code in EEPROM (true = created)
    bool createTag( unsigned long); // create tag entry for a specific code in EEPROM (true = created)

    bool deleteTag();               // delete tag entry for last active code in EEPROM (true = deleted)
    bool deleteTag( int);           // delete tag entry for a specific slot in EEPROM (true = deleted)
    bool deleteTag( unsigned long); // delete tag entry for a specific code in EEPROM (true = deleted)
    void deleteAll();               // delete all tag entries in EEPROM

    bool searchTag();               // search tag entry for last active code in EEPROM (true = found)
    bool searchTag( unsigned long); // search tag entry for a specific code in EEPROM (true = found)


  protected:
    int           _slot;            // last actice slot (-1 = not found)
    unsigned long _tags[ MAX_TAGS]; // tags in EEPROM

    void _EEPROM2Tags();            // copy EEPROM memory to active memory
    void _tags2EEPROM();            // copy active memory to EEPROM memory
};

#endif
