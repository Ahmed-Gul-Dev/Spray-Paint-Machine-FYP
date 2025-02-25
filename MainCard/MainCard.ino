#include <Servo.h>
Servo myservo;

// Relay Module (Y-Axis)
#define r1  5
#define r2  4

// Relay Module (X-Axis)
#define r3  9
#define r4  10

//24V Outputs
const int  setrst = A2; // set/rst
const int  uplimit = 7; // up limit
const int  downlimit = 6; // down limit
const int  leftlimit = 8; // left limit
const int  rightlimit = 11; // right limit
const int  button = 12; // Start Button

const int  out1 = A0; // From Color Tester
const int  out2 = A3; // From Color Tester

// ***************************************************************************************************
void setup() {
  Serial.begin(9600);
  Serial.println("Initializing...............");
  initPins();
  Serial.println("OK.");

  myservo.attach(3);
  myservo.write(90);
  delay(1000);
  myservo.write(0);
  delay(1000);

  initY();
  initX();
  //  EndY();
}

bool commandReceived = false;
int endcount = 0;
void loop() {
  //  debug();
  if (digitalRead(button) == 0) {
    delay(2000);
    EndY();
    commandReceived = false;
    digitalWrite(setrst, HIGH);
    delay(3000);
    digitalWrite(setrst, LOW);
    while (commandReceived == false) {
      if (analogRead(out1) > 512 && analogRead(out2) < 512) {
        digitalWrite(13, HIGH);
        commandReceived = true;
        Serial.println("OK Received");
        RIGHT();
        endcount++;
      } else if (analogRead(out1) < 512 && analogRead(out2) > 512) {
        digitalWrite(13, LOW);
        commandReceived = true;
        Serial.println("Not OK");
        myservo.write(90);
        initY();
        myservo.write(0);
        delay(1000);
        RIGHT();
        endcount++;
      }
    }
  } else if (digitalRead(button) == 1) {
    digitalWrite(setrst, LOW);
    STOP();
  }
}

// ************************************************************************************************************************
void RIGHT() {
  if (endcount < 8) {
    digitalWrite(r3, HIGH);
    digitalWrite(r4, LOW);
    delay(2500);
    digitalWrite(r3, HIGH);
    digitalWrite(r4, HIGH);
  }
}

void EndY() {
  if (digitalRead(uplimit) == 1) {
    UP();
    while (digitalRead(uplimit) == 1) {
      if (digitalRead(uplimit) == 0) {
        Serial.println("Y Axis End Position Reached.");
        STOP();
        break;
      }
      delay(5);
    }
    STOP();
  } else {
    Serial.println("Already at Y Axis End");
  }
}

void initY() {
  if (digitalRead(downlimit) == 1) {
    DOWN();
    while (digitalRead(downlimit) == 1) {
      if (digitalRead(downlimit) == 0) {
        Serial.println("Y Axis Origin Position Reached.");
        STOP();
        break;
      }
    }
  } else {
    Serial.println("Already at Origin Y");
  }
}

void initX() {
  if (digitalRead(leftlimit) == 1) {
    LEFT();
    while (digitalRead(leftlimit) == 1) {
      if (digitalRead(leftlimit) == 0) {
        Serial.println("X Axis Origin Position Reached.");
        STOP();
        break;
      }
    }
  } else {
    Serial.println("Already at Origin X");
  }
}

void LEFT() {
  digitalWrite(r3, LOW);
  digitalWrite(r4, HIGH);
}

void STOP() {
  digitalWrite(r1, HIGH);
  digitalWrite(r2, HIGH);
  digitalWrite(r3, HIGH);
  digitalWrite(r4, HIGH);
}

void DOWN() {
  digitalWrite(r1, LOW);
  digitalWrite(r2, HIGH);
}

void UP() {
  digitalWrite(r1, HIGH);
  digitalWrite(r2, LOW);
}

void initPins() {
  pinMode(r1, OUTPUT);
  digitalWrite(r1, HIGH);
  pinMode(r2, OUTPUT);
  digitalWrite(r2, HIGH);
  pinMode(r3, OUTPUT);
  digitalWrite(r3, HIGH);
  pinMode(r4, OUTPUT);
  digitalWrite(r4, HIGH);

  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  pinMode(setrst, OUTPUT);
  digitalWrite(setrst, LOW);
  pinMode(A1, OUTPUT);
  digitalWrite(A1, LOW);
  pinMode(uplimit, INPUT_PULLUP);
  pinMode(downlimit, INPUT_PULLUP);
  pinMode(leftlimit, INPUT);
  pinMode(rightlimit, INPUT);
  pinMode(button, INPUT);
  pinMode(out1, INPUT);
  pinMode(out2, INPUT);
}

void debug() {
  Serial.print(digitalRead(uplimit));
  Serial.print("\t");
  Serial.print(digitalRead(downlimit));
  Serial.print("\t");
  Serial.print(digitalRead(leftlimit));
  Serial.print("\t");
  Serial.print(digitalRead(rightlimit));
  Serial.print("\t");
  Serial.print(digitalRead(button));
  Serial.print("\t");
  Serial.print(digitalRead(out1));
  Serial.print("\t");
  Serial.println(digitalRead(out2));
  delay(250);
}
