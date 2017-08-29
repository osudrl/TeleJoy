#include <math.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <string.h>
#include <stdbool.h>

#include "cpTime.h"
#include <pthread.h>
#include "../../joy/jt-constants.h"

#define SERIAL_TEST_COUNTING
// #define SERIAL_TEST_ESCAPING
// #define SERIAL_TEST_SQAURES


SDL_Joystick *get_joystick()
{
    
    const int n = SDL_NumJoysticks();
    char name[] = "Teensyduino Serial/Keyboard/Mouse/Joystick";
    for (int i = 0; i < n; ++i) {
        if (strncmp(SDL_JoystickNameForIndex(i), name, sizeof name) == 0)
            return SDL_JoystickOpen(i);
    }
    return NULL;
}

void* joy_read(void)
{   
    printf("About to init JOY \n");
    SDL_Init(SDL_INIT_JOYSTICK);
    printf("JOY was properly init\n");
    SDL_Joystick* ctrl = get_joystick();
    

    double radio_input[16] = {0};


    while(ctrl) 
    {
        SDL_JoystickUpdate();
        for (int i = 0; i < 16; i++)
        {
                 radio_input[i] = SDL_JoystickGetAxis(ctrl, i) / 32768.0;
                 printf("A%d:%.3f ",i,radio_input[i]);
        }

        for (int i = 0; i < 5; i++)
        {
            printf("D%i:%d ",i,SDL_JoystickGetButton(ctrl, i));
        }
        printf("\n");
        //cpSleep(1000);        
    }
    SDL_Quit();
    printf("k done\n");
    /*
    int motor_index = 0;
    int mode_index = 1;
    int hat_last = 0;

    cassie_t *c = cassie_init();
    cassie_vis_t *v = cassie_vis_init();
    cassie_state_t *s = cassie_state_alloc();
    cassie_get_state(c, s);

    double radio_input[16] = {0};
    radio_input[6] = 1;
    radio_input[8] = 1;

    double messages[4] = {-1, -1, -1, -1};

    do {
        int steps_per_frame = SDL_JoystickGetButton(xbox, 0) ? 3 : 33;

        for (int i = 0; i < steps_per_frame; ++i)
            cassie_step(c, radio_input);

        cassie_messages(c, messages);
        printf("%.0f, %.0f, %.0f, %.0f\n", messages[0], messages[1], messages[2], messages[3]);

        if (xbox) {
            SDL_JoystickUpdate();
            radio_input[0] = -SDL_JoystickGetAxis(xbox, 1) / 32768.0;
            radio_input[1] =  SDL_JoystickGetAxis(xbox, 0) / 32768.0;
            radio_input[2] = -SDL_JoystickGetAxis(xbox, 4) / 32768.0;
            radio_input[3] =  SDL_JoystickGetAxis(xbox, 3) / 32768.0;
            radio_input[6] = -SDL_JoystickGetAxis(xbox, 2) / 32768.0;
            radio_input[7] = -SDL_JoystickGetAxis(xbox, 5) / 32768.0;

            int hat = SDL_JoystickGetHat(xbox, 0);
            if (hat & 0x1 && !(hat_last & 0x01)) ++mode_index;
            if (hat & 0x2 && !(hat_last & 0x02)) ++motor_index;
            if (hat & 0x4 && !(hat_last & 0x04)) --mode_index;
            if (hat & 0x8 && !(hat_last & 0x08)) --motor_index;
            mode_index = (mode_index + 3) % 3;
            motor_index = (motor_index + 5) % 5;
            hat_last = hat;

            if (SDL_JoystickGetButton(xbox, 7))
                cassie_set_state(c, s);

            radio_input[4] = motor_index / 4.0;
            radio_input[9] = mode_index - 1.0;
        }
    } while (cassie_vis_draw(v, c));

    cassie_vis_free(v);
    cassie_free(c);
    cassie_cleanup();
    
    SDL_Quit();
    */

    return 0;

}

void* serial_read()
{
  FILE* input;
  char text[10000];
  input = fopen("/dev/ttyACM1", "r"); 
  int index = 0;

  while(true || cpMillis() < 60000)
  {
    char ch = getc (input);

    if( ch == EOF)
      continue;
    if ( ch != '\n')
     text[index++] = ch;
      else 
    {
    text[index] = '\0';
    index = 0;

   printf ( "%s\n", text );
    }
  }
}

FILE* output;

#ifdef SERIAL_TEST_COUNTING

int16_t sourceInts[tele_DATA_COUNT];
uint8_t printBuffer[tele_MAX_BUF];
int build_escaped_buffer(int16_t* source, uint8_t* result)
{
  int buildIndex = 0;
  result[buildIndex++] = USB_ESCAPE_BYTE /*0xFE*/;
  result[buildIndex++] = USB_HEADER_BYTE /*0x88*/;
  for(int i = 0; i < tele_DATA_COUNT; i++)
  {
    uint8_t lsb = (uint8_t) (source[i] & 0x00ff);
    uint8_t msb = (uint8_t) ((source[i] & 0xff00) >> 8);
    if(lsb == USB_ESCAPE_BYTE)
    {
      result[buildIndex++] = USB_ESCAPE_BYTE /*0xFE*/;
      result[buildIndex++] = USB_ESCAPE_BYTE /*0xFE*/; 
    }
    else
      result[buildIndex++] = lsb;

    if(msb == USB_ESCAPE_BYTE)
    {
      result[buildIndex++] = USB_ESCAPE_BYTE /*0xFE*/;
      result[buildIndex++] = USB_ESCAPE_BYTE /*0xFE*/;
    }
    else
      result[buildIndex++] = msb;
  }
  return buildIndex;
}

