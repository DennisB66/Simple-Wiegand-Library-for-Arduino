# Simple-Wiegand-Library-for-Arduino

Simple Wiegand is a library for Arduino that reads ID data from a Wiegang compatible device (e.g. a card reader) and is able to check the existence of this ID in a database stored in the Arduinio onboard EEPROM space. Non-existing IDs (tags) can be easily added to the ID database, existing IDs (tags) can be easily removed from the ID database.

The code is inspired by an existing Wiegand library (https://github.com/monkeyboard/Wiegand-Protocol-Library-for-Arduino).

The primary goal was to create a Arduino sketch that can open a garage door using a set of autorized RFID cards without hard coding the card IDs into the code. The example sketches shows how the library can be applied and allows quick testing of an attached Wiegand compatible RFID reader reacting on a set of RFID cards (Wiegand_Test) and how to store and retrieve tags from EEPROM (Wiegand_EEPROM_Test).

Update 0.6: multi factor authentication supported (cards + pin code)

## Functionality
The functions in the class Wiegand include:
```
begin()               // initialize device
reset()		      // reset tag / key values
hasDevice()	      // check if a Wiegand device is connected
available()           // check if a new Wiegand ID has been received
getCode()             // return the Wiegand ID code
getType()             // return the Wiegand ID type (WKEY:4 bit / 8 bit, WTAG: 26 bit / 34 bit)
getTagCode()	      // return current Wiegand tag code
getKeyCode()	      // return current Wiegand key code
```

The functions in the class Wiegand_EEPROM extend the class Wiegand and include:
```
getSlot()	      // return current active slot in EEPROM database
searchTag()           // search the EEPROM database for a specific tag
searchKey()           // search the EEPROM database for a specific key
createTag()           // create a (new) tag / key entry in the EEPROM database
deleteTag()           // delete an existing tag / key from the EEPROM database
deleteAll()           // delete all tag / key entries from the EEPROM database
```

## Technical notes
<to be added>
