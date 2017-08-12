dlCommand="wget -O"
mkdir dlTemp

filepath=$(find . -name "teensy3" | grep hardware)
parentname=$(dirname "$filepath")

parentname=$(dirname "$parentname")
parentname=$(dirname "$parentname")
parentname="$(basename "$(dirname "$parentname")")"
echo "$parentname"

#${dlCommand} dlTemp/usb_desc.c "https://raw.githubusercontent.com/PaulStoffregen/cores/632f311db1b60a5b6a89f2d9c1dee782e2747e71/teensy3/usb_desc.c"
#${dlCommand} dlTemp/usb_desc.h "https://raw.githubusercontent.com/PaulStoffregen/cores/632f311db1b60a5b6a89f2d9c1dee782e2747e71/teensy3/usb_desc.h"
#${dlCommand} dlTemp/usb_joystick.c "https://raw.githubusercontent.com/PaulStoffregen/cores/632f311db1b60a5b6a89f2d9c1dee782e2747e71/teensy3/usb_joystick.c"
#${dlCommand} dlTemp/usb_joystick.h "https://raw.githubusercontent.com/PaulStoffregen/cores/632f311db1b60a5b6a89f2d9c1dee782e2747e71/teensy3/usb_joystick.h"