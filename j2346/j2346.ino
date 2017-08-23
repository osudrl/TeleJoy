/*
const uint8_t sensor_ids[] = {
    0x00, 0xA1, 0x22, 0x83, 0xE4, 0x45, 0x67,
    0x48, 0x6A, 0xCB, 0xAC, 0x0D, 0x8E, 0x2F
};
*/
const int tele_DATA_COUNT = 9;

int tele_changed[tele_DATA_COUNT] = 
{
   1,1,1,
   1,1,1,
   1,1,1
};

uint16_t tele_ids[tele_DATA_COUNT] = 
{
  5,7,8,
  10,11,12,
  13,14,15
};

uint16_t tele_data[tele_DATA_COUNT] =
{
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

bool usb_validHeader = false;
int usb_indexEditing = -1;
bool tele_testChangeArray = false;

void sport_tryUsbInput()
{
  while(Serial.available())
  {
    //Serial.print("READ ");
    uint8_t usbIn = Serial.read();
    //Serial.println(usbIn,HEX);

    
    if(millis() < 7500)
    {
      Serial.print("TOO EARLY ");
    Serial.println(usbIn,HEX);
    continue;
    }
       
    if(!usb_validHeader && usbIn == 251)
    {
      usb_validHeader = true;
      usb_indexEditing = -1;
      Serial.println("HEADER SET");
    }
    else if(usb_validHeader && !(usb_indexEditing < 0 || usb_indexEditing > tele_DATA_COUNT))
    {
      usb_validHeader = false;      
      tele_testChangeArray = true;
      tele_data[usb_indexEditing] = usbIn;      
      Serial.print("TDATA ");
      Serial.print(usb_indexEditing);
      Serial.print(" SET TO ");
      Serial.println(tele_data[usb_indexEditing]);
      usb_indexEditing = -1;
    }
    else if (usb_validHeader)
    {
      usb_indexEditing = usbIn;
      Serial.print("INDEX SET ");
      Serial.println(usb_indexEditing);
    }
    else
    {
      Serial.println("NO MATCH");
    }
    Serial.flush();
  }
}

int tele_validity = 0;
int tele_count = 0;
int tele_mod = 0;

void sport_telemetry()
{
  digitalWrite(13,HIGH);
  unsigned char rByte = Serial3.read();
  if(rByte == 0x7e)
  {
    tele_validity = 1;
    return;
  }
  if(tele_validity != 1)
    return;
  tele_validity = 0;

  if(rByte==0x83)
  {
    bool found = false;
    for(int i = 0; tele_testChangeArray && i < tele_DATA_COUNT; i++)
    {
      if(tele_changed[i])
      {
        tele_mod = i;
        found = true;
        break;
      }
    }
    tele_testChangeArray = found;
    

    if((millis()/100) %3>0 || tele_changed[tele_mod])
    {
      tele_changed[tele_mod] = 0;
      sport_sendData(tele_ids[tele_mod],tele_data[tele_mod]);
      tele_mod = ++tele_mod % tele_DATA_COUNT;

    }
  }
}

void setup() 
{
	pinMode(13, OUTPUT);
	digitalWrite(13,LOW);
	sport_hdInit();
  Serial.begin(9600);
  Serial.println("INIT");
}
void loop() 
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
