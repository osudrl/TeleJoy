#include <stdio.h>

int main(int argc, char **argv)
{
    FILE *input,*output;
   //char text[10000];
    int i;
   
    //input = fopen("/dev/ttyACM0", "r");      //open the terminal keyboard
    output = fopen("/dev/ttyACM0", "w");     //open the terminal screen
     
    
    fprintf(output, "%c",0xfa );
    fprintf(output, "%c",'\n' );  // works only with end of line
   fprintf(output,"%c",0x09 );
    fprintf(output,"%c",'\n' ); 
    fprintf(output,"%c",0x88 );
    fprintf(output,"%c",'\n' ); 
    fflush(output);
  //  fscanf(input, "&#37;s",text); // worked with fputc(i), + putc('\n')
 
    
    fclose(output);
   // fclose(input);
     
 return 0;
}