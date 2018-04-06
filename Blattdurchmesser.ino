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

/*
#define MOTOR_DIR   6
#define MOTOR_STEP  7
#define MOTOR_ENA   8
#define optoPin     A0   // may be use both digital and analog pin (for int.)
#define swchPin     9
// OLED on I2C pins SCL, SDA
*/
#define TOL_LOW       -1  // < 0.5 mm tolerance ...
#define TOL_HIGH      -1  //
#define START_BUTTON  -1

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
  pinMode(optoPinA, INPUT_PULLUP);  // pull-up to allow check for sensor existence
  pinMode(optoPinB, INPUT_PULLUP);  // pull-up to allow check for sensor existence
  //pinMode(optoPinB, INPUT);        // use as interrupt?
  pinMode(swchPinA, INPUT_PULLUP);  // pull-up to allow check for sensor existence
  pinMode(swchPinB, INPUT_PULLUP);  // pull-up to allow check for sensor existence
  pinMode(LED_BUILTIN, OUTPUT);

  // System Self-Test
  bool selftest = true;
  selftest = selftest && (digitalRead(optoPinA)
                      && (!digitalRead(optoPinB)));  // test is optical sensor powered/connected and free?
//  selftest = selftest && ((digitalRead(optoPinA) == HIGH)
//                      && (digitalRead(optoPinB) == LOW));  // test is optical sensor powered/connected and free?
  if (!selftest) {
    oled.println("E: opt sw");
  }
  selftest = selftest && (digitalRead(swchPinA)
                      && (!digitalRead(swchPinB)));  // test is mechanical sensor powered/connected and free?
//  selftest = selftest && ((digitalRead(swchPinA) == HIGH)
//                      && (digitalRead(swchPinB) == LOW));  // test is mechanical sensor powered/connected and free?
  if (!selftest) {
    oled.println("E: mech sw");
  }
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
  if (selftest) {  // System Init, Self-Test and ... SUCCESSFUL
    // blink 3 times
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
  } else {         // System Init, Self-Test and ... FAILED
    digitalWrite(LED_BUILTIN, HIGH);  // LED on

    oled.println("ERROR: STOP!");
    while (true) {};  // loop for ever
  }

  delay(3000);

  oled.clear();
// TODO: check how much text fits on to the oled display (~12 chars per line?)
  oled.println("Remove dicing blade");
  oled.println("and press \"Start\"");
}

void loop() {
  measurement();

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


  digitalWrite(LED_BUILTIN, LOW);
*/
}

/*void loop() {
  digitalWrite(dirPin,HIGH); // Enables the motor to move in a particular direction
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

}*/

void measurement() {
  init_measurement();
  while (!run_measurement()) {};
}

