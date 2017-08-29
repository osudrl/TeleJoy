#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "cpTime.h"
#include <pthread.h>
#include "../joy/jt-constants.h"

#define SERIAL_TEST_COUNTING
// #define SERIAL_TEST_ESCAPING
// #define SERIAL_TEST_SQAURES

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

#elif SERIAL_TEST_ESCAPING

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

int main()
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

int main()
{
  pthread_t sreader;
  pthread_t swriter;
  void*result;

  if (pthread_create(&sreader, NULL, serial_read, NULL) == -1)
    printf("Can't create thread t0");
  if (pthread_create(&swriter, NULL, serial_write, NULL) == -1)
    printf("Can't create thread t1");
  if (pthread_join(sreader, &result) == -1)
    printf("Can't join thread t0");
  if (pthread_join(swriter, &result) == -1)
    printf("Can't join thread t1");

  printf("About to exit??");
return 0;
}