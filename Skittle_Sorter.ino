//  Skittle_sorter

//  James & John Rinkel
//  7/15/2023

//  A program to sort Skittles by color
//  and send the color to a second Arduino for display on an LCD

#include<Servo.h>

//Define the pins of the color sensor
#define s0  2   //S0  pin of the sensor on Arduino pin#2 
#define s1  3   //S1  pin of the sensor on Arduino pin#3 
#define s2  4   //S2  pin of the sensor on Arduino pin#4 
#define s3  5   //S3  pin of the sensor on Arduino pin#5
#define sOut 6  //Out pin of the sensor on Arduino pin#6 

// define the number of scans to average out the readings from the sensor
#define scanCnt 4   // the number of scans averaged together MUST be a power of 2 as in 2/4/8/16
#define bitShift 2  // the number of bits to shift to divide the results and aget the average 1/2/3/4

//Declaring Servos
Servo feedingServo;
Servo decisionServo;

//Declaring general delays
#define smallDelay       5
#define mediumDelay     30
#define bigDelay      1000
#define veryBigDelay  2000

//
//  FILL THIS SET OF VALUES WITH THE NUMBERS FROM THE
//  Skittle_Servo_Setup
//
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

//
//  FILL THIS SET OF VALUES WITH THE NUMBERS FROM THE
//  Skittle_Color_Calibration
//
 
// initializing range array for checking values
// redLowrange, redHighrange, greenLowrange, greenHighrange, blueLowrange, blueHighrange, clearLowrange, clearHighrange

byte colorValues[5][8] = {
         { 99, 172, 84,  91, 38, 39,  99, 137},  // Red
         { 99, 146, 80,  87, 33, 34,  99, 111},  // Yellow
         { 99, 147, 84,  88, 36, 37,  99, 119},  // Green
         { 70,  85, 72,  84, 25, 31,  95, 114},  // Orange
         { 49, 173, 82,  91, 37, 40,  99, 138}   // Purple
         };

// initializing skittle color name array
char *skittleName[5] = {"Red", "Yellow", "Green", "Orange", "Purple"};

// initializing sensor color name array
char *sensorName[4] = {"Red", "Green", "Blue", "Clear"};

//  array to save the sensor results
byte  sensorValue[4];  //Red, Green, Blue, Clear

// Variable to save the color
int identifiedColor = 0;
int tempColor = 0;
int zColor = 0;

// variable to keep track of range matches
byte  rangeMatchCount = 0;

// variable to keep track of rangeColor we are checking
byte  rangeColor = 0;

// variables for values to control the sensor
byte pinVal1 = 0;
byte pinVal2 = 0;

// prototyping functions
void scanTheColor();
void identifyTheColor();
bool checkEachColor();
bool checkRange();

void setup()
  {
//Set the pins of the Color Sensor
  pinMode (s0,OUTPUT);
  pinMode (s1,OUTPUT);
  pinMode (s2,OUTPUT);
  pinMode (s3,OUTPUT);
  pinMode (sOut,INPUT);

//  The pins S0 & S1 used for frequency scaling
//    S0  S1
//    L - L = Power Down
//    L - H = 2%
//    H - L = 20%
//    H - H = 100%
//
  
//Setting frequency
  digitalWrite(s0, HIGH);
  digitalWrite(s1, LOW);

//Attaching the Servos
  feedingServo.attach (feedingServoPin);
  decisionServo.attach(decisionServoPin);

//Set the serial communication in bytes per second for the serial 
//monitor and the link to the second arduino
  Serial.begin(9600);

// position the feeder and decision servos initially

// feeder servo half way between pos1 and pos2
  byte tempPos = (((pos1FeederServo - pos2FeederServo) / 2) + pos2FeederServo);
  feedingServo.write (tempPos);
  
// decision servo at the green bucket
  decisionServo.write (decisionServo_GREEN);

// wait 15 seconds to allow loading of skittles
  Serial.println("waiting for setup of skittles");
  for (int x=1; x<=15; x++)
    {
      delay(bigDelay);
      Serial.print(".");
    }
    Serial.println();
}

