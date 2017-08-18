Stream* s_paxStream;



#define TELEMETRY_DATA_LENGTH 14

const uint16_t data_type_ids[] = {
  0x0000, 0x0100, 0x0110, 0x0200, 0x0210,0x0300,0x0400,0x0410,
  0x0500,0x0600,0x0700,0x0710,0x0720,0x0800,0x0820,0x0830,0x0840,
  0x0850,0x0900,0x0910,0x0a00,0xf101,0xf102,0xf103,0xf104,0xf105
};

const uint8_t sensor_ids[] = {0x00, 0xA1, 0x22, 0x83, 0xE4, 0x45, 0xC6, 0x67, 0x48, 0xE9, 0x6A, 0xCB,
  0xAC, 0x0D, 0x8E, 0x2F, 0xD0, 0x71, 0xF2, 0x53, 0x34, 0x95, 0x16, 0xB7,
  0x98, 0x39, 0xBA, 0x1B};

// Value ID mapping
  const uint8_t value_ids[TELEMETRY_DATA_LENGTH] = {
    0, 1, 2, 3, 4, 5, 7, 8, 10, 11, 12, 13, 14, 15
  };

  const uint16_t telemetry_data_buffer[TELEMETRY_DATA_LENGTH] = {
    0, 1, 4, 9, 16, 25, 49, 64, 100, 121, 144, 169, 196, 225
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

int count = 0;
int indd = 0;
void setup() 
{
	pinMode(13, OUTPUT);
	digitalWrite(13,LOW);
	Serial.begin(9600);	
	hdInit();
	Serial.println("init");
}
void loop() 
{
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
			Serial.print("valid: 126  ");
			return;
		}
		/*
		for (int i = 0; i < TELEMETRY_DATA_LENGTH; ++i) 
		{
			if (rByte == obfuscated_sensor_ids[i])
			{
				Serial.print("<matched>   byte:");
				Serial.print(rByte);
				Serial.print(" valueid:");
				Serial.print(value_ids[i]);
				Serial.print(" td-data:");
				Serial.println(telemetry_data_buffer[i]);

            // Respond to the first matching id
				ksrp(value_ids[i], telemetry_data_buffer[i]) ;
				return;
			}
		}
		Serial.print("<unmatched> ");
		Serial.println(rByte);
		*/
     for(int i = 0; i < sizeof(sensor_ids); i += 2)
     {
      if(rByte == sensor_ids[i])
      {
    int tempp = i%(sizeof(data_type_ids)/2);
		sendData(data_type_ids[tempp], tempp*tempp);
  break;
}
    }
	}
}
