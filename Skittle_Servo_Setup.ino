// Skittle_Servo_Setup 

//  James & John Rinkel
//  7/29/2023

//  A program to calibrate the feeding and decision Servos
//  for the Skittle_sorter program

//  Write down these values for the feeder servo in these positions
//  and enter them in the skittle_sorter sketch
//  get skittle position
//  scan skittle position
//  drop skittle position

//  write down these values for the decision servo in these positions
//  and enter them in the skittle_sorter sketch
//  red bucket position
//  yellow bucket position
//  green bucket position
//  orange bucket position
//  purple bucket position

//  We will use 2 Potentiometer to read the Analog Input
//  and display the values
//

#include <Servo.h>

//  Declaring our Servos
Servo feedingServo;
Servo decisionServo;

//  declaring our servo pins
#define feedingServoPin 9
#define decisionServoPin 11

//  Delcaring our Potentiometers
byte potControllerFeeding = A0;
byte potControllerDecision = A2;

int  mappedFeeder = 0;
int  mappedDecision = 0;

int  feedingServoPosition = 0;
int  decisionServoPosition = 0;

void setup()
{
//  Attaching the Servos
  feedingServo.attach (9);
  decisionServo.attach(11);

//  PinMode for our Potentiometers
  pinMode(A0, INPUT);
  pinMode(A2, INPUT);

//  Begin Serial communication
  Serial.begin(9600);

} //Close setup

void loop()
{
//  read and save the servo positions
  feedingServoPosition = analogRead(A0);
  decisionServoPosition = analogRead(A2);

//  Map the analog read (0-1023) to convert it to degrees (0-180)
  mappedFeeder = map(feedingServoPosition, 0, 1023, 0, 180);
  mappedDecision = map(decisionServoPosition, 0, 1023, 0, 180);

//  move the servos to the positions indicated by the potentiometer
  feedingServo.write(mappedFeeder);
  decisionServo.write(mappedDecision);

//  display the current position of each servo
  Serial.print("Feeder servo = ");
  Serial.print(mappedFeeder);
  
  Serial.print("  Decision servo = ");
  Serial.print(mappedDecision);
  Serial.println();

  delay(500);

} //  close loop
/////////////////////////////////////////////////////////////////////
