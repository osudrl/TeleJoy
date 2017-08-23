Stream* s_paxStream;
/*
const uint8_t sensor_ids[] = {
    0x00, 0xA1, 0x22, 0x83, 0xE4, 0x45, 0x67,
    0x48, 0x6A, 0xCB, 0xAC, 0x0D, 0x8E, 0x2F
};
*/
const int DATA_COUNT = 18;

int changed[DATA_COUNT] = {
   0,1,2, 3, 4, 5, 7, 8, 10, 11, 12, 13, 15, 16,17,18,17,18
};

uint16_t telemetry_data_buffer[DATA_COUNT] = {
  20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3
};

union packet {
  //! byte[8] presentation
  uint8_t byte[8];
  //! uint64 presentation
  uint64_t uint64;
};

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

void flushInputBuffer(void)  
{
  while (s_paxStream->available())
    s_paxStream->read();
}

void setRX()
{
  s_paxStream->flush();
  flushInputBuffer(); 

  if (s_paxStream == (Stream*)&Serial3) 
  {
    UART2_C3 &= ~UART_C3_TXDIR;
    //UCSR3B = ((1 << RXCIE3) | (1 << RXEN3));
  }
}

void hdInit()
{
  Serial3.begin(57600,SERIAL_8N1_RXINV_TXINV);
  s_paxStream = &Serial3;
  
  UART2_C1 |= UART_C1_LOOPS | UART_C1_RSRC;
    //    CORE_PIN8_CONFIG = PORT_PCR_DSE | PORT_PCR_SRE | PORT_PCR_MUX(3) | PORT_PCR_PE | PORT_PCR_PS; // pullup on output pin;

  setRX();
}

void setTX()
{
  if (s_paxStream == (Stream*)&Serial3)
  {
    UART2_C3 |= UART_C3_TXDIR;
         // UCSR3B =  /*(1 << UDRIE3) |*/ (1 << TXEN3);
  }
}


uint8_t outBuf[32];
void sendData (uint8_t type, uint16_t id, int32_t val) {
  flushInputBuffer();
  setTX();
  union packet packet;

  packet.uint64  = (uint64_t) type | (uint64_t) id << 8 | (int64_t) val << 24;
  packet.byte[7] = CRC(packet.byte);
  
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
  s_paxStream->write(outBuf,outIndex);
  s_paxStream->flush();
  setRX();
}

void sendData (uint16_t id, int32_t val)
{

  sendData (0x10, id, (uint32_t) val);

}

bool validHeader = false;
int indexEditing = -1;
bool testChangeArray = false;
void tryUsbInput()
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
       
    if(!validHeader && usbIn == 251)
    {
      validHeader = true;
      indexEditing = -1;
      Serial.println("HEADER SET");
    }
    else if(validHeader && !(indexEditing < 0 || indexEditing > DATA_COUNT))
    {
      validHeader = false;      
      testChangeArray = true;
      telemetry_data_buffer[indexEditing] = usbIn;      
      Serial.print("TDATA ");
      Serial.print(indexEditing);
      Serial.print(" SET TO ");
      Serial.println(telemetry_data_buffer[indexEditing]);
      indexEditing = -1;
    }
    else if (validHeader)
    {
      indexEditing = usbIn;
      Serial.print("INDEX SET ");
      Serial.println(indexEditing);
    }
    else
    {
      Serial.println("NO MATCH");
    }
    Serial.flush();
  }
}

int validity = 0;
int count = 0;
int mod = 0;

void telemetry()
{
  digitalWrite(13,HIGH);
  unsigned char rByte = Serial3.read();
  if(rByte == 0x7e)
  {
    validity = 1;
    return;
  }
  if(validity != 1)
    return;
  validity = 0;

  if(rByte==0x83)
  {
    bool found = false;
    for(int i = 0; testChangeArray && i < DATA_COUNT; i++)
    {
      if(changed[i])
      {
        mod = i;
        found = true;
        break;
      }
    }
    testChangeArray = found;
    

    if((millis()/100) %3>0 || changed[mod])
    {
      changed[mod] = 0;
      sendData(mod,telemetry_data_buffer[mod]);
      mod = ++mod % DATA_COUNT;

    }
  }
}

void setup() 
{
	pinMode(13, OUTPUT);
	digitalWrite(13,LOW);
	hdInit();
  Serial.begin(9600);
  Serial.println("INIT");
}
void loop() 
{

  tryUsbInput();
  
  if(millis()/10000 > count)
  {
    count++;
    digitalWrite(13,LOW);
  }
  
  if(Serial3.available())
  {
    telemetry();
  }
}