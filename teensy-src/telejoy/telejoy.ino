

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
  Joystick.useManualSend(true);
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

        sendJoyOutput();
    }
    else
        bytesRead += 1;
}

bool force_digital = false;
int joy_min = 3950;
int joy_max = 62000;
int in_min = -820;
int in_max = 819;
int d = 0;
void sendJoyOutput()
{
  sbus_data_t* c = &controllerState;
    for(int i = 0; i < 8; i++)
    Joystick.slider(i+1,millis());
    Joystick.X(mapAnalog(c->analog[1]));
    Joystick.Y(mapAnalog(c->analog[0]));
    Joystick.Z(mapAnalog(c->analog[3]));
    Joystick.Zrotate(mapAnalog(c->analog[2]));

    int sliderCount = 4;
    if(!force_digital)
        sliderCount = 8;
    for(int i = 0; i < sliderCount; i++)
       Joystick.slider(i+1, mapAnalog(c->analog[i+4]));
    for(int i = 8; i < 17; i++)
       Joystick.slider(i+1, millis());
    if(!force_digital)
    {
        Joystick.Xrotate(mapAnalog(c->analog[12]));
        Joystick.Yrotate(mapAnalog(c->analog[14]));
    }    
    Joystick.button(4, c->analog[13] < 0);
    Joystick.button(5, c->analog[15] < 0);
    Joystick.button(2, c->frame_lost);
    Joystick.button(3, c->failsafe_active);
    Joystick.button(1, c->signal_good);
    Joystick.send_now();
}
int m()
{
  return millis() * 10;
}
int mapAnalog(int analog)
{
    return (int) map(analog,in_min,in_max,joy_min,joy_max);   
}


