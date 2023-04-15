# Smallsat repository
All the arduino code necessary for the suborbital rocket launch challenge.

# Repository usage instructions
Only make changes to the 'active' brach.

# Useful links
- Domino4 github: https://github.com/domino4com
- Pin configuration of core: https://github.com/domino4com/CWV 
- List of sensors: https://github.com/domino4com/xChips 
- Prototype board tutorial: https://www.youtube.com/watch?v=Ia6bBskam-o 

# Order for completing examples
- hello_world
- accelerometer
- barometer
- barometeri2c
- SD_card
- barometer_SD
- accelerometer_SD

# Arduino IDE setup
Add the link to Additional Boards Manager URLs in the preferences tab: https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

# What to do if the core 'bricks'
The core is prone to failure, i.e. 'bricking' when the SD card is connected.
Data packets fails to return when uploading a sketch when the core is 'bricked'.
If this happens, do the following:
- Install esptool via a terminal using "pip install esptool" (if not installed already)
- Remove the core from usb
- Remove sd card from core
- Reinsert core into usb
- Immediately run "python -m esptool erase_flash" in terminal
- The core should now be fixed 
