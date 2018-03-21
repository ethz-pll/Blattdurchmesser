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
 *
 * FreeRTOS + stepper motor:
 * - see "Example 6. Combining blocking and non-blocking tasks"
 *   https://www.freertos.org/Documentation/161204_Mastering_the_FreeRTOS_Real_Time_Kernel-A_Hands-On_Tutorial_Guide.pdf
 *
 * Stepper libs:
 * - need; A4988 support, exact step-by-step move, position, non-blocking (FreeRTOS)
 * - nice to have: acceleration
 * - look at https://github.com/laurb9/StepperDriver
 *           https://github.com/waspinator/AccelStepper
 *           https://www.arduino.cc/en/Reference/Stepper
 * - if not available write own or enhance given one
 *
 * PCB:
 * - Arduino UNO eagle footprint
 *   https://github.com/adafruit/Adafruit-Eagle-Library
 *
 */
// defines pins numbers
#define stepPin  3  // stepper motor control
#define dirPin   4  // stepper motor control
#define optoPin  2  // optical sensor reading (int possible)
#define distpstp 0.004  // 0.004 mm per step
 
void setup() {
  // if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));

  Serial.begin(9600);
  // Sets the two pins as In-/Outputs
  pinMode(stepPin, OUTPUT); 
  pinMode(dirPin, OUTPUT);
  pinMode(optoPin, INPUT);  // use as interrupt?
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);

  digitalWrite(dirPin,HIGH); // AWAY/DOWN; Enables the motor to move in a particular direction
//  digitalWrite(dirPin,LOW); // BACK/UP; Enables the motor to move in a particular direction
  const int turns =  random(1, 5);
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
    if(digitalRead(optoPin) == HIGH) {
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
*/

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
  }*/
  delay(1000);

  Serial.println(digitalRead(optoPin));
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}
