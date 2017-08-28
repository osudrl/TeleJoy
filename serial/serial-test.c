#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "cpTime.h"
#include <pthread.h>
#include "../joy/jt-constants.h"

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

int16_t sourceInts[tele_DATA_COUNT];
uint8_t printBuffer[tele_MAX_BUF];
FILE* output;
int build_escaped_buffer(int16_t* source, uint8_t* result)
{
  int buildIndex = 0;
  result[buildIndex++] = USB_ESCAPE_BYTE;
  result[buildIndex++] = USB_HEADER_BYTE;
  for(int i = 0; i < tele_DATA_COUNT; i++)
  {
    uint8_t lsb = (uint8_t) (source[i] & 0x00ff);
    uint8_t msb = (uint8_t) ((source[i] & 0xff00) >> 8);
    if(lsb == USB_ESCAPE_BYTE)
    {
      result[buildIndex++] = USB_ESCAPE_BYTE;
      result[buildIndex++] = USB_ESCAPE_BYTE;
    }
    else
      result[buildIndex++] = lsb;

    if(msb == USB_ESCAPE_BYTE)
    {
      result[buildIndex++] = USB_ESCAPE_BYTE;
      result[buildIndex++] = USB_ESCAPE_BYTE;
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
  
  output = fopen("/dev/ttyACM1", "w");     //open the terminal screen
  if ( output == NULL )
  {
    printf("ACM1 is NULL, trying ACM0.\n");
    output = fopen("/dev/ttyACM0", "w");
    if (output == NULL)
    {
      printf("ACM0 is also NULL. Exiting.\n");
      return 1;
    }
  }
  cpSleep(2000);
  while(true)
  {
    for(int i = 0; i < tele_DATA_COUNT; i+=1 )
    {
      if (i == 7)
        sourceInts[i] = 1010;
      else if (i == 9)
        sourceInts[i] = (cpMillis()+1500)/3000;
     else if (i == 11)
        sourceInts[i] = cpMillis()/10000;
      else
      sourceInts[i] = ((cpMillis()+(i*100)) / 1000);
    }
    int howMany = build_escaped_buffer(sourceInts,printBuffer);
    sendBuffer(printBuffer, howMany);
    cpSleep(15);
  }
  fclose(output);
}

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