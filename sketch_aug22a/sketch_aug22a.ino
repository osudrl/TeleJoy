void setup() {
  pinMode(13,OUTPUT);
  //Serial.begin(9600);
  
  

}
int num = 0;
void loop() 
{
 if(Serial.available())
 {
  uint8_t byt = Serial.read();
  if(byt >= 0)
  {
  Serial.println(byt,HEX);
  Serial.println("n");
 num++;
  }
 }
 if(num > 10)
 {
  digitalWrite(13,HIGH);
 }
 else
 digitalWrite(13,LOW);

}
