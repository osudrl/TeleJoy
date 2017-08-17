Stream* s_paxStream;

uint8_t expanded_response_buffer[16];

#define TELEMETRY_DATA_LENGTH 14

// Sensor ID mapping
static const uint8_t obfuscated_sensor_ids[TELEMETRY_DATA_LENGTH] = {
    0x00, 0xA1, 0x22, 0x83, 0xE4, 0x45, 0x67,
    0x48, 0x6A, 0xCB, 0xAC, 0x0D, 0x8E, 0x2F
};

// Value ID mapping
static const uint8_t value_ids[TELEMETRY_DATA_LENGTH] = {
    0, 1, 2, 3, 4, 5, 7, 8, 10, 11, 12, 13, 14, 15
};

static const uint16_t telemetry_data_buffer[TELEMETRY_DATA_LENGTH] = {
    0, 1, 4, 9, 16, 25, 49, 64, 100, 121, 144, 169, 196, 225
};

typedef struct __attribute__((__packed__)) {
    uint8_t  header;
    uint16_t value_id;
    uint32_t data;
    uint8_t  crc;
} response_packet_t;

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
  s_paxStream = &Serial3;
  if (s_paxStream == &Serial3)
  {
    Serial3.begin(57600,SERIAL_8N1_RXINV_TXINV);

    UART2_C1 |= UART_C1_LOOPS | UART_C1_RSRC;
        CORE_PIN8_CONFIG = PORT_PCR_DSE | PORT_PCR_SRE | PORT_PCR_MUX(3) | PORT_PCR_PE | PORT_PCR_PS; // pullup on output pin;
    }
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
int expand_response_packet(const response_packet_t *packet, uint8_t *expanded)
{
    int expanded_length;

    for (int i = 0; i < sizeof (response_packet_t); ++i) {
        switch (((uint8_t *) packet)[i]) {
        case 0x7d:
            expanded[expanded_length++] = 0x7d;
            expanded[expanded_length++] = 0x5d;
            break;
        case 0x7e:
            expanded[expanded_length++] = 0x7e;
            expanded[expanded_length++] = 0x5e;
            break;
        default:
            expanded[expanded_length++] = ((uint8_t*) packet)[i];
        }
    }

    return expanded_length;
}

uint8_t sport_crc(const response_packet_t *packet)
{
    uint16_t crc = 0;

    for (int i = 0; i < sizeof (response_packet_t); ++i) {
        crc += ((uint8_t*) packet)[i];
        crc += crc >> 8;
        crc &= 0xff;
    }

    return ~crc;
}

void ksrp(uint16_t value_id, uint32_t data)
{
    // Construct response packet
    response_packet_t response_packet;
  
        response_packet.header = 0x10,
        response_packet.value_id = value_id;//swap16(value_id),
        response_packet.data = data;//swap32(data)


    // Calculate crc(?) for the response packet
    response_packet.crc = sport_crc(&response_packet);

    // Escape special bytes in the packet
    int expanded_length =
        expand_response_packet(&response_packet,
                               expanded_response_buffer);

    flushInputBuffer();
    setTX();

    for (int i = 0; i < expanded_length; i++)
    	writeByte(expanded_response_buffer[i]);

    s_paxStream->flush();
    setRX();
}




void writeByte(unsigned char data)
{
	//if (s_paxStream == (Stream*)&Serial3) 
	//	while ( (UCSR3A & (1 << UDRE3)) == 0);
	s_paxStream->write(data);
}

void writeMultipleBytes(unsigned char *pdata, int length)
{
#if defined(ARDUINO_ARCH_AVR)
	while (length--)
		writeByte(*pdata++);
#else
	s_paxStream->write(pdata, length);
#endif
}
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
	if(Serial3.available())
	{
		digitalWrite(13,HIGH);
		mic = micros();
		unsigned char rByte = Serial3.read();
		if(rByte == 0x7e)
		{
			Serial.print("valid: 126  ")
			return;
		}
		for (int i = 0; i < TELEMETRY_DATA_LENGTH; ++i) 
		{
			if (rByte == obfuscated_sensor_ids[i])
			{
				Serial.print("<matched> byte:")
				Serial.print(rByte);
				Serial.print(" valueid:")
				Serial.print(value_ids[i]);
				Serial.print(" td-data:");
				Serial.println(telemetry_data_buffer[i]);

            // Respond to the first matching id
				ksrp(value_ids[i], telemetry_data_buffer[i]);
				return;
			}
		}
		Serial.print("<unmatched> ")
		Serial.println(rByte);
	}

}