void sendBuffer(uint8_t* buf, int filled)
{
  for(int i = 0; i < filled; i++)
    fprintf(output,"%c",buf[i]);
  fflush(output);
}

void* serial_write()
{
  printf("The Counting Serial Test\n");
  #warning "The Counting Serial Test"

  output = fopen("/dev/ttyACM1", "w");     //open the terminal screen
  if ( output == NULL )
  {
    printf("ACM1 is NULL, trying ACM0.\n");
    output = fopen("/dev/ttyACM0", "w");
    if (output == NULL)
    {
      printf("ACM0 is also NULL. Exiting.\n");
      return (void*) 1;
    }
  }
  cpSleep(100);
  while(true)
  {
    for(int i = 0; i < tele_DATA_COUNT; i+=1 )
    {
      int currMod = sourceInts[i];
      if (i == 7)
        currMod = 1010;
      else if (i == 9)
        currMod = (cpMillis()+1500)/3000;
      else if (i == 11)
        currMod = cpMillis()/10000;
      else
        currMod = ((cpMillis()+(i*100)) / 1000);

      if(currMod == 2313)
        printf("im currently doing a bad");

      sourceInts[i] = currMod;
    }
    int howMany = build_escaped_buffer(sourceInts,printBuffer);
    sendBuffer(printBuffer, howMany);
    cpSleep(15);
  }
  fclose(output);
}

#elif defined SERIAL_TEST_ESCAPING

void* serial_write()
{
  printf("Escaped Serial Test\n");
  #warning "Escaped Serial Test"
  output = fopen("/dev/ttyACM1", "w");     //open the terminal screen
  if ( output == NULL )
  {
    printf("ACM1 is NULL, trying ACM0.\n");
    output = fopen("/dev/ttyACM0", "w");
    if (output == NULL)
    {
      printf("ACM0 is also NULL. Exiting.\n");
      return (void*) 1;
    }
  }

  cpSleep(100);
  fprintf(output,"%c",0xfe);
  fprintf(output,"%c",0x88);

  //the rest are numbers  
  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x01);
  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x04);
  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x09);
  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x10);
  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x19);
  fprintf(output,"%c",0x00);

  //modified value 6
  //fprintf(output,"%c",0x24);
  fprintf(output,"%c",0xFE); // escape byte
  fprintf(output,"%c",0xFE); // tells teensy the value actually is 254
  fprintf(output,"%c",0x00);

  fprintf(output,"%c",0x31);
  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x40);
  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x51);
  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x64);
  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x79);
  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x90);
  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0xA9);
  fprintf(output,"%c",0x00);

  fflush(output);
  fclose(output);

}

#else

void* serial_write()
{
  printf("Squares Serial Test\n");
  #warning "Squares Serial Test"
  output = fopen("/dev/ttyACM1", "w");     //open the terminal screen
  if ( output == NULL )
  {
    printf("ACM1 is NULL, trying ACM0.\n");
    output = fopen("/dev/ttyACM0", "w");
    if (output == NULL)
    {
      printf("ACM0 is also NULL. Exiting.\n");
      return (void*) 1;
    }
  }

  cpSleep(100);

  //header bytes
  fprintf(output,"%c",0xfe);
  fprintf(output,"%c",0x88);

  //the rest are numbers  
  fprintf(output,"%c",0x00);
  fprintf(output,"%c",0x00);

  fprintf(output,"%c",0x01);
  fprintf(output,"%c",0x00);

  fprintf(output,"%c",0x04);
  fprintf(output,"%c",0x00);

  fprintf(output,"%c",0x09);
  fprintf(output,"%c",0x00);

  fprintf(output,"%c",0x10);
  fprintf(output,"%c",0x00);

  fprintf(output,"%c",0x19);
  fprintf(output,"%c",0x00);

  fprintf(output,"%c",0x24);
  fprintf(output,"%c",0x00);

  fprintf(output,"%c",0x31);
  fprintf(output,"%c",0x00);

  fprintf(output,"%c",0x40);
  fprintf(output,"%c",0x00);

  fprintf(output,"%c",0x51);
  fprintf(output,"%c",0x00);

  fprintf(output,"%c",0x64);
  fprintf(output,"%c",0x00);

  fprintf(output,"%c",0x79);
  fprintf(output,"%c",0x00);

  fprintf(output,"%c",0x90);
  fprintf(output,"%c",0x00);

  fprintf(output,"%c",0xA9);
  fprintf(output,"%c",0x00);

  fflush(output);
  fclose(output);
}

#endif

int main(void)
{
    pthread_t sreader;
    pthread_t swriter;
    pthread_t joy;

    void*result;

    if (pthread_create(&sreader, NULL, serial_read, NULL) == -1)
        printf("Can't create thread serial_read");
    if (pthread_create(&swriter, NULL, serial_write, NULL) == -1)
        printf("Can't create thread serial_write");
    if (pthread_create(&joy, NULL, joy_read, NULL) == -1)
        printf("Can't create thread joy");
    if (pthread_join(sreader, &result) == -1)
        printf("Can't join thread serial_read");
    if (pthread_join(swriter, &result) == -1)
        printf("Can't join thread serial_write");
    if (pthread_join(joy, &result) == -1)
        printf("Can't join thread joy");

    printf("About to exit??");
    return 0;
}


