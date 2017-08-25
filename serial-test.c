#include <stdio.h>
#include <stdbool.h>
#include "cpTime.h"

int main()
{
  FILE* input;
  FILE* output;
  char text[10000];


  input = fopen("/dev/ttyACM1", "r");      //open the terminal keyboard

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

 int count = 0;
  int index = 0;
  int n = 0;
  int plus = 0;
  long offset = 0;
  while(true)
  {
    char ch = getc (input);

    if(count < (cpMillis()-offset)/400)
    {
      if(count == 0)
        fprintf(output,"%c",0xfe);
      else if(count == 1)
        fprintf(output,"%c",0x88);
      else if (count == 20)
      {
        fprintf(output,"%c",0xfe);
        fprintf(output,"%c",0xfe);
      }
      else if(count % 2 == 0)
      {
        n = ((count-6)/2) -1;
        fprintf(output,"%c",n*n+plus);
        //printf("TESTTT %d\n",n*n);
      }
      else
      {
        fprintf(output,"%c",0);
      }
      fflush(output); 
      count++;
      if(count > 60)
      {
        offset = cpMillis();
        count = 0;
        plus = (cpMillis()/10000);
        //cpSleep(4000);
      }
    }

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


fclose(output);
fclose(input);
  return 0;
}