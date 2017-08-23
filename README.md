# TARANIS Telemetry to SDL Joystick

This teensy (Arduino) sketch will read serial data from the XSR (X4RS_Rev1.1) telemetry reciever and decode the information into controller axes and buttons.  This controller data can then be used to interact with the operating system as a Joystick device.

The Teensy 3.2 (left) connected to the XSR reciever (right):

<img src="http://i.imgur.com/vlCQ2Rf.jpg?1" width="600"> 

Note that in the above image, the S.PORT line isn't connected to the Teensy, but the S.PORT (yellow) wire is connected to Pin8 of the Teensy with the current project setup.

This project was written and uploaded in the Arduino IDE on Ubuntu Gnome 16.04.

This readme file includes a [Setup Guide](https://github.com/osudrl/TeleJoy#setup-guide) down below.

# Six Protocols

> When [links to code snippets](https://github.com/osudrl/TeleJoy/blob/552806b4f3a114bf1baaf2a7d394ab663f4caab5/telejoy/telejoy.ino#L60) from this project's source are included in this secion, they link to **outdated snapshots of the source code**.  Do not copy/paste source code from these linked files or try to use the code that is not highlited in yellow by the snippet link.  The **highlighted code provides an example or context** for some feature that is explained in the documentation.  For the most up-to-date version of the code to work with, see [the master branch](https://github.com/osudrl/TeleJoy/tree/master/).

In case you aren't familiar with serial communication, see [SparkFun's Guide](https://learn.sparkfun.com/tutorials/serial-communication);

As [the image below](http://i.imgur.com/MqNwuJ3.png) illustrates, there are six different infromation exchange protocols used across the four devices in the project.

![the image below](http://i.imgur.com/MqNwuJ3.png)

The Friendly Table:

<table>
	<tbody>
		<tr>
			<th>#</th>
			<th>Name</th>
			<th>Usage</th>
		</tr>
		<tr>
			<td>1</td>
			<td><a href="https://github.com/osudrl/TeleJoy#sbus-1">SBUS</a></td>
			<td>Controller data from reciever to teensy</td>
		</tr>
		<tr>
			<td>2/3</td>
			<td><a href="https://github.com/osudrl/TeleJoy#sport-23">S.PORT</a></td>     
			<td>Request/send numbers to display on the TARANIS</td>
		</tr>
		<tr>
			<td>4</td>
			<td><a href="https://github.com/osudrl/TeleJoy#setting-the-telemetry-data-4">HEX</a></td>    
			<td>Set those numbers that will be sent to the screen</td>
		</tr>
		<tr>
			<td>5</td>
			<td><a href="https://github.com/osudrl/TeleJoy#teensy-as-joystick-5">Joystick</a></td>   
			<td>Controller data from the teensy to USB</td>
		</tr>
		<tr>
			<td>6</td>
			<td><a href="https://github.com/osudrl/TeleJoy#serial-debug-information-6">ASCII</a></td>     
			<td>Debug infromation from the teensy to USB</td>
		</tr>
	</tbody>
</table>

The Technical Table:

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
			<td><a href="https://github.com/osudrl/TeleJoy#sbus-1">SBUS</a></td>
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
			<td><a href="https://github.com/osudrl/TeleJoy#sport-23">S.PORT</a></td>   
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
			<td><a href="https://github.com/osudrl/TeleJoy#sport-23">S.PORT</a></td>   
			<td>Same as above</td>
			<td>Same as above</td>
			<td>
				<p>Reply Packet: </p>
				<ul>
					<li>One Header Byte (0x10)</li>
					<li>Two* Value Id Bytes</li>
					<li>Four* T. Data Bytes</li>
					<li>One Checksum Byte</li>
					<li>*0x7D and 0x7E must be escaped</li>
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
			<td><a href="https://github.com/osudrl/TeleJoy#setting-the-telemetry-data-4">Serial HEX</a></td>   
			<td>PC USB --> Teensy MicroUSB</td>
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
		<tr>
			<td>5</td>
			<td><a href="https://github.com/osudrl/TeleJoy#teensy-as-joystick-5">Joystick Axes</a></td>   
			<td>Teensy MicroUSB --> PC USB</td>
			<td>SDL Joystick Input</td>
			<td>Sends Teensy Joystick data based on values in sbus_data_t struct</td>
			<td>
				<p>telejoy.ino</p>
				<ul>
					<li>sendJoyOutput()</li>
				</ul>
			</td>
		</tr>
		<tr>
			<td>6</td>
			<td><a href="https://github.com/osudrl/TeleJoy#serial-debug-information-6">Serial (ASCII)</a></td>
			<td>Teensy MicroUSB --> PC USB</td>
			<td>Bytes are ASCII Codes</td>
			<td>Debug information from the Teensy is sent as ASCII test via USB Serial</td>
			<td>TODO: WIP</td>
		</tr>
	</tbody>
</table>

### SBUS (1)

Be sure to see the above table (under "Rules") for the serial standard that SBUS uses.

Once the 25 byte buffer is decoded by sbus_decode_packet() according to [this protocol](https://developer.mbed.org/users/Digixx/notebook/futaba-s-bus-controlled-by-mbed/), the result is a populated sbus_data_t struct. 

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
D0 | digital[0] | N/A | Unused | Digital
D1 | digital[1] | N/A | Unused | Digital
FL | frame_lost | N/A | `1` when controller off, otherwise `0` | Digital
FA | failsafe_active | N/A | Unknown | Digital

States Type | Numerical Outputs
-- | ---
Analog | -820 to 819
3 | -820, 0, and 819
2 | -820 and 819
Digital | 0 and 1

>\* For the A15/SH button, a value of 819 represents the "unpressed" state and -820 is the "pressed" state

### S.PORT (2/3)

The SPORT protocol uses a single line which is half duplexed, meaning that both devices use the line to both recieve and transmit.  Both devices default to "listening" so that when a message does come in, it can be properly recieved.

To achive this, the Teensy's Serial3 UART is set up to work with this half-duplexed communication.  The single communication line is soldered to the Teensy's pin8, which is the TX pin of the Serial3 UART.  There is some register bit-shifting that allows the Teensy to default to "listening" on the pin, switch into TX mode to write data, and switch back to RX mode.  See the hdInit() setRX() and setTX() that are (as of now) in the sport-half-duplex.ino sketch.  The half-duplex bit-shifting code came from [KurtE's post on PJRC forums](https://forum.pjrc.com/threads/29619-Teensy-3-1-Serial-Half-Duplex) where he links his [BioloidSerial repository](https://github.com/KurtE/BioloidSerial) that houses code to set up a half duplexed line in the ax12Serial module.

The flow for the communication on the SPORT line between the reciever and the Teensy:

1. The reciever sends a 2-byte request packet
2. The teensy checks that the request packet has a valid header
3. If valid, the Teensy determines if the sensor (as supplied in the second byte) is availible
4. The teensy forms and sends a response packet that updates one of the telemetry values with a new value

Notes:
* It is best to only send a response packet if the value has changed or it has been awhile
* It is best to ignore most of the request packets from the reciever
* It may be best to decide to reply to only one sensor id, and reply with all the values for that sensor

The following images are snippets from a logic analyser reading the voltage on the SPORT line during normal program execution.  Note that **voltage is pulled low** when both lines are listening or for the stop bits as per the inverted serial protocol.  Note that they aren't necessesarily in any order.

#### 0x22 ignored

![0x22 ignored](http://i.imgur.com/uUY5qGx.png)

A request packet where the XSR reciever requests data from sensor 0x22 by first sending a valid header (0x7E) and then the sensor id in question (0x22).  The current code is programmed to [only respond to sensor (0x83)](https://github.com/osudrl/TeleJoy/blob/61096cde4488af96ef5abe7e2536eb1a9d7395c9/sport-half-duplex/sport-half-duplex.ino#L182) with data.

#### Teensy decides to repsond

![0x83 response 1](http://i.imgur.com/ArDqLf8.png)

1. The XSR's request packet requests data from sensor 0x83.  
2. The Teensy is programmed to respond to this sensor, and sends the reply packet on the same line.  

The first two bytes were sent by the XSR Reciver, and following 8 bytes were sent on the same line by the Teensy.  

3. The Teensy forms/send the reply packet consisting of:
  * One header byte (0x10)
  * Two value id bytes sent from least to most significant.  The resulting HEX (0x000C) converts to 12 in decimal.  
  * The following four data bytes set the current value to be displayed on the controller screen for the value id (12).  0x00000008 converts to 8 in decimal. 
  * The last bit is a checksum bit that is calculated given the previous seven bytes.  
4. As shown in the [telemetry data buffer declaration](https://github.com/osudrl/TeleJoy/blob/master/sport-half-duplex/sport-half-duplex.ino#L14-L16) in the test program from where this data was captured, `telemetry_data_buffer[12] = 8`, so the exchange is working as expected.

#### Teensy ignoring an 0x83 poll

![test3](http://i.imgur.com/ORMPBTY.png)

As shown above, the XSR polled sensor 0x83 with a properly formed request packet, but through testing with the XSR Reciever, it is best to only send a reply packet if a value has changed recently and needs to be updated or it has been awhile since a value has been updated (>10 seconds).

If every value is constantly updated, the XSR firmware assumes that the value is outdated and it starts flashing on the TARANIS screen.

#### Teensy responding again 

![test4](http://i.imgur.com/fCrMjeW.png)

This time, the Teensy replies to the sensor id 0x83 with the id of `0x000D=13` and value of `0x00000007=7`.  As shown in the test program's [telemetry data buffer declaration](https://github.com/osudrl/TeleJoy/blob/master/sport-half-duplex/sport-half-duplex.ino#L14-L16), `telemetry_data_buffer[13] = 7`, so the exchange continues to work as expected.

#### 0xE4 ignored 

![test5](http://i.imgur.com/8WBMcs3.png)

For this test program, all sensor ids are ignored except for id 0x83.

#### More on the SPORT Protocol
* [Frsky Sp Repo](https://github.com/jcheger/arduino-frskysp)
* [Documentation for above repo](https://www.ordinoscope.net/static/frsky-arduino/FrskySP/doc/html/index.html)
* [Scroll to the very bottom, lists polled sensor ids in request packet](https://trello-attachments.s3.amazonaws.com/5629385076f33320a6f253ab/56707387a82127aa89feb540/b4e91984cfa6e15dbc5a349d540387be/sport-protocol.htm)

### Setting the Telemetry Data (4)

### Teensy as Joystick (5)

[PJRC Joystick info page](https://www.pjrc.com/teensy/td_joystick.html)

### Serial Debug Information (6)


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

# Feedback

Written by [Kevin Kellar](https://github.com/kkevlar) for Oregon State University's [Dynamic Robotics Laboratory](http://mime.oregonstate.edu/research/drl/).  For issues, comments, or suggestions with this guide, contact the developer or open an issue.