void loop()
  {
// show moving feeder servo
//>>>  Serial.println("moving feeder to sensor position");
    
//Write "steps" of 1 degree to the servo until pos2 (Sensor position)
  for (int i = pos1FeederServo; i > pos2FeederServo ; i--)
    {
    feedingServo.write (i);
    delay (mediumDelay);
    }

//Delay to stabilize readings
  delay(bigDelay);

//  scan the color using the sensor to get red, blue, green & clear values
//  and store tham in the global array sensorValue
  scanTheColor();

//  Use the colorValues table to identify the color
  identifyTheColor();
  
//Switch Case to decide which color are we reading
  switch (identifiedColor)
  {    
//Case for Red
  case 1:
//>>>    Serial.println("moving decision servo to the red bucket");
    decisionServo.write (decisionServo_RED);    // move the slide to the red bucket
    Serial.write(identifiedColor);              // send the color to the second Arduino
    break;

//Case for Yellow
  case 2:
//>>>    Serial.println("moving decision servo to the yellow bucket");
    decisionServo.write (decisionServo_YELLOW);
    Serial.write(identifiedColor);
    break;

//Case for Green
  case 3:
//>>>    Serial.println("moving decision servo to the green bucket");
    decisionServo.write (decisionServo_GREEN);
    Serial.write(identifiedColor);
    break;

//Case for Orange
  case 4:
//>>>    Serial.println("moving decision servo to the orange bucket");
    decisionServo.write (decisionServo_ORANGE);
    Serial.write(identifiedColor);
    break;

//Case for Purple
  case 5:
//>>>    Serial.println("moving decision servo to the purple bucket");
    decisionServo.write (decisionServo_PURPLE);
    Serial.write(identifiedColor);
    break;

//Case DEFAULT
  default:
    Serial.println("Did not identify any color, dumping in purple bucket");
    Serial.println();
    decisionServo.write (decisionServo_PURPLE);
  }

//Delay before moving to exit position
  delay(bigDelay);

// Move Feeding servo to exit (drop position)
//>>>  Serial.println("moving feeding servo to exit position");
  
  for (int i = pos2FeederServo; i > pos3FeederServo; i--)
    {
    feedingServo.write (i);
    delay (mediumDelay);
    }
 
// Delay before returning feeder to the get skittle position
  delay(bigDelay);

// Return the Feeding Servo to initial (get skittle position)
//>>>  Serial.println("moving feeding servo to get another skittle");
  
  for (int i = pos3FeederServo; i < pos1FeederServo ; i++)
    {
    feedingServo.write (i);
    delay (smallDelay);
    }

// reset color
  identifiedColor = 0;

// delay before getting another skittle
  delay (veryBigDelay);
  
  }

/////////////////////////////////////////////////////////////////////////////////////////
//
//  read the sensor to get the red, green, blue & clear values nd store in scannedValue array
//
//////////////////////////////////////////////////////////////////////////////////////////
void  scanTheColor()
  {
    
// red    uses pinVal 0,0
// green  uses pinVal 0,1
// blue   uses pinVal 1,0
// clear  uses pinVal 1,1

//>>>  Serial.println("scanning the color ");

  pinVal1 = 0;
  pinVal2 = 0;
    
  for (byte x = 0; x < 4; x++)  // read the four values RED/GREEN/BLUE/CLEAR
    {
//>>>    Serial.print("pinVal1=");
//>>>    Serial.print(pinVal1);
//>>>    Serial.print("  ");
//>>>    Serial.print("pinVal2=");
//>>>    Serial.print(pinVal2);
//>>>    Serial.print("    ");
      
    digitalWrite(s2, pinVal1);
    digitalWrite(s3, pinVal2);

    tempColor = 0;  // clear temp variable to hold the total of all the sensor values
    zColor = 0;     // clear the variable to hold the sensor value
    
    for (byte y = 0; y < scanCnt; y++)
    {
      zColor = pulseIn(sOut, LOW);
//>>>      Serial.print("Z=");
//>>>      Serial.println(zColor);
      tempColor = tempColor + zColor;
//>>>      Serial.print("Temp=");
//>>>      Serial.println(tempColor);
      delay(smallDelay);
    }

    sensorValue[x] = tempColor >> bitShift;
        
    Serial.print(sensorName[x]);
    Serial.print("=");
    Serial.print(sensorValue[x]);
//>>>    Serial.print(" stored in sensorValue pos ");
//>>>    Serial.print(x);
    Serial.println();

// toggle the bits to get the next value
    pinVal1 = pinVal1 | pinVal2;
    pinVal2 = pinVal2 ^ 1;

    delay(smallDelay);
    }
  }
