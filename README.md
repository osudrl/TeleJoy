# CASSIE Telemetry in MuJoCo 

This teensy (Arduino) sketch will read serial data from the XSR (X4RS_Rev1.1) telemetry reciever and decode the information into controller axes and buttons.  This controller data can then be used to interact with the operating system as a Joystick device.

The Teensy 3.2 (left) connected to the XSR reciever (right):

<img src="http://i.imgur.com/vlCQ2Rf.jpg?1" width="600"> 

Back of the XSR receiver:

<img src="http://i.imgur.com/2jpbEeh.jpg?1" width="400"> 

## Patching Teensy Cores

Patching teensy cores with the files in patched-teensy3-cores allows for the emulated to joystick to have additional analog outputs to accommodate all 16 channels of the TARANIS.

## Uploading to the Teensy



## Binding with the TARANIS plus

The TARANIS plus controller:

![controller](http://cdn.shopify.com/s/files/1/0412/2761/products/taranis-x9d-plus3_grande.jpg?v=1468705173)

XSR when searching for controller to bind to:

![searching](http://i.imgur.com/orpNlgs.gif?1)

XSR when properly bound to a controller and recieving data:

![bound](http://i.imgur.com/f1CMw7O.jpg?1)

## Input from the XSR reciever (sbus_data_t struct)

Although the output channels can be configured within the TARANIS menu, at the time of this writing, the 16 analog channels corrospond to the following features on the controller.

Input | Struct Call | Label | Description | States
--- | --- | --- | --- | ---
A0 | analog[0] | None | Lstick up/down | Analog
A1 | analog[1] | None | Lstick left/right | Analog
A2 | analog[2] | None | Rstick up/down | Analog
A3 | analog[3] | None | Rstick left/right | Analog
A4 | analog[4] | S1 | Left Top Knob | Analog
A5 | analog[5] | S2 | Right Top Knob  | Analog
A6 | analog[6] | None | Left Side Knob | Analog
A7 | analog[7] | None | Right Side Knob | Analog
A8 | analog[8] | SA | Far Left Switch | 3
A9 | analog[9] | SB | Round Left Switch | 3
A10 | analog[10] | SC | Round Right Switch | 3
A11 | analog[11] | SD | Far Right Switch | 3
A12 | analog[12] | SE | Left Front Switch | 3
A13 | analog[13] | SF | Left Back Switch | 2
A14 | analog[14] | SG | Right Front Switch | 3
A15 | analog[15] | SH | Right Back **Button** | 2\*
D0 | digital[0] | ? | Haven't seen these digital outputs change from `0` | Digital
D1 | digital[1] | ? | See above | Digital
FL | frame_lost | N/A | `1` when controller off, otherwise `0` | Digital
FA | failsafe_active | N/A | `1` few seconds after controller off, otherwise `0` | Digital

States Type | Numerical Outputs
-- | ---
Analog | -820 to 819
3 | -820, 0, and 819
2 | -820 and 819
Digital | 0 and 1

>\* For the A15/SH button, a value of 819 represents the "unpressed" state and -820 is the "pressed" state

