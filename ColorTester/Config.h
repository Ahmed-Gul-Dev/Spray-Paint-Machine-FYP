#include <EEPROM.h>

int address = 0;
//#define NUMBER_OF_SHIFT_CHIPS   1
//#define DATA_WIDTH   NUMBER_OF_SHIFT_CHIPS * 8

// LED Indicator
#define led A5

// ---> ON 74165 IC
const int IRSensor = 2;
const int CheckColor = 10;
const int EndL = 8;
const int StartL = 9;

//74165 PISO Shift Register
//const int  LOAD = 11; // PL/LOAD pin 1
//const int  DATA = 12; //  Q7/SERIAL OUT pin 7 (MISO)
//const int  CLOCK = 13; // CP/SCK pin 2


//24V Outputs
const int  out1 = A0; // OUT 1
const int  out2 = A1; // OUT 2
const int  IRstatus = A2; // IR STATUS

// TCS230 or TCS3200 pins wiring to Arduino
#define S1 4
#define S0 3
#define S2 5
#define S3 6
#define sensorOut 7

// Relay Module - H-Bridge
#define r1 A3
#define r2 A4

bool start_machine = true;


void initPins();
uint8_t isrReadRegister();
int isrDigitalRead(uint8_t pin);
int readRGB();
bool checkColor();
void FWD();
void REV();
void STOP();
void saveColor(int val);
void Teach();
void initPusher();
void IR_Test();
void initColorSensor();

void saveColor(int val) {
  Serial.println("Saving RGB Color.");
  EEPROM.write(address, 0);
  delay(250);
  EEPROM.update(address, val);
}

void Teach() {
  Serial.println("Performing Teach Process.");
  IR_Test();
  int readColor = readRGB();
  delay(100);
  saveColor(readColor);
  delay(500);
}

void initPusher() {
  if (isrDigitalRead(1) == 1) {
    REV();
    while (isrDigitalRead(1) == 1) {
      if (isrDigitalRead(0) == 0) {
        Serial.println("Color Tester Origin Position Reached.");
        STOP();
        break;
      }
    }
  } else {
    if (digitalRead(IRSensor) == 1) {
      digitalWrite(IRstatus, LOW);
    }
    Serial.println("Already at Origin");
  }
}

void IR_Test() {
  if (digitalRead(IRSensor) == 1 && isrDigitalRead(0) == 1) {
    FWD();
    while (digitalRead(IRSensor) == 1) {
      if (isrDigitalRead(0) == 0 || digitalRead(IRSensor) == 0) {
        STOP();
        Serial.println("Color Tester Max Limit Reached.");
        digitalWrite(IRstatus, HIGH);
        break;
      }
    }
  } else {
    Serial.println("Already at Max Position.");
  }
}
int readRGB() { // reads only Red Color
  // Stores frequency read by the photodiodes
  int redFrequency = 0;
  // Setting RED (R) filtered photodiodes to be read
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  delay(1000);

//  for (int i = 0; i < 10; i++) {
    // Reading the output frequency
    redFrequency = pulseIn(sensorOut, LOW);
//    redFrequency += redFrequency;
//    // Printing the RED (R) value
//    delay(50);
//  }
//  redFrequency = redFrequency / 10;
  Serial.print("Desired Color = ");
  Serial.println(redFrequency);
  return redFrequency;
}

bool checkColor() {
  Serial.print("Check Color Method Called.");
  byte value = 0;
  value = EEPROM.read(address);
  Serial.print("Saved EEPROM Color : ");
  Serial.println(value, DEC);
  int readColor = readRGB();
  delay(250);
  if (readColor == value) {
    return true;
  } else {
    return false;
  }
}

void FWD() {
  digitalWrite(r1, LOW);
  digitalWrite(r2, HIGH);
}

void REV() {
  digitalWrite(r1, HIGH);
  digitalWrite(r2, LOW);
}

void STOP() {
  digitalWrite(r1, HIGH);
  digitalWrite(r2, HIGH);
}

void initColorSensor() {
  // Setting the outputs
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  // Setting the sensorOut as an input
  pinMode(sensorOut, INPUT);

  // Setting frequency scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
}

void initPins() {
  pinMode(r1, OUTPUT);
  digitalWrite(r1, HIGH);
  pinMode(r2, OUTPUT);
  digitalWrite(r2, HIGH);
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);

  pinMode(out1, OUTPUT);
  digitalWrite(out1, LOW);
  pinMode(out2, OUTPUT);
  digitalWrite(out2, LOW);
  pinMode(IRstatus, OUTPUT);
  digitalWrite(IRstatus, LOW);

  pinMode(IRSensor, INPUT);
  pinMode(CheckColor, INPUT);
  pinMode(StartL, INPUT);
  pinMode(EndL, INPUT);

    // Initialize 74HC165 connections
  
    //  pinMode(LOAD, OUTPUT);
    //  pinMode(CLOCK, OUTPUT);
    //  pinMode(DATA, INPUT);
    //
    //  digitalWrite(LOAD, LOW);
    //  digitalWrite(CLOCK, LOW);
}

void debug() {
  for (int i = 0; i < 8; i++) {
    Serial.print(isrDigitalRead(i));
    Serial.print("\t");
  }
  Serial.println();
}

//uint8_t isrReadRegister() {
//  uint8_t SerialOut = 0;
//  digitalWrite(CLOCK, HIGH);  // preset clock to retrieve first bit
//  digitalWrite(LOAD, HIGH);  // disable input latching and enable shifting
//  SerialOut = shiftIn(DATA, CLOCK, MSBFIRST);  // capture input values
//  digitalWrite(LOAD, LOW);  // disable shifting and enable input latching
//  return SerialOut;
//}
//
//int isrDigitalRead(uint8_t pin) {
//  return bitRead(isrReadRegister(), pin);
//}
