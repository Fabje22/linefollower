#include <SoftwareSerial.h>

SoftwareSerial mySerial(1, 0); // RX, TX nog aan te passen van welke pinnen ik zal nemen voor tx en rx

void setup() {
  
  Serial1.begin(115200);
  Serial.begin(115200);
  Serial.println("Ready");
  
}


  void loop()
{
  
  if (Serial.available() > 0) 
  {
    String teststr = Serial.readString();  //lezen tot een timeout
    teststr.trim();                        // verwijderen witte stukken
    Serial1.println(teststr);
  }


  if (Serial1.available())
  {
  Serial.write(Serial1.read());
  }
  
}
}
