/*
const uint8_t sensor_ids[] = {
    0x00, 0xA1, 0x22, 0x83, 0xE4, 0x45, 0x67,
    0x48, 0x6A, 0xCB, 0xAC, 0x0D, 0x8E, 0x2F
};
*/
//#define JOY_SEND_DEBUG_ASCII

const int tele_DATA_COUNT = 14;
const uint8_t SPORT_REQUEST_HEADER = 0x7e;
const uint8_t SPORT_ONLY_SENSOR_ID = 0x83;
const int JOY_MIN = 3950;
const int JOY_MAX = 62000;
const int IN_MIN = -820;
const int IN_MAX = 819;
const int DEADZONE_MITIGATION_CONSTANT = 3800; // 0 for very sticky deadzone, 3800 is normally pretty good

const uint8_t tele_sensorids[tele_DATA_COUNT] = {
    0x00, 0xA1, 0x22, 0x83, 0xE4, 0x45, 0x67,
    0x48, 0x6A, 0xCB, 0xAC, 0x0D, 0x8E, 0x2F
};

int tele_changed[tele_DATA_COUNT] = 
{
  1,1,1,1,1,

   1,1,1,
   1,1,1,
   1,1,
};

uint8_t tele_ids[tele_DATA_COUNT] = {
    0, 1,  2, 3, 4,

    5, 7, 8,
    10, 11,12,
    13, 14,15
};

uint16_t tele_data[tele_DATA_COUNT] =
{
  77,77,77,77,77,

  1,4,9,
  16,25,36,
  49,64,81

};

union sport_reply_packet {
  //! byte[8] presentation
  uint8_t byte[8];
  //! uint64 presentation
  uint64_t uint64;
};