////////////////////////////////////////////////////////////////////////////////
//
// check to see if the values scanned match a color of skittle
//
////////////////////////////////////////////////////////////////////////////////

void  identifyTheColor()
  {
    for (byte skittleColor=0; skittleColor <= 4; skittleColor++)
    {
      if (checkEachColor(skittleColor))
        {
          identifiedColor = skittleColor + 1; //add one to color for switch-case statement
          Serial.print("identified ");
          Serial.println(skittleName[skittleColor]);
          Serial.println();
          return;
        }
        else
        {
          identifiedColor = 0;
        }
    }
  }

////////////////////////////////////////////////////////////////////////////////////////
//
// checks the skittle color values are within the R,G,B,C range for that skittle color
// row 0=red, 1=yellow, 2=green, 3=orange, 4=purple
//
////////////////////////////////////////////////////////////////////////////////////////

bool checkEachColor(byte row)
  {
    byte lowValue = 0;
    byte highValue = 1;
    rangeMatchCount = 0;  // initialize nbr of range matches
    
    for (rangeColor=0; rangeColor <=3; rangeColor++)  // check each R,B,G,C range
    {
//>>>      Serial.print("Checking ");
//>>>      Serial.print(skittleName[row]);
//>>>      Serial.println(" skittle ");
      
      if (checkRange(row, rangeColor, lowValue, highValue))
        {
          rangeMatchCount++;
        }
      lowValue +=2;
      highValue +=2;
    }
    
//>>>    Serial.print("rangeMatchCount=");
//>>>    Serial.println(rangeMatchCount);
    
    if (rangeMatchCount == 4)
    {
//>>>      Serial.println("returning TRUE from checkEachColor");

      return true;
    }
    else
    {
//>>>      Serial.println("returning FALSE from checkEachColor");
      return false;
    }
  }
///////////////////////////////////////////////////////////////////////////////////
//
// checks the sensor value is within the range for the skittle color being checked
// rangeColor 0=red, 1=green, 2=blue, 3=clear
//
///////////////////////////////////////////////////////////////////////////////////

bool checkRange(byte tableRow, byte scanType, byte lowRange, byte highRange)
  {
//>>>    Serial.print("--checking ");
//>>>    Serial.print(sensorName[scanType]);
//>>>    Serial.print(" sensor value ");
//>>>    Serial.print(sensorValue[scanType]); 
//>>>    Serial.print(" is between ");
//>>>    Serial.print(colorValues[tableRow][lowRange]);
//>>>    Serial.print("-");
//>>>    Serial.print(colorValues[tableRow][highRange]);

    if ((sensorValue[scanType] >= colorValues[tableRow][lowRange]) && (sensorValue[scanType] <= colorValues[tableRow][highRange]))
    {
//>>>        Serial.println(" -- in range ");
        return true;
    } 
    else
    {
//>>>        Serial.println(" -- not in range ");
        return false;
    }
  }
/////////////////////////////////////////////////////////////////
