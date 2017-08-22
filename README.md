# TARANIS Telemetry to SDL Joystick

This teensy (Arduino) sketch will read serial data from the XSR (X4RS_Rev1.1) telemetry reciever and decode the information into controller axes and buttons.  This controller data can then be used to interact with the operating system as a Joystick device.

The Teensy 3.2 (left) connected to the XSR reciever (right):

<img src="http://i.imgur.com/vlCQ2Rf.jpg?1" width="600"> 

This project was written and uploaded in the Arduino IDE on Ubuntu Gnome 16.04.

# The Six Protocols

As [the image below](http://i.imgur.com/MqNwuJ3.png) illustrates, there are six different infromation exchange protocols used across the four devices in the project.

![the image below](http://i.imgur.com/MqNwuJ3.png)

<table>
  <tbody>
    <tr>
      <th>#</th>
      <th>Name</th>
      <th>Connections</th>
      <th>Rules</th>
      <th>Flow</th>
      <th>Code Usage</th>
    </tr>
    <tr>
      <td>1</td>
      <td><a href="https://developer.mbed.org/users/Digixx/notebook/futaba-s-bus-controlled-by-mbed/">SBUS</a></td>
      <td>XSR White --> Teensy Pin0 (Serial1)</td>
      <td>
      	<p>Serial:</p>
        <ul>
          <li>Two Stop Bits</li>
          <li>Even Parity</li>
          <li>Inverted</li>
          <li>100K Baud</li>
          <li><a href="https://github.com/osudrl/TeleJoy/blob/552806b4f3a114bf1baaf2a7d394ab663f4caab5/telejoy/telejoy.ino#L60">Declaration</a></li>
        </ul>
      </td>
      <td>
        <ul>
          <li>Fill a buffer of 25 bytes from Serial1</li>
          <li>Give the buffer and data struct pointer to sbus_decode_packet</li>
          <li>XSR Delays 7 or 14 ms between "packets" of 25 bytes</li>
        </ul>
      </td>
      <td>
      	<p>telejoy.ino:</p>
        <ul>
          <li>loop()</li>
          <li>sbus_decode_packet()</li>
        </ul>
      </td>
    </tr>
    <tr>
      <td>2</td>
      <td>S. PORT (IN)</td>
      <td>XSR Yellow --> Teensy Pin8 (Serial3 TX)</td>
      <td>
      	<p>Serial (Half Duplexed):</p>
        <ul>
          <li>One Stop Bit</li>
          <li>No Parity</li>
          <li>Inverted</li>
          <li>57600 Baud</li>
          <li><a href="https://github.com/osudrl/TeleJoy/blob/552806b4f3a114bf1baaf2a7d394ab663f4caab5/sport-half-duplex/sport-half-duplex.ino#L57">Declaration</a></li>
        </ul>
      </td>
      <td>
        <p>Request Packet: </p>
        <ul>
          <li>One Header Byte (0x7E)</li>
          <li>One "Sensor" Byte</li>
        </ul>
      </td>
      <td>
      	<p>sport-half-duplex.ino</p>
        <ul>
          <li>telemetry()</li>
        </ul>
      </td>
    </tr>
    <tr>
      <td>3</td>
      <td>S. PORT (OUT)</td>
      <td>Same as above</td>
      <td>Same as above</td>
      <td>
        <p>Reply Packet: </p>
        <ul>
          <li>One Header Byte (0x10)</li>
          <li>Two Value Id Bytes</li>
          <li>Four T. Data Bytes</li>
          <li>0x7D and 0x7E must be escaped</li>
        </ul>
      </td>
      <td>
      	<p>sport-half-duplex.ino</p>
        <ul>
          <li>telemetry()</li>
          <li>send_data()</li>
        </ul>
      </td>
    </tr>
    <tr>
      <td>4</td>
      <td>Serial (HEX)</td>
      <td>PC Serial Port --> Teensy MicroUSB</td>
      <td>
      	<p>Default Serial:</p>
        <ul>
          <li>One Stop Bit</li>
          <li>No Parity</li>
          <li>Not Inverted</li>
          <li>12M Baud</li>
        </ul>
      </td>
      <td>
        <p>Packet: </p>
        <ul>
          <li>Header Byte (0xFB)</li>
          <li>Value ID Byte (DEC 0-18)</li>
          <li>Value Byte (DEC 0-255) </li>
        </ul>
      </td>
      <td>
      	<p>sport-half-duplex.ino</p>
        <ul>
          <li>tryUsbInput()</li>
        </ul>
      </td>
    </tr>
  </tbody>
</table>


# Setup Guide

To get this project running, make sure the hardare is wired as shown above and that the each of the following sections of the guide below are followed.

1. Teensyduino setup (Teensy for the Arduino IDE)
2. Patching teensy cores to allow for more axes to be sent to the operating system
3. Uploading the source to the teensy
4. Binding with the TARANIS plus
5. Testing the teensy's output

## Teensy installation for Arduino IDE

Install the [Arduino IDE](https://www.arduino.cc/en/Main/Software) to somewere in your home folder.  Next, download and install the Teensyduino add-on from the [PJRC download page](https://www.pjrc.com/teensy/td_download.html) and select the folder where the Arduino IDE was installed.  Install everything that Teensyduino has to offer (default).

## Allow for "Extreme Joystick"

Navigate to the Arduino/Teensyduino installation, and open `hardware/teensy/avr/cores/teensy3/usb_desc.h` in a text editor.  The line numbers below might be different with newer versions of Teensyduino, but find the `JOYSTICK_SIZE` definition under the `USB_SERIAL_HID` line and change the size from 12 to 64.

```c
#ifndef _usb_desc_h_
#define _usb_desc_h_

// Scroll to line 215

#elif defined(USB_SERIAL_HID)

// Scroll to line 252

#define JOYSTICK_ENDPOINT     6
#define JOYSTICK_SIZE         64	//  12 = normal, 64 = extreme joystick
#define JOYSTICK_INTERVAL     1

```

Once extreme joystick is set by `#define JOYSTICK_SIZE 64`, the teensy should be able relay enough joystick axes to the operating system.

## Uploading to the Teensy

To upload the telejoy sketch (`thisrepository/teensysrc/telejoy/telejoy.ino`) to the Teensy, reference the [PJRC page on Teensyduino usage](https://www.pjrc.com/teensy/td_usage.html) to complete the following:

1. Connect the Teensy board to the computer via micro-usb
2. Open the telejoy.ino sketch in the Arduino IDE
3. Select `Teensy 3.2 / 3.1` from `Tools -> Boards`
4. Select `Serial + Keyboard + Mouse + Joystick` from `Tools -> USB Type`
  * The sketch may need to be uploaded to the board first before this option becomes availible
5. Upload the sketch to the Teensy as explained on the PJRC page linked above
6. Ensure that `Serial + Keyboard + Mouse + Joystick` is still selected from `Tools -> USB Type`

That's it! The telejoy code should now be loaded onto the Teensy.

## Binding with the TARANIS plus

The TARANIS plus controller:

<img src="http://cdn.shopify.com/s/files/1/0412/2761/products/taranis-x9d-plus3_grande.jpg?v=1468705173" width="400"> 

When the reciever is searching for a controller to bind to, the **red LED will flash about once a second**.  If the red LED is solid, it may need to be put into binding mode by holding down the button on the bottom right of the chip while simultaneously plugging in the power for the device, and then releasing the button.

While properly bound to a controller and recieving data, the reciever will keep the green LED lit:

<img src="http://i.imgur.com/f1CMw7O.jpg?1" width="600">

If there are issues getting the reciever properly bound to the radio controller, the following steps, as detailed in this [video tutorial](https://www.youtube.com/watch?v=1IYg5mQdLVI) may fix the issue: 
1. Create/duplicate a new model in the TARANIS `MENU` 
2. In that new model's `PAGE`, towards the bottom, set the `Channel Range` to `Ch1-16` 
3. A different `Recivever No.` may be selected (currently `1` is selected) 
4. Press `ENT` while `BIND` is highlighted.  Try binding while the reciever is on, off, being turned on, and being turned off until the green LED lights and stays lit as shown in the above example image.

## Testing the virtual joystick

### jstest

The easiest way to test the output of the Teensy as a joystick input device is with the application jstest-gtk, availible via aptitude.

```shell
sudo apt install jstest-gtk
# Check that the Teensy is connected and that the TARANIS is properly bound.
jstest-gtk
```
If no joysticks show up, check that the right usb type was selected when uploading to the board.

If more than one joysticks show up in the jstest application, only one will work.  In my tests, js1 was the proper board.

### SDL2 in C

First, install SDL2.

```shell
sudo apt install libsdl2-dev
```
Next, compile and run sdl-example.c

```shell
gcc sdl-example.c -lSDL2
./a.out
```
Note that as of now, the only way to exit the sdl-example application is to press `Ctrl+\`

# Other Information

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

## Feedback

Written by [Kevin Kellar](https://github.com/kkevlar) for Oregon State University's [Dynamic Robotics Laboratory](http://mime.oregonstate.edu/research/drl/).  For issues, comments, or suggestions with this guide, contact the developer or open an issue.
