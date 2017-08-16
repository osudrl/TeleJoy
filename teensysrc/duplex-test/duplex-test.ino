
void setup() 
{
  pinMode(13, OUTPUT);
  pinMode(8,INPUT);
  digitalWrite(13,LOW);
}


int n = 0;
void loop() 
{
  n = digitalRead(8) == HIGH ? 1 : n;
  digitalWrite(13,n);
}

