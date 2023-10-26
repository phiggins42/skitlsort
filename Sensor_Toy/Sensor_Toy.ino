//  Sensor Toy

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

// define the number of scans to average out the readings from the sensor
#define scanCnt 4   // the number of scans averaged together MUST be a power of 2 as in 2/4/8/16
#define bitShift 2  // the number of bits to shift to divide the results and aget the average 1/2/3/4

//Declaring general delays
#define smallDelay       5
#define mediumDelay     30
#define bigDelay      1000
#define veryBigDelay  2000
#define startupDelay    5

// in theory, we want distinctRBG[n][3], where N is the number of buckets we know
// and then distinctColors[n] as just the postition of the servo for a given color
const int distinctRGB[22][3] = {{255, 255, 255},{0,0,0},{128,0,0},{255,0,0},{255, 200, 220},{170, 110, 40},{255, 150, 0},{255, 215, 180},{128, 128, 0},{255, 235, 0},{255, 250, 200},{190, 255, 0},{0, 190, 0},{170, 255, 195},{0, 0, 128},{100, 255, 255},{0, 0, 128},{67, 133, 255},{130, 0, 150},{230, 190, 255},{255, 0, 255},{128, 128, 128}};
const String distinctColors[22] = {"white","black","maroon","red","pink","brown","orange","coral","olive","yellow","beige","lime","green","mint","teal","cyan","navy","blue","purple","lavender","magenta","grey"};

/**
 * Match the closest named color to a passed R G B value set
 * 
 * @param r {Integer} The R value 
 * @param g {Integer} The G value
 * @param b {Integer} The B value
 * @returns {String} The name of the closest color
 */
String closestColor(int r, int g, int b) {
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

// initializing sensor color name array
char *sensorName[4] = {"Red", "Green", "Blue", "Clear"};

//  array to save the sensor results
byte sensorValue[4];  //Red, Green, Blue, Clear

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

void setup() {

  //Set the pins of the Color Sensor
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(sOut, INPUT);


  //  The pins S0 & S1 used for frequency scaling
  //    S0  S1
  //    L - L = Power Down
  //    L - H = 2%
  //    H - L = 20%
  //    H - H = 100%
  //
  
  //Setting frequency
  digitalWrite(s0, HIGH);
  digitalWrite(s1, HIGH); // LOW?
 
  //Set the serial communication in bytes per second for the serial 
  //monitor and the link to the second arduino
  Serial.begin(9600);

}

void loop() {
    
  //Delay to stabilize readings
  delay(bigDelay);

  // scan the color using the sensor to get red, blue, green & clear values
  // and store tham in the global array sensorValue
  scanTheColor();

  // Use the colorValues table to identify the color
  identifyTheColor();
  

  // reset color
  identifiedColor = 0;
  
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  read the sensor to get the red, green, blue & clear values nd store in scannedValue array
//
//////////////////////////////////////////////////////////////////////////////////////////
void  scanTheColor() {
    
  // red    uses pinVal 0,0
  // green  uses pinVal 0,1
  // blue   uses pinVal 1,0
  // clear  uses pinVal 1,1

  pinVal1 = 0;
  pinVal2 = 0;
    
  for (byte x = 0; x < 4; x++) {
    // read the four values RED/GREEN/BLUE/CLEAR
    // Serial.print("pinVal1=");
    // Serial.print(pinVal1);
    // Serial.print("  ");
    // Serial.print("pinVal2=");
    // Serial.print(pinVal2);
    // Serial.print("    ");
      
    digitalWrite(s2, pinVal1);
    digitalWrite(s3, pinVal2);

    tempColor = 0;  // clear temp variable to hold the total of all the sensor values
    zColor = 0;     // clear the variable to hold the sensor value
    
    Serial.print("Z=");
    for (byte y = 0; y < scanCnt; y++) {
      zColor = pulseIn(sOut, LOW);
      
      Serial.print(zColor);
      Serial.print(", ");
      tempColor = tempColor + zColor;
      // Serial.print("Temp=");
      // Serial.println(tempColor);
      delay(smallDelay);
    }
    Serial.println("");

    sensorValue[x] = tempColor >> bitShift;
        
    Serial.print(sensorName[x]);
    Serial.print("=");
    Serial.print(sensorValue[x]);
    Serial.println();

    // toggle the bits to get the next value
    pinVal1 = pinVal1 | pinVal2;
    pinVal2 = pinVal2 ^ 1;

    delay(smallDelay);
  }

  // sensorValue[0], 1, 2, 3
  Serial.print(sensorValue[0]); Serial.print(", ");
  Serial.print(sensorValue[1]); Serial.print(", ");
  Serial.print(sensorValue[2]); Serial.print(", ");

}
