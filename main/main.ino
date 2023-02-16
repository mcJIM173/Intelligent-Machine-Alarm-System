#include <Stepper.h>
#include <SoftwareSerial.h>

SoftwareSerial Arduino_softSerial(12, 13); //RX, TX

const int step_num = 2048;
Stepper changching_stepper(step_num, 2,4,3,5);

int switch_in = 7;
int isFlamePin = 8;
#define LEDB 9
#define LEDG 10
#define LEDR 11
const int tiltPin = 22;
const int buzzPin = 6;

int isFlame = HIGH;
byte tilt_value;

int r = 0;
int g = 0;
int b = 0;

// Initialize status
int Flame_status = 0;
int Motor_status = 0;
int Power_status = 0;
int Tilt_status  = 0;

void setup() {
  // open serial communication (Arduino -> PC)
  Serial.begin(115200);
  // open serial communication (Arduino -> NodeMCU)
  Arduino_softSerial.begin(115200);
  
  pinMode(tiltPin,    INPUT);
  pinMode(isFlamePin, INPUT);
  pinMode(switch_in,  INPUT);
  pinMode(buzzPin,    OUTPUT);
  pinMode(LEDR,       OUTPUT);
  pinMode(LEDG,       OUTPUT);
  pinMode(LEDB,       OUTPUT);
  changching_stepper.setSpeed(15);
}

void loop() {
  Serial.println("started...");
  isFlame = digitalRead(isFlamePin);
  tilt_value = digitalRead(tiltPin);

  if(digitalRead(switch_in)==HIGH)
  {
    Serial.println("Switch on!");
    Power_status = 1;
    Motor_status = 1;
    
    if (tilt_value == 1)
    {
      Tilt_status = 1;
      Motor_status = 0;
      changching_stepper.step(0);
      digitalWrite(buzzPin, HIGH);
      r=255; g=0; b=0;
      analogWrite(LEDR, r);
      analogWrite(LEDG, g);
      analogWrite(LEDB, b);
      Serial.println("TILT! TILT! TILT!");
      delay(500);
    }
    else
    {
      Motor_status = 1;
      Tilt_status = 0;
      if (isFlame== LOW)
      {
        Flame_status = 1;
        Motor_status = 0;
        changching_stepper.step(0);
        digitalWrite(buzzPin, HIGH);
        r=255; g=0; b=0;
        analogWrite(LEDR, r);
        analogWrite(LEDG, g);
        analogWrite(LEDB, b);
        Serial.println("FLAME! FLAME! FLAME!");
        delay(500);
      }
      else
      {
        changching_stepper.step(15);
        digitalWrite(buzzPin, LOW);
        Flame_status = 0;
        Motor_status = 1;
        r=0; g=255; b=0;
        analogWrite(LEDR, r);
        analogWrite(LEDG, g);
        analogWrite(LEDB, b);
      }
    }
  }
  if(digitalRead(switch_in)==LOW)
  {
    Serial.println("Switch off!");
    changching_stepper.step(0);
    Power_status = 0;
    Motor_status = 0;
    
    // Reset All
    digitalWrite(buzzPin, LOW);
    r=0; g=0; b=0;
    analogWrite(LEDR, r);
    analogWrite(LEDG, g);
    analogWrite(LEDB, b);
    delay(500);
  }

  Arduino_softSerial.print(Flame_status); Arduino_softSerial.print("A");
  Arduino_softSerial.print(Tilt_status);  Arduino_softSerial.print("B");
  Arduino_softSerial.print(Motor_status); Arduino_softSerial.print("C");
  Arduino_softSerial.print(Power_status); Arduino_softSerial.print("D");
  Arduino_softSerial.print("\n");
}
