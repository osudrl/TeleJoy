# Joy Sketch Documentation

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

### sport_setRX()

### sport_hdInit()

### sport_setTX()

### sport_sendData()

### usb_addSafeByte()

### sport_tryUsbInput()

### sport_telemetry()

### sport_setup()

### sport_loop()

## Controller / SBUS Functions

### sbus_decode_packet() 

### sendJoyOutput()

### mapAnalog()

### sbus_loop()

### sbus_setup()




