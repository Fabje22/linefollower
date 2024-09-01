#include "SerialCommand.h"
#include "EEPROMAnything.h"

#define SerialPort Serial
#define Baudrate 9600
/*
const int motor1PWM = 9;   // PWM input for Motor 1
const int motor1IN1 = 8;   // Input 1 for Motor 1
const int motor1IN2 = 7;   // Input 2 for Motor 1

const int motor2PWM = 5;   // PWM input for Motor 2
const int motor2IN1 = 4;   // Input 1 for Motor 2
const int motor2IN2 = 3;   // Input 2 for Motor 2
*/

SerialCommand sCmd(SerialPort);
bool debug;
unsigned long previous, calculationTime;
bool start;
float lastErr;
const int sensor [] = {A5, A4, A3, A2, A1, A0};   

struct param_t
{
  unsigned long cycleTime;
  /* andere parameters die in het eeprom geheugen moeten opgeslagen worden voeg je hier toe ... */
  int black[6];
  int white[6];
  float kp;
  float kd;
  float ki;
  float diff;
  int power;
} params;

void setup()
{
  SerialPort.begin(Baudrate);

  sCmd.addCommand("set", onSet);
  sCmd.addCommand("debug", onDebug);
  sCmd.setDefaultHandler(onUnknownCommand);
  sCmd.addCommand("start", onStart);
  sCmd.addCommand("stop", onStop);

  EEPROM_readAnything(0, params);

  
  SerialPort.println("ready");
  /*

  pinMode(motor1PWM, OUTPUT);
  pinMode(motor1IN1, OUTPUT);
  pinMode(motor1IN2, OUTPUT);

  pinMode(motor2PWM, OUTPUT);
  pinMode(motor2IN1, OUTPUT);
  pinMode(motor2IN2, OUTPUT);
  */
  start = false;
  float lastErr = 0;
  pinMode(8, OUTPUT);  //A1
  pinMode(7, OUTPUT);  //A2 
  pinMode(6, OUTPUT);  //B2
  pinMode(5, OUTPUT); //B1
  pinMode (13,INPUT); // start

  
}

void loop()
{
 sCmd.readSerial();
 
  unsigned long current = micros();
  if (current - previous >= params.cycleTime)
  {
    previous = current;

    /* code die cyclisch moet uitgevoerd worden programmeer je hier ... */
    //motorForward(motor1PWM, motor1IN1, motor1IN2);

  /* Move Motor 2 forward
  motorForward(motor2PWM, motor2IN1, motor2IN2);

 delay(2000);  // Move forward for 2 seconds

   Stop both motors
  motorStop(motor1IN1, motor1IN2);
  motorStop(motor2IN1, motor2IN2);

  delay(1000);  // Pause for 1 second
      
      SerialPort.println("hier");
    }*/
if (digitalRead (12)== HIGH and  start == false ) start = true;
      else if (digitalRead (12)== HIGH and start == true)  start = false;  
      
      
      //
      
      SerialPort.println (start);
      
    long normalised[6];
    
    
    //SerialPort.print("values: ");
    for (int i = 0; i < 6; i++)
    {
      long value = analogRead(sensor[i]);
      //SerialPort.print(value);
      //SerialPort.println(" ");
      normalised[i] = map(value, params.black[i], params.white[i], 0, 1000);
      normalised[i]=constrain(normalised [i],0,1000);
      
    }
     /*SerialPort.print("normalised: ");
      for (int i = 0; i < 6; i++)
        {
            SerialPort.print(normalised[i]);
            SerialPort.println(" ");
        }*/
      

  
    int index = 0;
    for (int i =1; i < 6 ;i++) if( normalised[i] < normalised[index]) index = i;
    
    /*SerialPort.print("index= ");
    SerialPort.println(index);*/
   // if (normalised [index] >= 1000) start = false;
    int index2;
    index2= index;
    if (index ==0) index = 1;
    else if(index ==5) index = 4;
    
    /*SerialPort.print("index= ");
    SerialPort.println(index);*/
    
    long Snul = normalised[index];
    long Smineen = normalised[index-1];
    long Spluseen = normalised[index+1]; 

    /*SerialPort.print("S-1: ");
    SerialPort.println(Smineen);
    SerialPort.print("S0: ");
    SerialPort.println(Snul);
    SerialPort.print("S+1: ");
    SerialPort.println(Spluseen);*/

    float b = (Smineen - Spluseen ) / -2;
    float a = Spluseen - b - Snul;
    if (a == 0) a = 1;
    //SerialPort.print("b: ");
    //SerialPort.println(b);
    //SerialPort.print("a=");
    //SerialPort.println(a); 


    float top = (-b / (2*a) * 10);
    top = top + (index*10-25);
    /*SerialPort.print("top=");
    SerialPort.println(top); */
    
    //if (index == 1 or index == 4) top = constrain(top, -7.50,7.50);
    if (index2 == 0) top = -25;
    if (index2 == 5) top = 25;
    /*SerialPort.print("index=");
    SerialPort.println(index2); 
    SerialPort.print("top=");
    SerialPort.println(top); */  
  
    float error = -top; //error = setpoint - input
    float output = error * params.kp;
    float iTerm;
    if ( error == 0) iTerm=0;
    iTerm += params.ki*error;
    iTerm = constrain(iTerm, -510, 510);
    output += iTerm;
    //if (index2 ==1 or index2==2 or index2 ==3 or index2==4)
    //{
    //if (error == 0) lastErr = 0;
    output += params.kd * (error - lastErr);
    lastErr = error;
    //}
    
    
    output = constrain(output, -510, 510); 
    float powerLeft, powerRight;

    //SerialPort.print("output= ");
    //SerialPort.println(output);

    if (output >= 0)
      {
        powerLeft = constrain(params.power + params.diff * output, -255, 255);
        powerRight = constrain(powerLeft - output, -255, 255);
        powerLeft = powerRight + output;
      }
    else
      {
        powerRight = constrain(params.power - params.diff * output, -255, 255);
        powerLeft = constrain(powerRight + output, -255, 255);
        powerRight = powerLeft - output;
      }
    //if (index==0) (powerRight =255, powerLeft =0);
    // if (index==5) (powerRight =0, powerLeft =255);
    //SerialPort.print("powerLeft=");
    //SerialPort.print(powerLeft); 
    //SerialPort.print("powerRight=");
    //SerialPort.println(powerRight);
    
    if(start==true){
      if (powerLeft >=0)  (analogWrite (3,abs (powerLeft)),digitalWrite (5,LOW));
      if (powerRight >=0)  (analogWrite (10,abs (powerRight)),digitalWrite(9,LOW));
      if (powerLeft <0)  (analogWrite (5,abs (powerLeft)),digitalWrite (3,LOW));
      if (powerRight <0)  (analogWrite (9,abs (powerRight)),digitalWrite(10,LOW));
     /* analogWrite(3, abs(powerLeft));
      digitalWrite(5, LOW);
      analogWrite(10, abs(powerRight));
      digitalWrite(9, LOW); */ 
    }else{
      digitalWrite(3, LOW);
      digitalWrite(5, LOW);
      digitalWrite(9, LOW);
      digitalWrite(10, LOW);
    }

  }
   




  
  unsigned long difference = micros() - current;
  if (difference > calculationTime) calculationTime = difference;
}
/*
void motorStop(int in1, int in2) {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}
void motorForward(int pwm, int in1, int in2) {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(pwm, 255);  // Full speed
}
*/










