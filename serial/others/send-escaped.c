#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

int main()
{
  FILE* output = fopen("/dev/ttyACM1", "w");

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