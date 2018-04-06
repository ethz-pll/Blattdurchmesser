/*     Simple Stepper Motor Control Exaple Code
 *
 *  by Dejan Nedelkovski, www.HowToMechatronics.com
 *  http://howtomechatronics.com/tutorials/arduino/how-to-control-stepper-motor-with-a4988-driver-and-arduino/
 *
 *  stepper: 360 deg by 200 steps -> 1.8 deg per step
 *  screw: 8 mm on 10 turns -> 0.8 mm per turn
 *  -> 0.8/200 = 0.004 mm by step
 *
 *  Test show typical accuracy with PM-64 is <= 50um per try.
 *
 * !!! Ablauf: siehe Flussdiagramm Hansjakob !!!
 * !!! Kupplung Spindel - Motor: flexible Kupplung !!!
 *
 * FreeRTOS + stepper motor:
 * - see "Example 6. Combining blocking and non-blocking tasks"
 *   https://www.freertos.org/Documentation/161204_Mastering_the_FreeRTOS_Real_Time_Kernel-A_Hands-On_Tutorial_Guide.pdf
 *
 * Stepper libs:
 * - need; A4988 support, exact step-by-step move, position, non-blocking (FreeRTOS)
 * - nice to have: acceleration
 * - look at https://www.arduino.cc/en/Reference/Stepper (speed, single step - but blocking)
 *           https://github.com/laurb9/StepperDriver (speed, single step, non-blocking)
 *           File > Examples > StepperDriver > BasicStepperDriver also NonBlocking
 *           https://github.com/waspinator/AccelStepper
 * - if not available write own or enhance given one
 *
 * OLED lib:
 * - SSD1306 only: https://github.com/greiman/SSD1306Ascii
 * - SSD1306, SH1106: https://github.com/stanleyhuangyc/MultiLCD
 * - look at https://forum.arduino.cc/index.php?topic=256374.0
 *
 * PCB:
 * - Arduino UNO eagle footprint
 *   https://github.com/adafruit/Adafruit-Eagle-Library
 *
 */

#define SERIAL_OUTPUT  // enable verbose serial output

// defines pins numbers
#define stepPin    3  // stepper motor control STEP
#define dirPin     4  // stepper motor control DIR

#define optoPinA   8  // optical sensor reading (int possible?)
#define optoPinB   2  // optical sensor reading (int possible)
#define swchPinA   9  // mechanical sensor reading (int possible?)
#define swchPinB  10  // mechanical sensor reading (int possible?)

#define distpstp 0.004  // 0.004 mm per step

//------------------------------------------------------------------------------

#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

SSD1306AsciiWire oled;

//------------------------------------------------------------------------------

void setup() {
  // Init/Setup
  // Configure serial interface
#ifdef SERIAL_OUTPUT
  Serial.begin(9600);

  // http://www.instructables.com/id/Arduino-Lesser-Known-Features/
  Serial.println("SOFTWARE:");
  Serial.println(__DATE__); // compilation date
  Serial.println(__TIME__); // compilation time
  String stringOne = String(ARDUINO, DEC);
  Serial.println(stringOne); // arduino ide version
  Serial.println(__VERSION__); // gcc version
  Serial.println(__FILE__); // file compiled
#endif

  // Configure wire (i2c) and oled interfaces
  Wire.begin();
  Wire.setClock(400000L);

  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.setFont(System5x7);
  oled.clear();
//  oled.set2X();
//  oled.set1X();
  oled.println("\"Blattdurchmesser\"");

  // Define/Configure pins as input, output, etc.
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  //pinMode(optoPinB, INPUT);        // use as interrupt?
  pinMode(optoPinB, INPUT_PULLUP);   // pull-up to allow check for sensor existence
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(swchPinA, INPUT_PULLUP);  // pull-up to allow check for sensor existence
  pinMode(swchPinB, INPUT_PULLUP);  // pull-up to allow check for sensor existence

  // System Self-Test
// TODO: also report errors to OLED as human-readable message
  bool selftest = true;
  selftest = selftest && (digitalRead(optoPinA))
                      && (!digitalRead(optoPinB));   // test is optical sensor powered/connected and free?
  selftest = selftest && (digitalRead(swchPinA)
                      && (!digitalRead(swchPinB)));  // test is mechanical sensor powered/connected and free?
// test is stepper powered?
// test is stepper functional?
#ifdef SERIAL_OUTPUT
  Serial.println("RUN:");
#endif
  oled.println("RUN:");
#ifdef SERIAL_OUTPUT
  Serial.print("System Self-Test: ");
#endif
  oled.print("System Self-Test: ");
#ifdef SERIAL_OUTPUT
  Serial.println(selftest);
#endif
  oled.println(selftest);

  // Check current status and decide what to do as next step
  // - ...

  // System Init, Self-Test and ... SUCCESSFUL: blink 3 times
  digitalWrite(LED_BUILTIN, HIGH);  // LED on
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);   // LED off
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);  // LED on
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);   // LED off
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);  // LED on
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);   // LED off

  oled.println("READY...");
}
void loop() {}
/*

  // if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));

  Serial.begin(9600);
  // Sets the two pins as In-/Outputs
  pinMode(stepPin, OUTPUT); 
  pinMode(dirPin, OUTPUT);
  pinMode(optoPinB, INPUT);  // use as interrupt?
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);

  digitalWrite(dirPin,HIGH); // BACK/UP; Enables the motor to move in a particular direction
//  digitalWrite(dirPin,LOW); // AWAY/DOWN; Enables the motor to move in a particular direction
//  const int turns =  random(1, 5);
  const int turns = 1;
  Serial.println(turns);
  Serial.println(turns * 200);
//  // Makes 200 pulses for making one full cycle rotation
//  for(int x = 0; x < (200 * turns); x++) {
  // Makes pulses for visibly moving the sledge in a direction
  for(int x = 0; x < (1000 * turns); x++) {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(500); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(500); 
  }
  delay(1000); // One second delay

/*
  digitalWrite(dirPin,LOW); //Changes the rotations direction
//  digitalWrite(dirPin,HIGH); // Enables the motor to move in a particular direction
  // Makes 400 pulses for making two full cycle rotation
  for(int x = 0; x < 4000; x++) {
    delay(2);  // sound becomes very unpleasant with higher values, but accuracy increases
    if(digitalRead(optoPinB) == HIGH) {
      Serial.println(x);
      Serial.println((turns * 200)-x);
      Serial.println(((turns * 200)-x)*0.004*1000);
      Serial.println("done");
      break;
    }
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(500);
  }
x/

  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
/*  digitalWrite(dirPin,HIGH); // Enables the motor to move in a particular direction
  // Makes 200 pulses for making one full cycle rotation
  for(int x = 0; x < 200; x++) {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(500); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(500); 
  }
  delay(1000); // One second delay
  
  digitalWrite(dirPin,LOW); //Changes the rotations direction
  // Makes 400 pulses for making two full cycle rotation
  for(int x = 0; x < 400; x++) {
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(500);
  }x/
  delay(1000);

  Serial.println(digitalRead(optoPinB));
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}
*/
