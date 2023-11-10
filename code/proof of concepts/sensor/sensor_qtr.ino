void setup() {
  Serial.begin(9600);           //  setup serial
}

void loop() {
  
//8kolommen 250 ms
  Serial.print(analogRead(A5));
  Serial.print(" ");
  
  Serial.print(analogRead(A4));
  Serial.print(" ");
  
  Serial.print(analogRead(A3));
  Serial.print(" ");
  
  Serial.print(analogRead(A2));
  Serial.print(" ");
  
  Serial.print(analogRead(A1));
  Serial.print(" ");
  
  Serial.print(analogRead(A9));
  Serial.print(" ");
  
  Serial.print(analogRead(A10));
  Serial.print(" ");
  
  Serial.print(analogRead(A11));


  Serial.println("");
  delay (500);
}
