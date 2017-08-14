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

## Output of the TARANIS (sbus_data_t struct)

XSR Output | Label | Description | States
--- | --- | --- | ---
A1 | None | Lstick up/down | Analog
A2 | None | Lstick left/right | Analog
A3 | None | Rstick up/down | Analog
A4 | None | Rstick left/right | Analog
A5 | S1 | Left Top Knob | Analog
A6 | S2 | Right Top Knob  | Analog
A7 | None | Left Side Knob | Analog
A8 | None | Right Side Knob | Analog
A9 | SA | Far Left Switch | 3
A10 | SB | Round Left Switch | 3
A11 | SC | Round Right Switch | 3
A12 | SD | Far Right Switch | 3
A13 | SE | Left Front Switch | 3
A14 | SF | Left Back Switch | 2
A15 | SG | Right Front Switch | 3
A16 | SH | Right Back **Button** | 2\*
D1 | ? | Haven't seen these digital outputs change from `0` | Digital
D2 | ? | See above | Digital
frame_lost | N/A | This value goes from `0` to `1` as soon as the controller is turned off after previously being connected | Digital
failsafe_active | N/A | This value goes from `0` to `1` a few seconds after the above goes to 1 | Digital

States Type | Numerical Outputs
-- | ---
Analog | -820 to 819
3 | -820, 0 and 819
2 | -820 and 819
Digital | 0 and 1

>\* For the A16/SH button, a value of 819 represents the "unpressed" state and -820 is the "pressed" state

## Uploading to the Teensy

## Pairing with the TARANIS plus
