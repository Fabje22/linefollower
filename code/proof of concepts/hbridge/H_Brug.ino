//invoegen bibliotheken
#include "SerialCommand.h"
#include <EEPROM.h>
#include "EEPROMAnything.h"
#include <SoftwareSerial.h>

//instellen basiswaarden
#define SerialPort Serial
#define BluetoothPort Serial1
#define Baudrate 115200
#define Ain1 5
#define Ain2 6
#define Bin1 9
#define Bin2 10
#define Sleep  12
SerialCommand sCmd(BluetoothPort);
SoftwareSerial Bluetooth(1, 0);

//gebruikte globale parameters
unsigned long previous;
unsigned long calculationTime;
int powerLeft;
int powerRight;
bool start;

//parameters opgeslagen in EEPROM
struct param_t
{
  unsigned long cycleTime;
  int calibrateBlack[6];
  int calibrateWhite[6];
  float kp; //niet kleiner dan 0
  float diff; //tussen 0 en 1
} params;

void setup()
{
  SerialPort.begin(Baudrate);
  BluetoothPort.begin(Baudrate);
  start = false;
  pinMode(Sleep, OUTPUT);
  digitalWrite(Sleep, HIGH);

  //onnodig?
  pinMode(Ain1, OUTPUT);
  pinMode(Ain2, OUTPUT);
  pinMode(Bin1, OUTPUT);
  pinMode(Bin2, OUTPUT);

  //De verschillende commandos
  sCmd.addCommand("set", onSet);
  sCmd.addCommand("debug", onDebug);
  sCmd.addCommand("start", onStart);
  sCmd.addCommand("reset", onReset);
  sCmd.setDefaultHandler(onUnknownCommand);

  EEPROM_readAnything(0, params);

  SerialPort.println("ready");
}

void loop()
{
  sCmd.readSerial();

  unsigned long current = micros();
  if (current - previous >= params.cycleTime)
  {
    previous = current;

    //cyclisch programma adhv van micros ipv millis

    
    //motorsturing bepalen
    if (start == true) {
      //wegschrijven bijsturing
      if (powerRight > 0)
      {
        analogWrite(Ain1, powerRight);
        analogWrite(Ain2, 0);
      }
      else
      {
        powerRight = 0 - powerRight; //abs() gebruiken?
        analogWrite(Ain1, 0);
        analogWrite(Ain2, powerRight);
      }

      if (powerLeft > 0)
      {
        analogWrite(Bin1, powerLeft);
        analogWrite(Bin2, 0);
      }
      else
      {
        powerLeft = 0 - powerLeft; //abs() gebruiken?
        analogWrite(Bin1, 0);
        analogWrite(Bin2, powerLeft);
      }
    }
  }

  //berekenen hoelang het duurt om cyclus te doorlopen
  unsigned long difference = micros() - current;
  if (difference > calculationTime) calculationTime = difference;

}

void onUnknownCommand(char *command)
{
  BluetoothPort.print("unknown command: \"");
  BluetoothPort.print(command);
  BluetoothPort.println("\"");
}

void onSet()
{
  char* param = sCmd.next();
  char* value = sCmd.next();

  if (strcmp(param, "cycle") == 0)
  {
    params.cycleTime = atol(value);
  }
  else if (strcmp(param, "left") == 0)
  {
    params.kp = atof(value);
  }
  else if (strcmp(param, "right") == 0)
  {
    params.diff = atof(value);
  }
  EEPROM_writeAnything(0, params);
}

void onDebug()
{
  BluetoothPort.print("cycle time: ");
  BluetoothPort.println(params.cycleTime);

  BluetoothPort.print("Left: ");
  BluetoothPort.println(powerLeft);

  BluetoothPort.print("Right: ");
  BluetoothPort.println(powerRight);

  BluetoothPort.print("calculation time: ");
  BluetoothPort.println(calculationTime);
  calculationTime = 0;

}

void onStart()
{
  if (start == true) {
    start = false;
    analogWrite(Ain1, 0);
    analogWrite(Ain2, 0);
    analogWrite(Bin1, 0);
    analogWrite(Bin2, 0);
    BluetoothPort.println("Stopped");
  }
  else
  {
    start = true;
    BluetoothPort.println("Started");
  }
}

void onReset()
{
  powerLeft = 0;
  powerRight = 0;
  params.cycleTime = 3000;

  EEPROM_writeAnything(0, params);

  BluetoothPort.println("Reset done");

}
