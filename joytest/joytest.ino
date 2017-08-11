/* Basic USB Joystick Example
   Teensy becomes a USB joystick

   You must select Joystick from the "Tools > USB Type" menu

   Pushbuttons should be connected to digital pins 0 and 1.
   Wire each button between the digital pin and ground.
   Potentiometers should be connected to analog inputs 0 to 1.

   This example code is in the public domain.
*/

void setup() {

}

void loop() {

  // read the digital inputs and set the buttons
  Joystick.button(1, (millis()/2000) %2 == 0);
  Joystick.button(2, (millis()/2000) %2  == 1);

  // a brief delay, so this runs 20 times per second
  delay(50);
}

