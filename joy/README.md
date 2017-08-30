# Joy Sketch Documentation

![joy sketch flow](http://i.imgur.com/4GLNSDM.png)

The Joy sketch is intended to be uploaded to a Teensy 3.2 to perform the following tasks.

1. Recieve information about the current state of the radio controller (axes, buttons, etc.) and relay that information as a virtual joystick device to be interfaced with using libraries such as SDL.
2. Send telemetery values to display on the TARANIS controller based on the information read over USB Serial.

Functions/variables dealing with the first task often have the `sbus_` prefix, while ones implementing the second task often have the `tele_` or `sport_` prefix.

## Initial Constants ([lines 4-13](https://github.com/osudrl/TeleJoy/blob/d402bd79707f8b4f5290dc375cd43663402ff82e/joy/joy.ino#L4-L13))

Name | Value | Purpose
--- | --- | ---
SPORT_ REQUEST_ HEADER | 0x7e | The header byte that validates a proper request packet for the [SPORT protocol](https://github.com/osudrl/TeleJoy#sport-23).
SPORT_ ONLY_ SENSOR_ ID | 0x22 | The only sensor id that the telemetry (sport) code sends a reply to with this implementation of the SPORT protocol.  Can be any of the sensorids that the reciever polls, but replying to all the sensors will cause the reciever to not respond.
JOY_ MIN | 3950 | Approximately the value that corresponds to the most negative input that the operating system supports for analog joystick axes
JOY_ MAX | 62000 | See above and the [mapAnalog() function](https://github.com/osudrl/TeleJoy/blob/d402bd79707f8b4f5290dc375cd43663402ff82e/joy/joy.ino#L400-L408).
IN_ MIN | -820 | The minimum analog joystick supplied by the input from the sbus line from the radio reciever.
IN_ MAX | 819 | See above.
DEADZONE_ MITIGATION_ CONSTANT | 3800 | The operating system interprets joysticks as having a pretty large deadzone around 0.  Approximately, all values between -3800 to 3800 will be interpreted as exactly zero by the os. This value can be tuned based on experimentation.
TELE_ ALLOWED_ IDLE_ TIME | 5000 | The amount of time that the joy program will wait before re-sending a telemetry value that hasn't changed to the reciever.  See the sport_telemetry() function and the tele_msUpdated[ ] array for more information. TODO add links.

## Telemetry Arrays

Name | Purpose
--- | ---
tele_ids | Contains the fourteen different data ids that the TARANIS will display.  For instance, with the current setup, ids 1-4 refer to the error codes that are displayed on page1, while the rest of the array is the ids of the 9 different channels displayed on the following telemetry page.
tele_data | Contains the values that corrospond to ids contained in `tele_ids` at the same index
tele_msUpdated | Holds the number of milliseconds that had elapsed when the value at that index was most recently updated.  Used to ensure that a value doesn't go un-updated for longer than `TELE_ALLOWED_IDLE_TIME`.

## Telemetry (SPORT) Functions

### sport_CRC()

Calculates the proper checksum byte given the beginning of the reply packet to be sent at the end of the reply packet.

### sport_flushInputBuffer()

Reads and throws out any bytes that haven't been read from the buffer from the half-duplexed telemetry line.  Is called before switching between RX and TX mode on the [half-duplexed line](https://github.com/osudrl/TeleJoy/#on-half-duplexing).

### sport_setRX()

Puts Serial3 (Pin8) in RX mode despite that Pin8 is the TX pin of the Serial3 UART.  This is done through changing individual bit registries on the Teensy.  See the section in the README [on half duplexing](https://github.com/osudrl/TeleJoy/#on-half-duplexing) for more information.  

### sport_hdInit()

Sets up the Serial3 UART for inverted serial at 57600 baud in half-duplex mode.
See the section in the README [on half duplexing](https://github.com/osudrl/TeleJoy/#on-half-duplexing) for more information.  

### sport_setTX()

Puts the Serial3 UART in TX mode by changing the registries.
See the section in the README [on half duplexing](https://github.com/osudrl/TeleJoy/#on-half-duplexing) for more information.  

### sport_sendData()

Sends a reply packet to update a value id with new data.  This function is called by `sport_telemetry()`.

#### Arguments

Num | Name | Usage
--- | --- | ---
0 | id | The value id being updated.  See [tele_ids](https://github.com/osudrl/TeleJoy/tree/master/joy#telemetry-arrays) for the valid value ids that may be used for the current setup
1 | val | The value that will display on the TARANIS given the current id.  Although the fuction takes a uint32_t and four value bytes are sent over the half-duplexed line as per the SPORT protocol, testing has only gotten a signed 16 bit integer to work, with the last two bytes that are sent on the half-duplexed line being zeros. 

#### Function procedures 
1. Create a sport_reply_packet type and fill it with the information from the arguments
2. Set the half-duplexed line to TX mode.
3. Generate the checksum byte based on the current reply paket
4. Iterate over all the bytes in the packet and add the byte to the out buffer.  If the byte needs to be escaped according to the SPORT protocol, add the proper bytes to the out buffer.
5. Write the out buffer to the Serial3 UART
6. Put the half-duplexed line (Serial3) into RX mode.

> See the [SPORT protocol section](https://github.com/osudrl/TeleJoy/#sport-23) in the TeleJoy README for more information.

### Setting Telemetry Values (Protocol 4)

#### Overview

The two functions that deal with updating the telemetry values are `usb_addSafeByte()` and `sport_tryUsbInput()`.

![prot4 bytes](http://i.imgur.com/KtNfuiW.png)

The above image shows:

1. How the intended telemetry data is filled into an array
2. This data is converted a longer array of single bytes
3. Those bytes are sent over USB serial
4. tryUsbInput recieves those bytes and rebuilds the original desired telemetry values

#### tryUsbInput()

The job of `tryUsbInput()` is to sort out telemtry data from headers and escape bytes.  It has the following flow:

<img src="http://i.imgur.com/ugFIKXG.png" width="600">

`usb_addSafeByte()` is called by `tryUsbInput()` to process a byte which is guaranteed to be the actual data (**blue X** in the diagram above) that was intended to be sent over USB Serial.

>If no escaping/headers was required for updating packets of telemetry data, then each individual byte of data could be passed to `usb_addSafeByte()`.  

#### addSafeByte()

As per the USB Serial updating protcol (number 4 in the main README), the least significant byte is sent first, followed by the most significant byte.
`usb_addSafeByte()` has three different operations based on the current state.

Name | Conditions | Actions
--- | --- | ---
Bail | Current index (**red #s** above) is invalid | Do nothing with the byte
Set LSB | Current index is valid **and** LSB **not** set | Save the lsb for later and remember that LSB has been set
Set MSB | Current index valid **and** LSB **is** set | Construct a `int16_t` from the two bytes, increment index

The following flowchart illustrates the information in the above table:

<img src="http://i.imgur.com/N8mYxGk.png" width="600">

Unless is valid header is seen in tryUsbInput(), the currentIndex will be -1, which is invalid, and all the bytes will get ignored.

Most importantly, `addSafeByte()` is the only method which directly updates the `tele_data[]` array as shown in the [joy master flowchart](http://i.imgur.com/4GLNSDM.png). 

### sport_telemetry()

Sport telemetry reads and recieves the request packets over serial from the radio reciever.  It checks to make sure that these packets are valid, and then, if valid, can decide to send a response packet which will update the telemetry value for a specific value id.

<img src="http://i.imgur.com/EhLFhsw.png" width="600">

The table in the above diagram checks that the sensor id is 0x22 because that is the [currently (when this documentation was written)](https://github.com/osudrl/TeleJoy/blob/a3defc924a38f9ea9222f368a4a72c7ff03cf73e/joy/joy.ino#L7) single sensor id that the code tests for. 

### sport_setup()

This function is called by the arduino function `setup()`.  It sets up the "sport" side of the code.  Most notably, it calls `sport_hdInit()` which sets up the Serial3 UART to be half-duplexed.

### sport_loop()

Calls tryUsbInput() and sport_telemetry() if applicable so they can be constantly testing their inputs.  

It also turns off the onboard LED on the Teensy every 10 seconds.
In `sport_telemetry()`, the led is turned back on, so if `sport_telemetry()` is never called because no more requests are coming in on the Serial3 UART, then the onboard LED will stay off.
A past, bugged version of the joy sketch would not properly follow the expected SPORT protocol with the radio reciever, and the radio reciever would stop sending request packets- ending all telemetry data exchange.
If this happens, there will be an indication by the onbard LED not lighting.

## Controller / SBUS Functions



### sbus_decode_packet() 

### sendJoyOutput()

### mapAnalog()

Source | Left Range | Right Range
---|---|---
.analog[ ] from sbus_data_t | -820 to 0 | 0 to 820
Sent to USB Joystick (`Joystick.`) | \~3950 to 32975\* | 32975\* to ~62000
Recieved in SDL `SDL_JoystickGetAxis()` | -32768 to 0 | 0 to 32768

The approximate values above are determined from experimenting with the joy code and interfacing it with the jstest program.
Their values are set in the constants at the top of joy.ino but can be tuned or changed as more testing is done and if any issues are found with the range or responsiveness.

Passing an axis value of 3950 to `Joystick.slider()` or `Joystick.X()` results in the leftmost value (-32768) observed in SDL, and passing a value of 62000 to the Joystick interface on the Teensy will result in the rightmost value (32768) read in SDL. 

\*When testing, the operating system was creating an artificial deadzone around the neutral position **(0 in SDL)**.
When passing any value within **3800 of 32975**, the mean ((62000-3950) /2 =**32975**), jstest would show the axis at 0.
For all values from **29175 to 36775** that were passed to the Teensy Joystick interface, the result in jstest would be **0**.
To counteract this software-enforced deadzone, no values from **29175 to 36775** are sent to the Joystick interface, and instead the **left** (negative) values are mapped from  **[-820 to 0]** to **[3950 to 29175]** and the **right** (positive) values are mapped from **[0 to 820]** to **[36775 to 62000]**.
The mapping and transformation of the analog axis data is illustrated in the below diagram and is able to be tweaked (or turned off) by modifying the `DEADZONE_MITIGATION_CONSTANT` defined at the top of the joy sketch.

![axes transformations](http://i.imgur.com/MrhnZZ2.png)

### sbus_loop()

### sbus_setup()




