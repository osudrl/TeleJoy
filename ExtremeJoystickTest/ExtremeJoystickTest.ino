/* Complete USB Joystick Example
   Teensy becomes a USB joystick with 16 or 32 buttons and 6 axis input

   You must select Joystick from the "Tools > USB Type" menu

   Pushbuttons should be connected between the digital pins and ground.
   Potentiometers should be connected to analog inputs 0 to 5.

   This example code is in the public domain.
*/

// Configure the number of buttons.  Be careful not
// to use a pin for both a digital button and analog
// axis.  The pullup resistor will interfere with
// the analog voltage.
const int numButtons = 14;  // 16 for Teensy, 32 for Teensy++

void setup() {
  // you can print to the serial monitor while the joystick is active!
  Serial.begin(9600);
  // configure the joystick to manual send mode.  This gives precise
  // control over when the computer receives updates, but it does
  // require you to manually call Joystick.send_now().
  Joystick.useManualSend(true);
  for (int i=0; i<numButtons; i++) {
    pinMode(i, INPUT_PULLUP);
  }
  Serial.println("Begin Complete Joystick Test");
}

byte allButtons[numButtons];
byte prevButtons[numButtons];
int angle=0;

void loop() {
  
  //Serial.println("test");
  
  // read 6 analog inputs and use them for the joystick axis
  Joystick.X(analogRead(A0) * 64);
  Joystick.Y(analogRead(A1) * 64);
  Joystick.Z(analogRead(A2) * 64);
  Joystick.Xrotate(analogRead(A3) * 64);
  Joystick.Yrotate(analogRead(A4) * 64);
  Joystick.Zrotate(analogRead(A5) * 64);
  
  Joystick.slider(1, analogRead(A6) * 64);
  Joystick.slider(2, analogRead(A7) * 64);
  Joystick.slider(3, analogRead(A8) * 64);
  Joystick.slider(4, analogRead(A9) * 64);
  Joystick.slider(5, analogRead(A10) * 64);
  Joystick.slider(6, analogRead(A11) * 64);
  Joystick.slider(7, analogRead(A12) * 64);
  Joystick.slider(8, analogRead(A13) * 64);

  
  //Joystick.sliderLeft(analogRead(4));
  //Joystick.sliderRight(analogRead(5));
  
  // read digital pins and use them for the buttons
  for (int i=0; i<numButtons; i++) {
    if (digitalRead(i)) {
      // when a pin reads high, the button is not pressed
      // the pullup resistor creates the "on" signal
      allButtons[i] = 0;
    } else {
      // when a pin reads low, the button is connecting to ground.
      allButtons[i] = 1;
    }
    Joystick.button(i + 1, allButtons[i]);
  }

  // make the hat switch automatically move in a circle
  angle = angle + 1;
  if (angle >= 360) angle = 0;
  Joystick.hat(1, angle);
  Joystick.hat(2, angle);
  Joystick.hat(3, angle);
  Joystick.hat(4, angle);
  
  // Because setup configured the Joystick manual send,
  // the computer does not see any of the changes yet.
  // This send_now() transmits everything all at once.
  Joystick.send_now();
  
  // check to see if any button changed since last time
  boolean anyChange = false;
  for (int i=0; i<numButtons; i++) {
    if (allButtons[i] != prevButtons[i]) anyChange = true;
    prevButtons[i] = allButtons[i];
  }
  
  // if any button changed, print them to the serial monitor
  if (anyChange) {
    Serial.print("Buttons: ");
    for (int i=0; i<numButtons; i++) {
      Serial.print(allButtons[i], DEC);
    }
    Serial.println();
  }
  
  // a brief delay, so this runs "only" 200 times per second
  delay(10);
}

