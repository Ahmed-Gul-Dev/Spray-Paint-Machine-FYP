#define ColorValue 65

//24V Outputs
const int  out1 = A1; // OUT 1
const int  out2 = A2; // OUT 2

// TCS230 or TCS3200 pins wiring to Arduino
#define S1 4
#define S0 3
#define S2 5
#define S3 6
#define sensorOut 7

// Relay Module - H-Bridge
#define r1 A3
#define r2 A4

const int IRSensor = 2;
const int CheckColor = 10;
const int EndL = 8;
const int StartL = 9;

void setup() {
  Serial.begin(9600);
  Serial.println("Initializing...............");
  initPins();
  initColorSensor();
  initPusher();
  Serial.println("OK.");
}

bool start = false;
void loop() {
  //  Serial.println(readRGB());
  //  delay(250);

  if (digitalRead(CheckColor) == 0) {
    digitalWrite(out1, LOW);
    digitalWrite(out2, LOW);
    Serial.println("SET command from Master.");
    while (digitalRead(CheckColor) == 0) { // SET from Master Card
      IR_Test();
      start = true;
      bool result = checkColor();
      initPusher();
      if (result) { // OK --> (Matched)
        Serial.println("OK Sent");
        digitalWrite(out1, HIGH);
        digitalWrite(out2, LOW);
        digitalWrite(13, HIGH);
      } else { // not OK
        Serial.println("Not OK Sent");
        digitalWrite(out1, LOW);
        digitalWrite(out2, HIGH);
        digitalWrite(13, LOW);
      }
    }
  }
  else if (digitalRead(CheckColor) == 1 && start) {
    Serial.println("RST command from Master.");
    initPusher();
    start = false;
  }
}


//void saveColor(int val) {
//  Serial.println("Saving RGB Color.");
//  EEPROM.write(address, 0);
//  delay(250);
//  EEPROM.update(address, val);
//}
//
//void Teach() {
//  Serial.println("Performing Teach Process.");
//  IR_Test();
//  int readColor = readRGB();
//  delay(100);
//  saveColor(readColor);
//  delay(500);
//}

void initPusher() {
  if (digitalRead(StartL) == 1) {
    REV();
    while (digitalRead(StartL) == 1) {
      if (digitalRead(StartL) == 0) {
        Serial.println("Color Tester Origin Position Reached.");
        STOP();
        break;
      }
    }
    STOP();
  } else {
    Serial.println("Already at Origin");
  }
}

void IR_Test() {
  if (digitalRead(IRSensor) == 1 && digitalRead(EndL) == 1) {
    FWD();
    while (digitalRead(IRSensor) == 1) {
      if (digitalRead(EndL) == 0 || digitalRead(IRSensor) == 0) {
        STOP();
        Serial.println("Color Tester Max Limit Reached.");
        break;
      }
    }
    STOP();
  } else {
    Serial.println("Already at Max Position.");
  }
}

int readRGB() { // reads only Red Color
  int redFrequency = 0;
  // Setting RED (R) filtered photodiodes to be read
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  delay(500);

  for (int i = 0; i < 10; i++) {
    // Reading the output frequency
    redFrequency = pulseIn(sensorOut, LOW);
    redFrequency += redFrequency;
    // Printing the RED (R) value
    delay(50);
  }
  redFrequency = redFrequency / 10;
  delay(500);
  Serial.print("Desired Color = ");
  Serial.println(redFrequency);
  return redFrequency;
}

bool checkColor() {
  Serial.print("Check Color Method Called.");
  int readColor = readRGB();
  delay(250);
  if (readColor < ColorValue) {
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

  pinMode(out1, OUTPUT);
  digitalWrite(out1, LOW);
  pinMode(out2, OUTPUT);
  digitalWrite(out2, LOW);

  pinMode(IRSensor, INPUT);
  pinMode(CheckColor, INPUT);
  pinMode(StartL, INPUT);
  pinMode(EndL, INPUT);
}
