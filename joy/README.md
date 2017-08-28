# Joy Sketch Documentation

The Joy sketch is intended to be uploaded to a Teensy 3.2 to perform the following tasks.

1. Recieve information about the current state of the radio controller (axes, buttons, etc.) and relay that information as a virtual joystick device to be interfaced with using libraries such as SDL.
2. Send telemetery values to display on the TARANIS controller based on the information read over USB Serial.

Functions/variables dealing with the first task often have the `sbus_` prefix, while ones implementing the second task often have the `tele_` or `sport_` prefix.

## Initial Constants ([lines 4-13](https://github.com/osudrl/TeleJoy/blob/d402bd79707f8b4f5290dc375cd43663402ff82e/joy/joy.ino#L4-L13))

Name | Value | Purpose
--- | --- | ---
SPORT_REQUEST_HEADER | 0x7e | The header byte that validates a proper request packet for the [SPORT protocol](https://github.com/osudrl/TeleJoy#sport-23).
SPORT_ONLY_SENSOR_ID | 0x22 | The only sensor id that the telemetry (sport) code sends a reply to with this implementation of the SPORT protocol.  Can be any of the sensorids that the reciever polls, but replying to all the sensors will cause the reciever to not respond.
JOY_MIN | 3950 | Approximately the value that corresponds to the most negative input that the operating system supports for analog joystick axes
JOY_MAX | 62000 | See above and the [mapAnalog() function](https://github.com/osudrl/TeleJoy/blob/d402bd79707f8b4f5290dc375cd43663402ff82e/joy/joy.ino#L400-L408).
IN_MIN | -820 | The minimum analog joystick supplied by the input from the sbus line from the radio reciever.
IN_MAX | 819 | See above.
DEADZONE_MITIGATION_CONSTANT | 3800 | The operating system interprets joysticks as having a pretty large deadzone around 0.  Approximately, all values between -3800 to 3800 will be interpreted as exactly zero by the os. This value can be tuned based on experimentation.
TELE_ALLOWED_IDLE_TIME | 5000 | The amount of time that the joy program will wait before re-sending a telemetry value that hasn't changed to the reciever.  See the sport_telemetry() function and the tele_msUpdated[] array for more information.


