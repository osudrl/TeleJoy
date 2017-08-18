/** \mainpage
   C library for Frsky Smart Port protocol.

   This library is not designed to decode data on the transmitter, but on the receiver side. OpenTX makes the rest of
    the job on the transmitter.

   This development is completely independent of Frsky or OpenTX.

   What to know about Smart Port
   =============================
 * * use an inverted serial communication at 57600bds muxed on one port
 * * the receiver polls 27 physical sensors periodically
 * * compare to D, the SP protocol seems much faster, but maybe not - it's easier to use for data encoding, and is
     nicer to have a bus than a hub, but the polling of 27 IDs takes time (i.e. if a sensor has many values, it can
     send only one at a time, ex. GPS).

   Receiver behavior
   -----------------
   The receiver polls in a cyle of (a bit more of) 11 ms.

   byte | description
   -----|------------
   0x7E | poll header
   ID   | physical ID (1-27) computed with a CRC (see \ref FrskySP_sensor_demo/FrskySP_sensor_demo.ino for the full list of polled IDs)

 * * The receiver will poll the IDs in sequence to find which one is present.
 * * If only one physical ID is found, the receiver will alternate the sensor polling and the search (present sensor,
     next ID to search, present sensor, next ID and so on).
 * * If more sensors are found, the poll sequence returns almost to a normal search pattern.

   Sensor behavior
   ---------------
   Genuine Frsky sensors: the sensor answers to every pool on its physical ID to announce its presence. If no data can
   be transmitted (no refresh), the sensor answers by an empty packet and a false CRC (type 0x00, ID 0x0000,
   value 0x00000000, CRC 0xFF).

   byte(s) | descrption
   --------|-----------
   1       | type (only 0x10 at now)
   2       | sensor logical ID (see \ref FrskySP.h for the full list of IDs)
   4       | value
   1       | CRC

   Slowness considerations
   =======================
   There are 2 recurrent discussions on Internet, that are related to what is used in this code and examples:
 * * SoftwareSerial is too slow - no, it is not (at least in Arduino 1.0+). The answers from the Arduino are even faster
     than the genuine Frksy sensors. Allthough, the annoying point is the conflict with the PinChangeInt library.
 * * Floating point computing is slow - yes and no. The slowness is reversed if the input or the output of the formula
     is a float. Example:
   ~~~~~
   int b = (int) a / 10;     // faster (a & b are integers)
   int b = (int) a * 0.1;    // slower

   float b = (int) a / 10;   // slower (b is a float)
   float b = (int) a * 0.1;  // faster
   ~~~~~
   A float computing can take up to 40us. Unless you make a lot of them, there is plenty of time to answer within the
   poll cycle (11 ms). Allthough, OpenTX has many computing to do and has no time to lose. There is a little drift for
   the GPS and airspeed values shown on the remote control.

   Although, you must be careful around those issues:
 * * only one sensor per physical ID (ex. GPS and normal precision altimeter share the same physical ID 3)
 * * only one answer per poll cycle (the [FrskySP_sensor_demo.ino](\ref FrskySP_sensor_demo/FrskySP_sensor_demo.ino)
     example shows how to handle multiple answers for one physical ID)
 * * take care about the polling time of the sensor. For instance, polling a DS18x20 temperature sensor takes up to
     750ms. The polling must be asynchronous to be answered within the cycle of 11ms.

   Connection draft
   ================
   \image html Smart_Port_bb.png

 * * pull down resistor on TX line (100k)
 * * diode between TX and RX (ex. 1N4108)

   On this circuit, RX will hang after serial begin. There is a workaround that inverts the TX pinMode to INPUT and back
    to OUTPUT.

   \version devel
   \author Jean-Christophe Heger
   \see https://github.com/jcheger/frsky-arduino/ - source of this library
   \see http://www.frsky-rc.com/
   \see http://www.open-tx.org/
   \copyright 2014 - Jean-Christophe Heger - Released under the LGPL 3.0 license.
   \ChangeLog 2014-06-27 - public devel release
   \todo write an example to simulate an X8R receiver

   \bug There is an unsolved bug with one value only, until now. While trying to send airspeed value 100 mph, converted
   to knots, the receiver will not detect the sensor and not send the value to the remote neither. It works perfectly
   with 101 or 99 mph, but 100 mph will hang.
   ~~~
   FrskySP.sendData (FRSKY_SP_AIR_SPEED, 100 * 10 / 1.15077945);    // packet: 0x10 00 0A 64 03 00 00 7E
   ~~~
*/

#include "FrskySP.h"

/**
   Smart Port protocol uses 8 bytes packets.

   Packet format (byte): tiivvvvc
   - t: type (1 byte)
   - i: sensor ID (2 bytes)
   - v: value (4 bytes - int32)
   - c: crc

   The uint64 presentation is much easier to use for data shifting.
*/
union packet {
  //! byte[8] presentation
  uint8_t byte[8];
  //! uint64 presentation
  uint64_t uint64;
};

void initSerial () {
  //this->mySerial->begin (57600);
}

/**
   Check if a byte is available on Smart Port
*/
int available () {
  //return this->mySerial->available ();
}

/**
   \brief Calculate the CRC of a packet
   \see https://github.com/opentx/opentx/blob/next/radio/src/telemetry/frsky_sport.cpp
   \param packet packet pointer (in byte[8] presentation)
*/
uint8_t CRC (uint8_t *packet) {
  short crc = 0;
  for (int i = 0; i < 8; i++) {
    crc += packet[i]; //0-1FF
    crc += crc >> 8;  //0-100
    crc &= 0x00ff;
    crc += crc >> 8;  //0-0FF
    crc &= 0x00ff;
  }
  return ~crc;
}

byte read () {
  //return this->mySerial->read ();
}

/**
   Sensors logical IDs and value formats are documented in FrskySP.h.

   \brief Simplified version of sendData(), while the type is only 0x10 at now.
   \param id sensor ID
   \param val value
*/
void sendData (uint16_t id, int32_t val) {
  this->sendData (0x10, id, (uint32_t) val);
}

/**
   Sensors logical IDs and value formats are documented in FrskySP.h.

   Packet format:
   content   | length | remark
   --------- | ------ | ------
   type      | 8 bit  | always 0x10 at now
   sensor ID | 16 bit | sensor's logical ID (see FrskySP.h for values)
   data      | 32 bit | preformated data
   crc       | 8 bit  | calculated by CRC()

   \brief Prepare the packet and send it.
   \param type value type
   \param id sensor ID
   \param val value
   \return return the CRC for control
*/
void sendData (uint8_t type, uint16_t id, int32_t val) {
  int i = 0;
  union packet packet;

  packet.uint64  = (uint64_t) type | (uint64_t) id << 8 | (int64_t) val << 24;
  packet.byte[7] = this->CRC (packet.byte);
  
  this->_ledToggle (HIGH);
  for (i=0; i<8; i++) {
    if (packet.byte[i] == 0x7D) {
      this->mySerial->write (0x7D);
      this->mySerial->write (0x7D);
    } else if (packet.byte[i] == 0x7E) {
      this->mySerial->write (0x7D);
      this->mySerial->write (0x20);
    } else {
      this->mySerial->write (packet.byte[i]);
    }
  }
  this->_ledToggle (LOW);
}

byte write (byte val) {
  //return this->mySerial->write (val);
}
