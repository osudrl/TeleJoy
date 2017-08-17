Stream* s_paxStream;
void flushInputBuffer(void)  
{
	while (s_paxStream->available())
		s_paxStream->read();
}

void setRX()
{
	flushInputBuffer();
	s_paxStream->flush();

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
	Serial.println("inti");
}
long mic;
void loop() 
{
	if(Serial3.available())
 {
    Serial.print(micros()-mic);
    mic = micros();
    Serial.print(" : ");
		Serial.println(Serial3.read());
 }
}

