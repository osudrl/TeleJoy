

typedef struct {
    int16_t analog[16];       /**< Analog channels 1-16 */
    bool    digital[2];       /**< Digital channels 17 and 18 */
    bool    frame_lost;       /**< Radio signal lost? */
    bool    failsafe_active;  /**< Channels set to failsafe values? */
    bool    signal_good;      /**< Receiving valid S-BUS packets? */
} sbus_data_t;

sbus_data_t controllerState;
uint8_t buffer[25];
int bytesRead = 0;

void sbus_decode_packet(const uint8_t *packet, sbus_data_t *sbus_data)
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

void setup() {
  Serial1.begin(100000, SERIAL_8E1_RXINV);
  Serial.begin(9600);
  Serial.println("THIS SHOULD SHOW UP");
}

long time = micros();
int count = 0;
void loop() 
{
    while (!Serial1.available()){};
    int nByte = Serial1.read();
    if (nByte < 0)
        return;
    if(micros() - time > 3000)
        bytesRead = 0;
    time = micros();
    buffer[bytesRead] = nByte;

    if(bytesRead == 24)
    {
        sbus_decode_packet(buffer, &controllerState);
        bytesRead = 0;

        if(millis()/100 > count)
        {
            for (int i = 0; i < 16; i++)
            {
                Serial.print("A");
                Serial.print(i);
                Serial.print(":");
                Serial.print(controllerState.analog[i]);
                Serial.print("  ");
            }
            Serial.print("D0:");
            Serial.print(controllerState.digital[0]);
            Serial.print(" D1:");
            Serial.print(controllerState.digital[1]);
            Serial.print(" FL:");
            Serial.print(controllerState.frame_lost);
            Serial.print(" FA:");
            Serial.print(controllerState.failsafe_active);
            Serial.println("");
            count++;
        }
    }
    else
        bytesRead += 1;
}


