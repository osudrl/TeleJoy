#include <stdio.h>
#include <stdbool.h>
#include "cpTime.h"
#include <pthread.h>

void* serial_read()
{
  FILE* input;
  char text[10000];
  input = fopen("/dev/ttyACM1", "r"); 
  int index = 0;

  while(cpMillis() < 60000)
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

void* serial_write()
{
  printf("ahi");
  FILE* output;
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
  fprintf(output,"%c",0xfe);
  fprintf(output,"%c",0x88);
  fflush(output); 
  while(cpMillis()<60000)
  {
    ;
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