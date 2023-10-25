# start/stop interrupt proof of concept
minimale hard- en software die de correcte werking van een start/stop drukknop aantoont, gebruik makend van een hardware interrupt
//dit is de code voor switch interrrupt van Fabrice Couck

const byte LED=1;
const byte knop=2;
bool run;


void setup() {
  // put your setup code here, to run once:
pinMode(LED,OUTPUT;)
attachInterrupt(digitalPinToInterrupt(knop), aan, RISING);
}

void loop() {
  // put your main code here, to run repeatedly:
if (run)
{
digitalWrite(LED,HIGH);
delay(500);
digitalWrite(LED, LOW);
delay(500);
}
else digitalWrite(LED,LOW);

}
void aan()
{
run= !run;
 }
