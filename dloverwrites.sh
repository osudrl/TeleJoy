dlCommand="wget -O"

rm usb_desc.c
rm usb_desc.h
rm usb_joystick.c 
rm usb_joystick.h 

${dlCommand} usb_desc.c "https://raw.githubusercontent.com/PaulStoffregen/cores/632f311db1b60a5b6a89f2d9c1dee782e2747e71/teensy3/usb_desc.c"
${dlCommand} usb_desc.h "https://raw.githubusercontent.com/PaulStoffregen/cores/632f311db1b60a5b6a89f2d9c1dee782e2747e71/teensy3/usb_desc.h"
${dlCommand} usb_joystick.c "https://raw.githubusercontent.com/PaulStoffregen/cores/632f311db1b60a5b6a89f2d9c1dee782e2747e71/teensy3/usb_joystick.c"
${dlCommand} usb_joystick.h "https://raw.githubusercontent.com/PaulStoffregen/cores/632f311db1b60a5b6a89f2d9c1dee782e2747e71/teensy3/usb_joystick.h"