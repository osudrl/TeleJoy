void setup() {
  pinMode(13,OUTPUT);
  Serial.begin(9600);
}
int n = 0;
void loop() 
{
  if(Serial.available())
  {
    char c = Serial.read();
    
    Serial.print(c);
    if(c == '\n')
    Serial.print("r:");
    n++;
  }
  digitalWrite(13,n%2);

}
