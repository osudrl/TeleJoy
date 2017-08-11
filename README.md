# CASSIE Telemetry in MuJoCo 

This teensy (Arduino) sketch will read serial data from the XSR (X4RS_Rev1.1) telemetry reciever and decode the information into controller axes and buttons.  This controller data can then be used to interact with the operating system as a Joystick device.

The Teensy 3.2 (left) connected to the XSR reciever (right):

<img src="http://i.imgur.com/vlCQ2Rf.jpg?1" width="600"> 

Back of the XSR receiver:

<img src="http://i.imgur.com/2jpbEeh.jpg?1" width="400"> 

## Code Routine

To start, the Teensy program will do the following:

* Read Serial1 (pin0) with a custom Serial protocol (inverted, 2 stop bits, even parity)
* Fill a 25-byte buffer with the bytes coming in from Serial1
* Once full, decode the 25-length buffer
* After calling the decode function, reset the buffer and index
* If ever a >3000us delay between availible bytes, reset the index before saving that byte

## Uploading to the Teensy

## Pairing with the TARANIS plus