uint8_t sport_CRC (uint8_t *packet) {
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

void sport_flushInputBuffer(void)  
{
  while (Serial3.available())
    Serial3.read();
}

void sport_setRX()
{
  Serial3.flush();
  sport_flushInputBuffer(); 
  UART2_C3 &= ~UART_C3_TXDIR;
}

void sport_hdInit()
{
  Serial3.begin(57600,SERIAL_8N1_RXINV_TXINV);
  UART2_C1 |= UART_C1_LOOPS | UART_C1_RSRC;
  sport_setRX();
}

void sport_setTX()
{
  UART2_C3 |= UART_C3_TXDIR;
}


uint8_t outBuf[32];

void sport_sendData (uint16_t id, int32_t val) {
  sport_flushInputBuffer();
  sport_setTX();
  union sport_reply_packet packet;

  packet.uint64  = (uint64_t) 0x10 | (uint64_t) id << 8 | (int64_t) val << 24;
  packet.byte[7] = sport_CRC(packet.byte);
  
  int outIndex = 0;
  
  for (int i=0; i<8; i++) {
    if (packet.byte[i] == 0x7D) {
      outBuf[outIndex] = (0x7D);
      outIndex++;
      outBuf[outIndex] = (0x7D);
      outIndex++;
    } else if (packet.byte[i] == 0x7E) {
      outBuf[outIndex] = (0x7D);
      outIndex++;
      outBuf[outIndex] = (0x20);
      outIndex++;
    } else {
      outBuf[outIndex] = packet.byte[i];
      outIndex++;
    }
  }
  Serial3.write(outBuf,outIndex);
  Serial3.flush();
  sport_setRX();
}


const uint8_t USB_ESCAPE_BYTE = 0xFE;
const uint8_t USB_HEADER_BYTE = 0x88;
bool usb_wasEscaped = false;
int usb_currIndex = -1; 
bool usb_lsbSet = false;
uint8_t usb_lsb = 0;

void usb_addSafeByte(uint8_t safe)
{
  if(usb_currIndex < 0 || !(usb_currIndex<tele_DATA_COUNT))
  {
    #ifdef JOY_SEND_DEBUG_ASCII
      Serial.print("BAILING ON THIS BYTE: ");
      Serial.println(safe,HEX);
    #endif
    return;
  }

  if(usb_lsbSet)
  {
    uint16_t bigLsb = (((uint16_t) usb_lsb) & 0x00ff);
    uint16_t bigMsb = (((( uint16_t) safe) << 8) & 0xff00);
    tele_data[usb_currIndex] = (bigLsb | bigMsb);
    #ifdef JOY_SEND_DEBUG_ASCII
      Serial.print("INDEX");
      Serial.print(usb_currIndex);
      Serial.print("'s TOTAL SET TO ");
      Serial.println(tele_data[usb_currIndex]);
    #endif
    usb_currIndex++;
    usb_lsbSet = false;

  }
  else
  {
    #ifdef JOY_SEND_DEBUG_ASCII
      Serial.print("INDEX");
      Serial.print(usb_currIndex);
      Serial.print("'s lsb set to ");
      Serial.println(safe);
    #endif
    usb_lsb = safe;
    usb_lsbSet = true;
  }
}

void sport_tryUsbInput()
{
  if(Serial.available())
  {
    uint8_t usbIn = Serial.read();

    if(millis() < 4000)
    {
      #ifdef JOY_SEND_DEBUG_ASCII
        Serial.print("TOO EARLY ");
        Serial.println(usbIn,HEX);
      #endif
      return;
    }

    if(!usb_wasEscaped && usbIn == USB_ESCAPE_BYTE)
    {
      usb_wasEscaped = true;
      #ifdef JOY_SEND_DEBUG_ASCII
        Serial.println("ESCAPE TURNED ON");
      #endif
      return;
    }

    if(usb_wasEscaped)
    {
      usb_wasEscaped = false;
      if(usbIn == USB_HEADER_BYTE)
      {
        #ifdef JOY_SEND_DEBUG_ASCII
          Serial.println("HEADER FOUND, ESCAPE TURNED OFF");
        #endif
        usb_currIndex = 0;
        usb_lsbSet = false;
        return;
      }
      if(usbIn == USB_ESCAPE_BYTE)
      {
        #ifdef JOY_SEND_DEBUG_ASCII
          Serial.println("ESCAPE LITERAL FOUND, ESCAPE TURNED OFF");
        #endif
        usb_addSafeByte(usbIn);
        return;
      }
      return;
    }
    //Serial.print("About to deal with: ");
    //Serial.println(usbIn,HEX);
    usb_addSafeByte(usbIn);
  }
}



int tele_validity = 0;
int tele_count = 0;
int tele_mod = 0;

void sport_telemetry()
{
  digitalWrite(13,HIGH);
  unsigned char rByte = Serial3.read();
  if(rByte == SPORT_REQUEST_HEADER)
  {
    tele_validity = 1;
    return;
  }
  if(tele_validity != 1)
    return;
  tele_validity = 0;

  if(rByte==SPORT_ONLY_SENSOR_ID)
  {
    bool found = false;
    for(int i = 0; false && i < tele_DATA_COUNT; i++)
    {
      if(tele_changed[i])
      {
        tele_mod = i;
        found = true;
        break;
      }
    }
   // tele_testChangeArray = found;
    

    if( (millis()/100) %2==0 || tele_changed[tele_mod])
    {
      tele_changed[tele_mod] = 0;
      sport_sendData(tele_ids[tele_mod],tele_data[tele_mod]);
      tele_mod = (++tele_mod) % tele_DATA_COUNT;
    }
  }
}

void sport_setup() 
{
	pinMode(13, OUTPUT);
	digitalWrite(13,LOW);
	sport_hdInit();
  Serial.begin(9600);
  #ifdef JOY_SEND_DEBUG_ASCII
  Serial.println("INIT");
  #endif
}

void sport_loop() 
{

  sport_tryUsbInput();
  
  if(millis()/10000 > tele_count)
  {
    tele_count++;
    digitalWrite(13,LOW);
  }
  
  if(Serial3.available())
  {
    sport_telemetry();
  }
}


struct _sbus_data_t
{
    int16_t analog[16];       /**< Analog channels 1-16 */
    bool    digital[2];       /**< Digital channels 17 and 18 */
    bool    frame_lost;       /**< Radio signal lost? */
    bool    failsafe_active;  /**< Channels set to failsafe values? */
    bool    signal_good;      /**< Receiving valid S-BUS packets? */
};
typedef struct _sbus_data_t sbus_data_t;

sbus_data_t controllerState;
uint8_t sbus_buffer[25];
int sbus_bytes_read = 0;
long sbus_micros = micros();
int joy_avg = ((JOY_MAX+JOY_MIN)/2);

void sbus_decode_packet(const uint8_t *packet, struct _sbus_data_t* sbus_data)
{
    // Check data packet validity before proceeding
    if (packet[0] != 0x0f || packet[24] != 0x00) {
        sbus_data->signal_good = false;
        return;
    }

    const size_t num_channels = 16;
    const int16_t zero_offset = 992;

    for (size_t i = 0; i < num_channels; ++i) {
        // Assemble each channel value from the next 11 sequential
        // bits, which are spread across two or three chars
        const size_t byte_offset = (i * 11) / 8 + 1;
        const size_t bit_offset  = (i * 11) % 8;
        uint16_t raw_value;

        if (bit_offset <= 5) {
            // Channel value is spread across two chars
            const uint8_t mask0 = ~((1 << bit_offset) - 1);
            const uint8_t mask1 =   (1 << (bit_offset + 11 - 8)) - 1;
            raw_value = (packet[byte_offset + 0] & mask0) >> (0 + bit_offset) |
                        (packet[byte_offset + 1] & mask1) << (8 - bit_offset);
        } else {
            // Channel value is spread across three chars
            const uint8_t mask0 = 0xff & ~((1 << bit_offset) - 1);
            const uint8_t mask1 = 0xff;
            const uint8_t mask2 = (1 << (bit_offset + 11 - 16)) - 1;
            raw_value = (packet[byte_offset + 0] & mask0) >> ( 0 + bit_offset) |
                        (packet[byte_offset + 1] & mask1) << ( 8 - bit_offset) |
                        (packet[byte_offset + 2] & mask2) << (16 - bit_offset);
        }

        sbus_data->analog[i] = raw_value - zero_offset;
    }

    sbus_data->digital[0]      = packet[23] & 0x01;
    sbus_data->digital[1]      = packet[23] & 0x02;
    sbus_data->frame_lost      = packet[23] & 0x04;
    sbus_data->failsafe_active = packet[23] & 0x08;
    sbus_data->signal_good     = true;
}

void sendJoyOutput()
{
    sbus_data_t* c = &controllerState;
    for(int i = 0; i < 8; i++)
    Joystick.slider(i+1,millis());
    Joystick.X(mapAnalog(c->analog[0]));
    Joystick.Y(mapAnalog(c->analog[1]));
    Joystick.Z(mapAnalog(c->analog[2]));
    Joystick.Xrotate(mapAnalog(c->analog[3]));
    Joystick.Yrotate(mapAnalog(c->analog[4]));
    Joystick.Zrotate(mapAnalog(c->analog[5]));

    int sliderCount = 10;
    
    for(int i = 0; i < sliderCount; i++)
       Joystick.slider(i+1, mapAnalog(c->analog[i+6]));
        
    Joystick.button(4, c->analog[13] < 0);
    Joystick.button(5, c->analog[15] < 0);
    Joystick.button(2, c->frame_lost);
    Joystick.button(3, c->failsafe_active);
    Joystick.button(1, c->signal_good);
    Joystick.send_now();
}

int mapAnalog(int analog)
{    
    if(analog < 0)
        return (int) map(analog,IN_MIN,0,JOY_MIN,joy_avg-DEADZONE_MITIGATION_CONSTANT);
    else if(analog > 0)
        return (int) map(analog,0,IN_MAX,joy_avg+DEADZONE_MITIGATION_CONSTANT,JOY_MAX);
    else
        return joy_avg;
}

void sbus_loop() 
{
    
    if(!Serial1.available())
     return;
    int nByte = Serial1.read();
    if (nByte < 0)
        return;
    if(micros() - sbus_micros > 3000)
        sbus_bytes_read = 0;
    sbus_micros = micros();
    sbus_buffer[sbus_bytes_read] = nByte;

    if(sbus_bytes_read == 24)
    {
        sbus_decode_packet(sbus_buffer, &controllerState);
        sbus_bytes_read = 0;

        sendJoyOutput();
    }
    else
        sbus_bytes_read += 1;   
}

void sbus_setup() {
  Serial1.begin(100000, SERIAL_8E1_RXINV);
  //Serial.begin(9600);
  //Serial.println("THIS SHOULD SHOW UP");
  Joystick.useManualSend(true);
}

void setup()
{
  sbus_setup();
  sport_setup();
}

void loop()
{
  sbus_loop();
  sport_loop();
}