void init_measurement() {
  // wait for "Start" button (has pull-up on port)
// TODO: #define START_BUTTON
// TODO: add "Start" and "Reset" buttons
  while (digitalRead(START_BUTTON) == HIGH) {};

  oled.clear();
  oled.println("Init in Progress");
  oled.println("Please wait...");

return;

// TODO: Besprechen mit HJ; kann nach meiner init Prozedur gar nicht sein
//     + -> Fehler tritt davor auf in setup() bei Init auf
//   ! - -> kann NICHT selbst versuchen durch -Fahrt Fehler zu beheben
  if (digitalRead(swchPinB) == HIGH) {    // mech sw pressed?
    // move 5mm in -dir
    // ...
    if (digitalRead(swchPinB) == HIGH) {  // mech sw pressed?
      // ERROR: Axis blocked or reference switch defect!
      // Please restart machine or call for assistance!
      while (true) {};  // "Ende"; loop for ever
    }
  }

  // move to reference mech sw in +dir an store position
// TODO: Besprechen mit HJ; "Endschalter: Mechanischer Microswitch NO" -> "NO" ??
  // ...

// TODO: Besprechen mit HJ; kann nach meiner init Prozedur gar nicht sein
//     + -> Fehler tritt davor auf in setup() bei Init auf
//   ! - -> kann NICHT selbst versuchen durch -Fahrt Fehler zu beheben
// TODO: Besprechen mit HJ; STOP == Ende?
// TODO: Besprechen mit HJ; Wie kann Min/Max des optischen sensors gemessen werden?
// TODO: Consider using analog reading to increase photo sensor precission!!
  if (digitalRead(optoPinB) == HIGH) {    // opt sw triggered?
//  if (analogRead(optoPinB) >= 614) {      // opt sw triggered? 60% of 1023
    // ERROR: Photo sensor blocked or defect!
    // Please restart machine or call for assistance!
    while (true) {};    // "STOP"; loop for ever
  }

  // move to reference opt sw in -dir and store position
  // ...
  float dist;
  if (!((TOL_LOW < dist) && (dist < TOL_HIGH))) {
    // ERROR: Measurement area blocked!
    // Remove all parts and press \"Start\"
    return;
  }

  // move to reference opt sw in -dir and store position
  // for a 2nd time (5 mm)
// TODO: tolerance for difference of reference values? compare reference values?
// TODO: Besprechen mit HJ; macht das Sinn?
//   ? - -> wie sollte denn auf Fehler reagiert werden?
  // ...

  // 2 values!
  if (!((TOL_LOW < dist) && (dist < TOL_HIGH))) {
    // ERROR: Optical switch not in tolerance!
    // Initialize again by pressing \"Start\"
    return;
  }

  // zero axis - position tracking
// TODO: how precise is position tracking?
  // ...

  // fast move in +dir to waiting position (~1 mm before mech. sw.)
// TODO: stop closer to dicing blade? (see next step also)
  // ...

// TODO: Besprechen mit HJ; kann je nach waiting position im Schritt vorher gar nicht sein
//   +/- -> Fehler tritt nie auf
//   ? - -> wie sollte denn auf Fehler reagiert werden?
  if (digitalRead(swchPinB) == HIGH) {    // mech sw pressed?
    // ERROR: End switch pressed!
    // Initialize again by pressing \"Start\"
    return;
  }

  oled.clear();
  oled.println("Ready: Place dicing blade");
  oled.println("and press \"Start\"");

// TODO: "DB" with pre-selectable dicing blades (diameter, tolerance during use) from SD card, with multi-switch
}

bool run_measurement() {
  // wait for "Start" button (has pull-up on port)
  while (digitalRead(START_BUTTON) == HIGH) {};

// TODO: Besprechen mit HJ; kann je nach waiting position im Schritt vorher gar nicht sein
//   +/- -> Fehler tritt nie auf
//   ? - -> wie sollte denn auf Fehler reagiert werden?
  if (digitalRead(swchPinB) == HIGH) {    // mech sw pressed?
    // ERROR: End switch pressed!
    // Initialize again by pressing \"Start\"
    return true;
  }

  // move to reference opt sw in -dir and store position
  // do this 3 times - go back in +dir ~3 mm each time
  // ...

  // 3 values!
  float dist;
  if (!((TOL_LOW < dist) && (dist < TOL_HIGH))) {
    // ERROR: Measurement not in tolerance!
    // Reinsert dicing blade and press \"Start\"

    // fast move in +dir to waiting position (~1 mm before mech. sw.)
    // ...

// TODO: Besprechen mit HJ; kann je nach waiting position im Schritt vorher gar nicht sein
//   +/- -> Fehler tritt nie auf
//   ? - -> wie sollte denn auf Fehler reagiert werden?
    if (digitalRead(swchPinB) == HIGH) {    // mech sw pressed?
      // ERROR: End switch pressed!
      // Initialize again by pressing \"Start\"
      return true;
    } else {
      return false;
    }
  }

  // calculate diameter, see drawing
  float diameter;
  // ...

  oled.print("Diameter: ");
  oled.print(diameter);
  oled.println(" mm");
  oled.println("Press \"Start\" for");
  oled.println("the next measurement");

  // fast move in +dir to waiting position (~1 mm before mech. sw.)
  // ...

// TODO: Besprechen mit HJ; kann je nach waiting position im Schritt vorher gar nicht sein
//   +/- -> Fehler tritt nie auf
//   ? - -> wie sollte denn auf Fehler reagiert werden?
  if (digitalRead(swchPinB) == HIGH) {    // mech sw pressed?
    // ERROR: End switch pressed!
    // Initialize again by pressing \"Start\"
    return true;
  } else {
    return false;
  }
}
