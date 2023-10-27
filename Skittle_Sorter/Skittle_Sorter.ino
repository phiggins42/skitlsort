#include <Servo.h>

//  Skittle_sorter

//  James & John Rinkel
//  7/15/2023

//  A program to sort Skittles by color
//  and send the color to a second Arduino for display on an LCD

//Define the pins of the color sensor
#define s0  2   //S0  pin of the sensor on Arduino pin#2 
#define s1  3   //S1  pin of the sensor on Arduino pin#3 
#define s2  4   //S2  pin of the sensor on Arduino pin#4 
#define s3  5   //S3  pin of the sensor on Arduino pin#5
#define sOut 6  //Out pin of the sensor on Arduino pin#6 

// Declaring Servos
Servo feedingServo;
Servo decisionServo;

// Declaring general delays
#define smallDelay       5
#define mediumDelay     30
#define bigDelay      1000
#define veryBigDelay  2000
#define startupDelay    5

// defining position of the feedingServo
#define pos1FeederServo 168 // get skittle position
#define pos2FeederServo 104 // scan skittle position
#define pos3FeederServo   6  // drop skittle position

// defining position of the decisionServo
#define decisionServo_RED       4   // red skittle bucket position
#define decisionServo_YELLOW   33   // yellow skittle bucket position
#define decisionServo_GREEN    58   // green skittle bucket position
#define decisionServo_ORANGE   84   // orange skkittle bucket position
#define decisionServo_PURPLE  109   // purple skittle bucket position

// define servi pins
#define feedingServoPin 9
#define decisionServoPin 11

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

// used to store self calibrated color what color
int colorFrequencies[3][5] = { 
         { "red", LOW, LOW, 1000, 0 },
         { "green", HIGH, HIGH, 1000, 0 },
         { "blue", LOW, HIGH, 1000, 0 },
};

// prototyping functions
String identifyTheColor();
int scanTheColor(int colorIndex);

void setup() {
  //Set the pins of the Color Sensor
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);

  // Setting the sensorOut as an input
  pinMode(sOut, INPUT);

  // Setting frequency scaling to 20% per docs
  digitalWrite(s0, HIGH);
  digitalWrite(s1, LOW);

  //Attaching the Servos
  feedingServo.attach(feedingServoPin);
  decisionServo.attach(decisionServoPin);

  //Set the serial communication in bytes per second for the serial 
  //monitor and the link to the second arduino
  Serial.begin(9600);

  // position the feeder and decision servos initially
  // feeder servo half way between pos1 and pos2
  feedingServo.write(((pos1FeederServo - pos2FeederServo) / 2) + pos2FeederServo); 
  decisionServo.write(decisionServo_GREEN);

  // wait 15 seconds to allow loading of skittles
  Serial.println("Waiting for setup of skittles");

  // TODO - this phase could be used to self calibrate - the idea is that the rotating disc would have black, white strips for it to scan!
  for (int x=1; x<=startupDelay; x++) {
    delay(bigDelay);
    Serial.print(".");
  }
  Serial.println();
}

