# TARANIS Telemetry to SDL Joystick

This teensy (Arduino) sketch will read serial data from the XSR (X4RS_Rev1.1) telemetry reciever and decode the information into controller axes and buttons.  This controller data can then be used to interact with the operating system as a Joystick device.

The Teensy 3.2 (left) connected to the XSR reciever (right):

<img src="http://i.imgur.com/vlCQ2Rf.jpg?1" width="600"> 

Note that in the above image, the yellow S.PORT line isn't connected to the Teensy.

This project was written and uploaded in the Arduino IDE on Ubuntu Gnome 16.04.

See the [Setup Guide](https://github.com/osudrl/TeleJoy#setup-guide) for information about how to get the program working as intended.

Also see the [documentation written](https://github.com/osudrl/TeleJoy/blob/master/joy/README.md) specifically for the [joy sketch](https://github.com/osudrl/TeleJoy/tree/master/joy) which goes into depth about how the code works if modifications are needed or a similar project is being developed.

## In this Respository

* [joy](https://github.com/osudrl/TeleJoy/tree/master/joy): The "main" program that most of this repository's documentation references.  Meant to uploaded to the Teensy 3.2 to act as a Joystick HID based on the input from the radio controller.
* [ExtrmeJoystickTest](https://github.com/osudrl/TeleJoy/tree/master/ExtremeJoystickTest): Use this sketch to test if the Teensy has been properly configured as a "big" joystick.
* [sdl-test](https://github.com/osudrl/TeleJoy/tree/master/sdl-test): A simple C program to show that SDL can "see" and interface with the Teensy as a joystick.
* [serial](https://github.com/osudrl/TeleJoy/tree/master/serial): A simple C program to send telemetry values to the controller.  Use as an example of how to change the Telemetry values over USB Serial.
* [rulesetup](https://github.com/osudrl/TeleJoy/blob/master/rulesetup.sh): A simple shell script to update the linux udev rules to properly upload to the Teensy with Teensyduino.

# Setup Guide

**If all of the hardware that was used in development has remained unchaned and the Teensy's program has not yet been overwritten, skip to the [testing section](https://github.com/osudrl/TeleJoy#testing-the-virtual-joystick) or, more speficially, the [writing custom software to interface with the Teensy section](https://github.com/osudrl/TeleJoy#writing-a-serialsdl-program-to-use-with-the-teensy).**

This Setup Guide has the following sections.

1. Teensyduino setup (Teensy for the Arduino IDE)
2. Patching teensy cores to allow for more axes to be sent to the operating system
3. Uploading the source to the teensy
4. Binding with the TARANIS plus
5. Testing the teensy's output

## Teensy installation for Arduino IDE

Install the [Arduino IDE](https://www.arduino.cc/en/Main/Software) to somewere in your home folder.  Next, download and install the Teensyduino add-on from the [PJRC download page](https://www.pjrc.com/teensy/td_download.html) and select the folder where the Arduino IDE was installed.  Install everything that Teensyduino has to offer (default).

### Udev Rules

On Linux, follow the instructions in Step2 on the [download page](https://www.pjrc.com/teensy/td_download.html) to setup proper udev rules for the Teensy.

Alternatively, run `bash rulesetup.sh` in the repository directory to have a shell script do the process described in the PJRC page's "Step2".

If Teensyduino is having problems properly uploading to the Teensy, your linux user may need to be added to the dialout group as [described here](https://askubuntu.com/questions/58119/changing-permissions-on-serial-port).

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

To upload the joy sketch (`TeleJoy/joy/joy.ino`) to the Teensy, reference the [PJRC page on Teensyduino usage](https://www.pjrc.com/teensy/td_usage.html) to complete the following steps:

1. Connect the Teensy board to the computer via micro-usb
2. Open the telejoy.ino sketch in the Arduino IDE
3. Select `Teensy 3.2 / 3.1` from `Tools -> Boards`
4. Select `Serial + Keyboard + Mouse + Joystick` from `Tools -> USB Type`
  * The sketch may need to be uploaded to the board first before this option becomes availible
5. Upload the sketch to the Teensy as explained on the PJRC page linked above
6. Select `Serial + Keyboard + Mouse + Joystick` from `Tools -> USB Type`

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

### Running the SDL Example

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

## Writing a Serial+SDL Program to use with the Teensy

### Serial Test Example

A program to interface with the joy sketch on the Teensy needs to interpret Joystick data and also send updates to the telemetry data over Serial.
The [serial-test.c](https://github.com/osudrl/TeleJoy/blob/master/serial/serial-test.c) program with only `SERIAL_TEST_SDLSTATES` [defined](https://github.com/osudrl/TeleJoy/blob/9e670139f6f5e3446ecd3cccc188370dc8d6feb7/serial/serial-test.c#L13-L16).

```c
// serial-test.c
// ...

// #define SERIAL_TEST_COUNTING
// #define SERIAL_TEST_ESCAPING
// #define SERIAL_TEST_SQAURES
#define SERIAL_TEST_SDLSTATES

// ...
```
Run the serial-test like this:

```shell
cd TeleJoy/serial
bash compile.sh
./a.out
# To Exit, press Ctrl+\
```

Running this program while the Teensy is connected and bound to the radio controller should show a screen where UCH1-9 show 1.

<img src="http://i.imgur.com/DTwMNEb.jpg" width="400">

As the different switches and sticks are moved, the telemetry data values should increase from 1 to 3 or 5 (depending on if it is a stick or a switch). 

<img src="http://i.imgur.com/9T39XJB.jpg" width="400">

Even if the serial-test project doesn't run properly, the rest of this section will detail how the serial-test project was developed and similar project can be developed and adapted as needed.

### Program to read Joystick Data

The easiest way to interface with a USB Joystick in C is to use the SDL2 library.

```shell
# Install SDL2
sudo apt install libsdl2-dev
```

```c
// Inclue in the C program
#include <SDL2/SDL.h>
```

```shell
# Linking with SDL2 library at runtime
gcc mytest.c -lSDL2
```

To get the Teensy Joystick as an SDL Joystick:

```c
#include <SDL2/SDL.h>
#include <string.h>

SDL_Joystick* get_joystick()
{

    const int n = SDL_NumJoysticks();
    char name[] = "Teensyduino Serial/Keyboard/Mouse/Joystick";
    for (int i = 0; i < n; ++i) {
        if (strncmp(SDL_JoystickNameForIndex(i), name, sizeof name) == 0)
            return SDL_JoystickOpen(i);
    }
    return NULL;
}
```

And to read the Joystick axes:

```c
int main()
{
	SDL_Init(SDL_INIT_JOYSTICK);
	SDL_Joystick* ctrl = get_joystick();
	int analog[16];
	while (ctrl) 
	{
	    SDL_JoystickUpdate();
	    for (int i = 0; i < 16; i++) 
	        analog[i] = SDL_JoystickGetAxis(ctrl, i);
	}
	SDL_Quit();
}
```

This code should properly read Joystick data using SDL2.

### Setting Telemtry Data over USB

To send data to the Teensy over USB, first find what port the Teensy is on according to the operating system.
Open the Arduino IDE and select the Teensy device from `Tools -> Port`.  Make note of the port (in this case it was `/dev/ACM0`).

<img src="http://i.imgur.com/I01n4ix.png" width="500">

Next, test that serial communication is working by opening the ArduinoIDE's serial monitor and typing some letters into the input field and pressing enter.
The output on the serial monitor should complain that it is "BAILING" on a whole bunch of bytes.

<img src="http://i.imgur.com/TEU8NMT.png" width="400">

Next, write a simple C program to communicate with the Teensy.

# Six Serial Protocols

> When [links to code snippets](https://github.com/osudrl/TeleJoy/blob/552806b4f3a114bf1baaf2a7d394ab663f4caab5/telejoy/telejoy.ino#L60) from this project's source are included in this secion, they link to **outdated snapshots of the source code**.  Do not copy/paste source code from these linked files or try to use the code that is not highlited in yellow by the snippet link.  The **highlighted code provides an example or context** for some feature that is explained in the documentation.  For the most up-to-date version of the code to work with, see [the master branch](https://github.com/osudrl/TeleJoy/tree/master/).

In case you aren't familiar with serial communication, see [SparkFun's Guide](https://learn.sparkfun.com/tutorials/serial-communication).

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
			<td><a href="https://github.com/osudrl/TeleJoy#setting-the-telemetry-data-4">TELE-BUS (HEX)</a></td>    
			<td>Set the numbers that will be sent to the TARANIS screen</td>
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

The Less Friendly Table:

<table>
	<tbody>
		<tr>
			<th>#</th>
			<th>Name</th>
			<th>Connections</th>
			<th>Rules</th>
			<th>Flow</th>
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
					<li><strike>Four*</strike> <a href="https://github.com/osudrl/TeleJoy#sport-data-format">Two*</a> Data Bytes</li>
					<li>One Checksum Byte</li>
					<li>*0x7D and 0x7E must be escaped</li>
				</ul>
			</td>
		</tr>
		<tr>
			<td>4</td>
			<td><a href="https://github.com/osudrl/TeleJoy#setting-the-telemetry-data-4">TELE-BUS (HEX)</a></td>   
			<td>PC USB --> Teensy MicroUSB</td>
			<td>USB Serial</td>
			<td>
				<ul>
					<li>Two-byte header: (0xFE88)</li>
					<li>14 int16s conerted to ~30 escaped bytes</li>
				</ul>
			</td>
		</tr>
		<tr>
			<td>5</td>
			<td><a href="https://github.com/osudrl/TeleJoy#teensy-as-joystick-5">Joystick Axes</a></td>   
			<td>Teensy MicroUSB --> PC USB</td>
			<td>SDL Joystick Input</td>
			<td>Sends Teensy Joystick data based on values in sbus_data_t struct</td>
		</tr>
		<tr>
			<td>6</td>
			<td><a href="https://github.com/osudrl/TeleJoy#serial-debug-information-6">Serial (ASCII)</a></td>
			<td>Teensy MicroUSB --> PC USB</td>
			<td>Bytes are ASCII Codes</td>
			<td>Debug information from the Teensy is sent as ASCII test via USB Serial</td>
		</tr>
	</tbody>
</table>

## SBUS (1)

Be sure to see the above table (under "Rules") for the serial standard that SBUS uses.

First a [25 byte buffer](https://github.com/osudrl/TeleJoy/blob/85d4b24102093b3bf0f52b54fe9007617539fbff/joy/joy.ino#L389-L411) is read from Pin0.

Once the 25 byte buffer is decoded by [sbus_decode_packet()](https://github.com/osudrl/TeleJoy/blob/85d4b24102093b3bf0f52b54fe9007617539fbff/joy/joy.ino#L310-L352) according to [this protocol](https://developer.mbed.org/users/Digixx/notebook/futaba-s-bus-controlled-by-mbed/), the result is a populated [sbus_data_t struct](https://github.com/osudrl/TeleJoy/blob/85d4b24102093b3bf0f52b54fe9007617539fbff/joy/joy.ino#L294-L302). 

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

## S.PORT (2/3)

### On Half-Duplexing

The SPORT protocol uses a single line which is half duplexed, meaning that both devices use the line to both recieve and transmit.  Both devices default to "listening" so that when a message does come in, it can be properly recieved.

To achive this, the Teensy's Serial3 [UART](https://learn.sparkfun.com/tutorials/serial-communication) is set up to work with this half-duplexed communication.  The single communication line is soldered to the Teensy's pin8, which is the TX pin of the Serial3 UART.  There is some register bit-shifting that allows the Teensy to default to "listening" on the pin, switch into TX mode to write data, and switch back to RX mode.  See the [hdInit()](https://github.com/osudrl/TeleJoy/blob/0a33e0476821aa8a80c84cd690e4cee085026572/joy/joy.ino#L90-L95) [setRX()](https://github.com/osudrl/TeleJoy/blob/0a33e0476821aa8a80c84cd690e4cee085026572/joy/joy.ino#L83-L88) and [setTX()](https://github.com/osudrl/TeleJoy/blob/0a33e0476821aa8a80c84cd690e4cee085026572/joy/joy.ino#L97-L100) from the joy.ino sketch.  The half-duplex bit-shifting code came from [KurtE's post on PJRC forums](https://forum.pjrc.com/threads/29619-Teensy-3-1-Serial-Half-Duplex) where he links his [BioloidSerial repository](https://github.com/KurtE/BioloidSerial) that houses code to set up a half duplexed wire in the [ax12Serial](https://github.com/KurtE/BioloidSerial/blob/ebd235d3ee11d6df56a04cd9f8c8448df6b0c597/ax12Serial.cpp#L33-L231) module.

### S.PORT protocol flow

The flow for the communication on the SPORT line between the reciever and the Teensy:

1. The reciever sends a 2-byte request packet
2. The teensy checks that the request packet has a valid header
3. If valid, the Teensy determines if the sensor (as supplied in the second byte) is availible
4. The teensy forms and sends a response packet that updates one of the telemetry values with a new value

Notes:
* The code that implements the above flow is in the sport_telemetry() function im the [joy.ino sketch](https://github.com/osudrl/TeleJoy/blob/master/joy/joy.ino).
* It is best to only send a response packet if the value has changed or it has been awhile
* It is best to ignore most of the request packets from the reciever
* It may be best to decide to reply to only one sensor id, and reply with all of the telemetry values

To illustrate the flow described above, a logic analyzer has been set up as shown below to generate screenshots of what is happening on the S.PORT line as the two devices communicate.

<img src="http://i.imgur.com/q0sjXb4.jpg" width="400"> 

The analyzer probe is clipped to the yellow S.PORT wire.

<img src="http://i.imgur.com/fx9B7tU.jpg" width="400"> 

The following screenshots are snippets from the above logic analyzer setup reading the voltage on the SPORT line during **the execution of a past version of the program**.
In the current implementation of joy, the only sensor which the Teensy responds to **is [0x22](https://github.com/osudrl/TeleJoy/blob/1751baa7e642dfd4466d4a23cb8cfadb82bcdf71/joy/joy.ino#L7) and not [0x83](https://github.com/osudrl/TeleJoy/blob/61096cde4488af96ef5abe7e2536eb1a9d7395c9/sport-half-duplex/sport-half-duplex.ino#L182)**.

Note that voltage is pulled low when both lines are listening or for the stop bits as per the inverted serial protocol.  Note that they aren't necessesarily in any order.

#### 0x22 ignored

![0x22 ignored](http://i.imgur.com/uUY5qGx.png)

A request packet where the XSR reciever requests data from sensor 0x22 by first sending a valid header (0x7E) and then the sensor id in question (0x22).  The ~current code~ *past code* is programmed to [only respond to sensor (0x83)](https://github.com/osudrl/TeleJoy/blob/61096cde4488af96ef5abe7e2536eb1a9d7395c9/sport-half-duplex/sport-half-duplex.ino#L182) with data.

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

##### SPORT data format

>Although, as shown above, four data bytes are transmitted, it seems that the controller only displays the two least significant bytes as a signed 16 bit integer.  The two trailing bytes should be 0x00 and 0x00.

#### Teensy ignoring an 0x83 poll

![test3](http://i.imgur.com/ORMPBTY.png)

As shown above, the XSR polled sensor 0x83 with a properly formed request packet, ~it is best to only send a reply packet if a value has changed recently and needs to be updated or it has been awhile since a value has been updated (>10 seconds).~ **See the [flowchart](https://camo.githubusercontent.com/e77e158237bc03e69b47e3245b3309d69bb12aa0/687474703a2f2f692e696d6775722e636f6d2f45684c466873772e706e67) in `sport_telemetry()` [documentation](https://github.com/osudrl/TeleJoy/tree/master/joy#sport_telemetry) for an explaination of the decisionmaking that the joy sketch on whether or not to send the response packet.**

If an id is "updated" with the same data repetedly, the XSR firmware assumes that the value is outdated and it starts flashing on the TARANIS screen.

#### Teensy responding again 

![test4](http://i.imgur.com/fCrMjeW.png)

This time, the Teensy replies to the sensor id `0x83` with the id of `0x000D=13` and value of `0x00000007=7`.

#### 0xE4 ignored 

![test5](http://i.imgur.com/8WBMcs3.png)

For this ~test program~ *past implementation*, all sensor ids ~are~ *were* ignored except for `0x83`.

### Helpful links on the SPORT Protocol
* [Frsky Sp Repo](https://github.com/jcheger/arduino-frskysp)
* [Documentation for above repo](https://www.ordinoscope.net/static/frsky-arduino/FrskySP/doc/html/index.html)
* [Scroll to the **very bottom**, lists polled sensor ids in request packet](https://trello-attachments.s3.amazonaws.com/5629385076f33320a6f253ab/56707387a82127aa89feb540/b4e91984cfa6e15dbc5a349d540387be/sport-protocol.htm)

## Setting the Telemetry Data (4)

There are fourteen (however editable in [joy/jt-constants.h](https://github.com/osudrl/TeleJoy/blob/master/joy/jt-constants.h)) telemetry vales that can be updated as frequently or as infrequentlys as necessesary. 

To maximize efficiency, all fourteen values are sent in a big packet all at once.

A valid packet needs to begin with the [header byte](https://github.com/osudrl/TeleJoy/blob/5fb77209bf02b2036a5e0dda2002f5201d3f5446/joy/jt-constants.h#L6) `0x88`, but it needs to have the [escape byte](https://github.com/osudrl/TeleJoy/blob/5fb77209bf02b2036a5e0dda2002f5201d3f5446/joy/jt-constants.h#L5) `0xFE` preceeding it to differentiate it from the number 136.  With this protocol, to send a literal `0xFE (254)`, two `0xFE`'s should be send one right after another which will by interpreted by the joy sketch as `254`.  

Note that the two-byte values are built up from the two bytes that are sent from least significant to most significant.

Documentation on how specficially the joy sketch decodes this packet can be found on the **[joy documentation page](https://github.com/osudrl/TeleJoy/tree/master/joy#setting-telemetry-values-protocol-4)**.

### Examples

#### Squares

For example, say the program is trying to send the square of each index of the array as the telemetry data.

![indecies](http://i.imgur.com/WtIWs2G.png)

Now square each index:

![squares](http://i.imgur.com/I6gtrEE.png)

Now add the two initial header bytes and convert each square to hex, with the least sigificant bytes preceeding the most significant:

![hex](http://i.imgur.com/qw8ExIl.png)

If that string of bytes is sent over USB Serial like [this](https://github.com/osudrl/TeleJoy/blob/1713f972fc0746f4776ab2ba474475c2d345f146/serial/others/send-sqares.c),

```c
int main()
{
  FILE* output = fopen("/dev/ttyACM1", "w");

  //header bytes
  fprintf(output,"%c",0xfe);  fprintf(output,"%c",0x88);

  //the rest are numbers  
  fprintf(output,"%c",0x00);  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x01);  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x04);  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x09);  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x10);  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x19);  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x24);  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x31);  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x40);  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x51);  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x64);  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x79);  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x90);  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0xA9);  fprintf(output,"%c",0x00);

  fflush(output);
  fclose(output);
}
```

...the result will look like this on the TARANIS looks like this:

<img src="http://i.imgur.com/R0Llq89.jpg" width="600"> 

#### Using 254 as a value

If for some reason, the above example needed to be modified so that the value at index 6 was 254, the resulting hex string would look like this:

![hex 254](http://i.imgur.com/08hkw6z.png)

Notice that it takes three bytes to describe telemetry value 6, but that is because of of the `0xFE` bytes is escaping the other to produce a literal `0xFE`.

By sending that string to the Teensy with [this code](https://github.com/osudrl/TeleJoy/blob/db175aa74467dbf404a476b2932b8337b2586808/serial/others/send-escaped.c),

```c
int main()
{
  FILE* output = fopen("/dev/ttyACM1", "w");

  //header bytes
  fprintf(output,"%c",0xfe);  fprintf(output,"%c",0x88);

  //the rest are numbers  
  fprintf(output,"%c",0x00);  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x01);  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x04);  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x09);  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x10);  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x19);  fprintf(output,"%c",0x00);

  //modified value 6
  //fprintf(output,"%c",0x24);
  fprintf(output,"%c",0xFE); // escape byte
  fprintf(output,"%c",0xFE); // tells teensy the value actually is 254
  fprintf(output,"%c",0x00);

  fprintf(output,"%c",0x31);  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x40);  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x51);  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x64);  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x79);  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x90);  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0xA9);  fprintf(output,"%c",0x00);

  fflush(output);
  fclose(output);
}
```

...the result on the TARANIS should look like this:

<img src="http://i.imgur.com/Wd3Gc03.jpg" width="600">

See [serial/serial-test.c](https://github.com/osudrl/TeleJoy/blob/master/serial/serial-test.c) for a more elaborate example using this protocol and for [build escaped buffer](https://github.com/osudrl/TeleJoy/blob/28fff54aa91298a251d4542c6deb46d1ea543529/serial/serial-test.c#L36-L62) function that can be copy/pasted which takes an arrary of signed 16 bit integers and fills a buffer with the header and approximately 28 raw, excaped bytes to send over serial.

For more examples of the USB Serial protocol used to set the telemtry data in this project, see the [joy sketch doc](https://github.com/osudrl/TeleJoy/tree/master/joy#setting-telemetry-values-protocol-4).

## Teensy as Joystick (5)

If the proper USB HID is selected in the Usb Type menu in the Teensyduino software when the code is being written onto the Teensy, then the Teensy will show up as a Joystick device.  See the sdl-example.c code for an example on how to interface with the emulated Joystick device using SDL.

[PJRC Joystick info page](https://www.pjrc.com/teensy/td_joystick.html)

## Serial Debug Information (6)

Although the Teensy reads individual bytes of hex from USB serial as input to change the data that is send to the telemetry menu, the debug output over USB serial is in ASCII.  Each byte that is sent should be read as an ASCII character and can be displayed to stdout or ignored based on the needs of the client program.

# Feedback

Written by [Kevin Kellar](https://github.com/kkevlar) for Oregon State University's [Dynamic Robotics Laboratory](http://mime.oregonstate.edu/research/drl/).  For issues, comments, or suggestions with this guide, contact the developer or open an issue.