void onUnknownCommand(char *command)
{
  SerialPort.print("unknown command: \"");
  SerialPort.print(command);
  SerialPort.println("\"");
}

void onSet()
{
  char* param = sCmd.next();
  char* value = sCmd.next();  
  
 if (strcmp(param, "cycle") == 0)
  {
    long newCycleTime = atol(value);
    float ratio = ((float) newCycleTime) / ((float) params.cycleTime);

    params.ki *= ratio;
    params.kd /= ratio;

    params.cycleTime = newCycleTime;
  }
  else if (strcmp(param, "ki") == 0)
  {
    float sampleTimeInSec = ((float) params.cycleTime) / 1000000;
    params.ki = atof(value) * sampleTimeInSec;
  }
  else if (strcmp(param, "kd") == 0)
  {
    float sampleTimeInSec = ((float) params.cycleTime) / 1000000;
    params.kd = atof(value) / sampleTimeInSec;
  }
 
  if (strcmp(param, "power") == 0) params.power = atoi(value);
  if (strcmp(param, "kp") == 0) params.kp = atof(value);
  if (strcmp(param, "diff") == 0) params.diff = atof(value);
 

  
  EEPROM_writeAnything(0, params);
}

void onDebug()
{
  SerialPort.print("cycle time: ");
  SerialPort.println(params.cycleTime);
  
  /* parameters weergeven met behulp van het debug commando doe je hier ... */
  float sampleTimeInSec = ((float) params.cycleTime) / 1000000;
  float ki = params.ki / sampleTimeInSec;
  SerialPort.print("Ki: ");
  SerialPort.println(ki);

  float kd = params.kd * sampleTimeInSec;
  SerialPort.print("Kd: ");
  SerialPort.println(kd);
  SerialPort.print("Power : ");
  SerialPort.println(params.power);
  SerialPort.print("Kp : ");
  SerialPort.println(params.kp);
  SerialPort.print("Diff : ");
  SerialPort.println(params.diff);

  //waarden black
  SerialPort.print("black: ");
  for (int i = 0; i < 6; i++)
  {
      SerialPort.print(params.black[i]);
      SerialPort.print(" ");
  }

  //waarden white
  SerialPort.print("white: ");
  for (int i = 0; i < 6; i++)
  {
      SerialPort.print(params.white[i]);
      SerialPort.print(" ");
  }

  
  SerialPort.print("calculation time: ");
  SerialPort.println(calculationTime);
  calculationTime = 0;
}
void onCalibrate()
{
  char* param = sCmd.next();

  if (strcmp(param, "black") ==0 )
  {
    SerialPort.print("calibrate black: ");
    for (int i = 0; i < 6; i++) params.black[i]=analogRead(sensor[i]);
    SerialPort.println("ready");
  }
  else if (strcmp(param, "white") ==0 )
  {
    SerialPort.print("calibrate white: ");    
    for (int i = 0; i < 6; i++) params.white[i]=analogRead(sensor[i]);  
    SerialPort.println("ready");      
  }

  EEPROM_writeAnything(0, params);
}

void onStart()
{
    start = true; 
}

void onStop()
{
    start = false;
}
