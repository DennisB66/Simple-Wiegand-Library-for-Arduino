# Simple-Wiegand-Library-for-Arduino

Simple Wiegand is a library for Arduino that reads ID data from a Wiegang compatible device (e.g. a card reader) and is able to check the existence of this ID in a database stored in the Arduinio onboard EEPROM space. Non-existing IDs (tags) can be easily added to the ID database, existing IDs (tags) can be easily removed from the ID database.

The code is inspired by the existing Wiegand library (https://github.com/monkeyboard/Wiegand-Protocol-Library-for-Arduino).

The primary goal was to create a Arduino sketch that can open a garage door using a set of autorized RFID cards without hard coding the card IDs into the code. The example sketches shows how the library can be applied and allows quick testing of an attached Wiegand compatible RFID reader reacting on a set of RFID cards (Wiegand_Test) and how to store and retrieve tags from EEPROM (Wiegand_EEPROM_Test).

## Functionality
The functions in the class Wiegand include:
```
begin()               // initializes device
hasDevice()	      // checks if a Wiegand compatible device is attached
available()           // checks if a Wiegand ID is read from the device
getCode()             // returns the Wiegand code
getType()             // returns the Wiegand type (4 bit / 8 bit / 26 bit / 34 bit)
```

The functions in the class Wiegand_EEPROM include:
```
begin()               // initializes device
getCode()             // returns the last used Wiegand code
getType()             // returns the last used Wiegand type (4 bit / 8 bit / 26 bit / 34 bit)
available()           // indicate if a tag is available for reading from the Wiegand device
createTag()           // creates a new tag entry in the ID database
deleteTag()           // deletes an existing tag from the ID database
deleteAll()           // deletes all tags from the ID database
searchTag()           // searches the ID database for a specific tag
```

## Todo
- Extend for MFA based access (e.g. card + pin)

## Technical notes
<to be added>
