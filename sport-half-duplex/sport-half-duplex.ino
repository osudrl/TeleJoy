Stream* s_paxStream;
/*
const uint8_t sensor_ids[] = {
    0x00, 0xA1, 0x22, 0x83, 0xE4, 0x45, 0x67,
    0x48, 0x6A, 0xCB, 0xAC, 0x0D, 0x8E, 0x2F
};
*/
   uint8_t value_ids[] = {
    0, 1, 2, 3, 4, 5, 7, 8, 10, 11, 12, 13, 15, 16
  };

   uint16_t telemetry_data_buffer[] = {
    15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0
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
/*
uint8_t CRC(uint8_t *packet)
{
    uint16_t crc = 0;

    for (size_t i = 0; i < sizeof (response_packet_t); ++i) {
        crc += ((uint8_t*) packet)[i];
        crc += crc >> 8;
        crc &= 0xff;
    }

    return ~crc;
}
*/
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

void writeByte(uint8_t data)
{
  //if (s_paxStream == (Stream*)&Serial3) 
  //  while ( (UCSR3A & (1 << UDRE3)) == 0);
  s_paxStream->write(data);
  //s_paxStream->write(0xA5);
}


void sendData (uint8_t type, uint16_t id, int32_t val) {
  flushInputBuffer();
  setTX();
  union packet packet;

  packet.uint64  = (uint64_t) type | (uint64_t) id << 8 | (int64_t) val << 24;
  packet.byte[7] = CRC(packet.byte);
  
  for (int i=0; i<8; i++) {
    if (packet.byte[i] == 0x7D) {
      writeByte(0x7D);
      writeByte(0x7D);
    } else if (packet.byte[i] == 0x7E) {
      writeByte(0x7D);
      writeByte(0x20);
    } else {
      writeByte(packet.byte[i]);
    }
  }
  s_paxStream->flush();
   setRX();
}

void sendData (uint16_t id, int32_t val)
{
  
  sendData (0x10, id, (uint32_t) val);
 
}


int validity = 0;
int count = 0;
int mod = 0;
void setup() 
{
	pinMode(13, OUTPUT);
	digitalWrite(13,LOW);
	hdInit();
  //Serial.begin(9600);
}
void loop() 
{
  
  if(millis() > 20000)
    telemetry_data_buffer[8] = millis();
  
  if(millis()/500 > count)
  {
    count++;
    digitalWrite(13,LOW);
  }
  
	if(Serial3.available())
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
    
    if(rByte==0xA1 && (millis()/100) %3>0 )
    {
      //value_ids[mod] = 0;
      sendData(mod,telemetry_data_buffer[mod]);
      mod = ++mod % 16;
    }
       
    /*
		if((rByte == 0x98 || rByte == 0xA1))
    {
      int num = indd%18;
      
      if(num == 16)
          sendData(0x0011,(millis()/10000) %10);
      else if(num == 17)
          sendData(0x0014,(millis()/10000) %10);
      else
          sendData(num,(millis()/10000) %10);
      indd++;
    }
    */
	}
}