void loop() {
  // Write "steps" of 1 degree to the servo until pos2 (Sensor position)
  for (int i = pos1FeederServo; i > pos2FeederServo ; i--) {
    feedingServo.write(i);
    delay(mediumDelay);
  }

  //Delay to stabilize readings
  delay(bigDelay);

  // scan the color using the sensor to get red, blue, green & clear values
  String color = identifyTheColor();
  Serial.print(" color=");
  Serial.println(color);

  // I guess switch statements don't work with colors
  // TODO - this should be an array lookup for the servo position
  if (color == "red") {
    decisionServo.write (decisionServo_RED);    // move the slide to the red bucket
  } else if (color == "yellow") {
    decisionServo.write (decisionServo_YELLOW);
  } else if (color == "green") {
    decisionServo.write (decisionServo_GREEN);
  } else if (color == "orange") {
    decisionServo.write (decisionServo_ORANGE);
  } else if (color == "purple") {
    decisionServo.write (decisionServo_PURPLE);
  } else {
    Serial.println("Did not identify any color, dumping in purple bucket");
    Serial.println();
    decisionServo.write (decisionServo_PURPLE);
  }

  // Delay before moving to exit position
  delay(bigDelay);

  // Move Feeding servo to exit (drop position)  
  for (int i = pos2FeederServo; i > pos3FeederServo; i--) {
    feedingServo.write(i);
    delay(mediumDelay);
  }
 
  // Delay before returning feeder to the get skittle position
  delay(bigDelay);

  // Return the Feeding Servo to initial (get skittle position)
  for (int i = pos3FeederServo; i < pos1FeederServo ; i++) {
    feedingServo.write(i);
    delay (smallDelay);
  }

  // delay before getting another skittle
  delay (veryBigDelay);  
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  read the sensor to get the red, green, blue & clear values nd store in scannedValue array
//
//////////////////////////////////////////////////////////////////////////////////////////

int scanTheColor(int colorIndex) {
  // set photodiodes to correct low/high level
  digitalWrite(s2, colorFrequencies[colorIndex][1]);
  digitalWrite(s3, colorFrequencies[colorIndex][2]);
  
  // Reading the output frequency
  int colorFrequency = pulseIn(sOut, LOW);

  // self calibrate for incoming color
  colorFrequencies[colorIndex][3] = min(colorFrequency, colorFrequencies[colorIndex][3]);
  colorFrequencies[colorIndex][4] = max(colorFrequency, colorFrequencies[colorIndex][4]);

  // Remaping the value of the RED (R) frequency from 0 to 255
  // You must replace with your own values. Here's an example: 
  // check out https://randomnerdtutorials.com/arduino-color-sensor-tcs230-tcs3200/ for how map() is used
  int theColor = map(colorFrequency, colorFrequencies[colorIndex][3], colorFrequencies[colorIndex][4], 255, 0);

  // Printing the RED (R) value
  Serial.print(colorFrequencies[colorIndex][0]);
  Serial.print("=");
  Serial.print(colorFrequency);
  Serial.print("/");
  Serial.print(theColor);
  Serial.print(" ");

  delay(100);
  return theColor;
}

////////////////////////////////////////////////////////////////////////////////
//
// check to see if the values scanned match a color of skittle
//
////////////////////////////////////////////////////////////////////////////////

String identifyTheColor() {
  int r = scanTheColor(0); // red
  int g = scanTheColor(1); // green
  int b = scanTheColor(2); // blue

  const int distinctRGB[22][3] = {{255, 255, 255},{0,0,0},{128,0,0},{255,0,0},{255, 200, 220},{170, 110, 40},{255, 150, 0},{255, 215, 180},{128, 128, 0},{255, 235, 0},{255, 250, 200},{190, 255, 0},{0, 190, 0},{170, 255, 195},{0, 0, 128},{100, 255, 255},{0, 0, 128},{67, 133, 255},{130, 0, 150},{230, 190, 255},{255, 0, 255},{128, 128, 128}};
  const String distinctColors[22] = {"white","black","maroon","red","pink","brown","orange","coral","olive","yellow","beige","lime","green","mint","teal","cyan","navy","blue","purple","lavender","magenta","grey"};

  String colorReturn = "NA";
  int biggestDifference = 1000;
  for (int i = 0; i < 22; i++) {
    if (sqrt(pow(r - distinctRGB[i][0],2) + pow(g - distinctRGB[i][1],2) + pow(b - distinctRGB[i][2],2)) < biggestDifference) {
      colorReturn = distinctColors[i];
      biggestDifference = sqrt(pow(r - distinctRGB[i][0],2) + pow(g - distinctRGB[i][1],2) + pow(b - distinctRGB[i][2],2));
    }
  }
  return colorReturn;
}