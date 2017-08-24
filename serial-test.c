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

  fprintf(output,"%c",251);
    fprintf(output,"%c",1);
    fprintf(output,"%c",77);
    fprintf(output,"%c",0x00);
    fflush(output); 

  //fscanf(input, "&#37;s",text);
  // fscanf(input, "%s",  text);
  // fscanf(input, "%s",  text);
  // fscanf(input, "%s",  text);
  // fscanf(input, "%s",  text);
  // fscanf(input, "%s",  text);
  // fscanf(input, text);
  // fscanf(input, text);
  int index = 0;
  
  while(true)
  {
    char ch = getc (input);

    if(count < (cpMillis())/5000)
    {
    fprintf(output,"%c",251);
    fprintf(output,"%c",count);
    fprintf(output,"%c",77);
    fprintf(output,"%c",0x00);
    fflush(output); 
    count++;
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