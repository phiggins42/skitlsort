//
// Skittle_Color_Calibration
//
// A program to "calibrate the values
// of the TCS3200 Color Sensor
//
// James & John Rinkel
// 8/4/2023

  #include <Servo.h>

  // Declareing Servo
  Servo feedingServo;
  Servo decisionServo;

  // define servo pin connection
  #define feedingServoPin 9
  #define decisionServoPin 11

  // define the feeding servo positions
  #define pos1FeederServo 168  // get skittle position
  #define pos2FeederServo 104  // scan skittle position
  #define pos3FeederServo   6  // drop skittle position

  // define the decision servo positions
  byte decisionServoMatrix[5] = {125, 104, 78, 56, 31};
  
  // define delay times
  #define smallDelay     30   // 100 ms
  #define mediumDelay   500   // 250 ms
  #define bigDelay    10000   // ten seconds
  
  //Define the pins of the color sensor
  #define s0 2 //S0 pin of the sensor on Arduino pin#2 
  #define s1 3 //S1 pin of the sensor on Arduino pin#3 
  #define s2 4 //S2 pin of the sensor on Arduino pin#4 
  #define s3 5 //S3 pin of the sensor on Arduino pin#5
  #define sensorOut 6 //Output pin of the sensor on Arduino pin#6 

  //Variable to save the data coming from the sensorOut
  int frequency = 0;

  // set up list of colors
  // five colors of up to 6 chars and a null in an array
  char *skittleColor[5] = {"Red", "Yellow", "Green", "Orange", "Purple"};

  // set up color array variable
  byte skittleColorIndex = 0;

  // temp area for averging frequency
  int tempFrequency;

  void setup()
  {  
  // setup serial monitor
  Serial.begin(9600);

  // attach the servos
  feedingServo.attach (feedingServoPin);
  decisionServo.attach (decisionServoPin);
  
  //setup pins for color sensor
  pinMode (s0, OUTPUT);
  pinMode (s1, OUTPUT);
  pinMode (s2, OUTPUT);
  pinMode (s3, OUTPUT);
  pinMode (sensorOut, INPUT);

  // Setting frequency scaling
  //      S0    S1
  // 100% HIGH  HIGH
  //  20% HIGH  LOW
  //   2% LOW   HIGH
  //   0% LOW   LOW
  digitalWrite(s0, HIGH);
  digitalWrite(s1, LOW);

  // put the feeding servo in the initial position;
  feedingServo.write (pos3FeederServo);
  
  } // end setup

void loop()
  {
    Serial.print("Ready to calibrate sensor");
    
    // read each of the five skittle types
    for (skittleColorIndex = 0; skittleColorIndex <= 4; skittleColorIndex++)
    {
      // initialize variables to save the sensor range of values
      byte lowRedvalue = 99;
      byte highRedvalue = 0;
      byte lowBluevalue = 99;
      byte highBluevalue = 0;
      byte lowGreenvalue = 99;
      byte highGreenvalue = 0;
      byte lowClearvalue = 99;
      byte highClearvalue = 0;

      // move the decision servo to the color we are scanning
      delay(mediumDelay);
      decisionServo.write (decisionServoMatrix[skittleColorIndex]);
      
      // prompt for the color
      delay (bigDelay);
      Serial.println();
      Serial.print ("Place 5 ");
      Serial.print(skittleColor[skittleColorIndex]);
      Serial.print(" skittles in the feeder tube");
      Serial.println();
      delay (bigDelay);

      // read all five skittles from the feeder tube
      for (byte feederCount = 0; feederCount <= 4; feederCount++)
        {
        // move the feeder to get a skittle
        for (byte i = pos3FeederServo; i < pos1FeederServo; i++)
          {
          feedingServo.write (i);
          delay (smallDelay);
          }
        // move the feeder to scan the skittle
        for (byte i = pos1FeederServo; i > pos2FeederServo; i--)
          {
            feedingServo.write (i);
            delay (smallDelay);
          }
        // set frequency to zero  
        frequency = 0;
        tempFrequency = 0;
        
        // Begin the reading the Color 4 times
        for (byte i = 0; i < 4; i++)
          {
          // ---Read RED values... S2 LOW - S3 LOW---
          digitalWrite(s2, LOW);
          digitalWrite(s3, LOW);
          frequency = pulseIn(sensorOut, LOW);
          Serial.print("F=");
          Serial.print(frequency);
          tempFrequency = tempFrequency + frequency;
          Serial.print("TF=");
          Serial.print(tempFrequency);
          Serial.println();
          delay(smallDelay);
          }
          
          tempFrequency = tempFrequency / 4;
          Serial.print("Shifted=");
          Serial.print(tempFrequency);
          Serial.println();
          
          byte redValue = tempFrequency;

          // check low red value
          if (redValue < lowRedvalue) lowRedvalue = redValue;

          // check high red value
          if (redValue > highRedvalue) highRedvalue = redValue;

          // wait 
          delay(smallDelay);

          frequency = 0;
          tempFrequency = 0;
          
   
          // ---Read Green values ... S2 LOW - S3 HIGH---
          for (byte i = 0; i < 4; i++)
          {
          digitalWrite(s2, LOW);
          digitalWrite(s3, HIGH);
          frequency = pulseIn(sensorOut, LOW);
          Serial.print("F=");
          Serial.print(frequency);
          tempFrequency = tempFrequency + frequency;
          Serial.print("TF=");
          Serial.print(tempFrequency);
          Serial.println();
          delay(smallDelay);
          }

          tempFrequency = tempFrequency / 4;
          Serial.print("Shifted=");
          Serial.print(tempFrequency);
          Serial.println();
          
          byte greenValue = tempFrequency;

          // check low green value
          if (greenValue < lowGreenvalue) lowGreenvalue = greenValue;

          // check high green value
          if (greenValue > highGreenvalue) highGreenvalue = greenValue;

          // wait
          delay(smallDelay);

          frequency = 0;
          tempFrequency = 0;
          
          // ---Read Blue values ... S2 HIGH - S3 LOW---
          for (byte i = 0; i < 4; i++)
          {
          digitalWrite(s2, HIGH);
          digitalWrite(s3, LOW);
          frequency = pulseIn(sensorOut, LOW);
          Serial.print("F=");
          Serial.print(frequency);
          tempFrequency = tempFrequency + frequency;
          Serial.print("TF=");
          Serial.print(tempFrequency);
          Serial.println();
          delay(smallDelay);
          }

          tempFrequency = tempFrequency / 4;
          Serial.print("Shifted=");
          Serial.print(tempFrequency);
          Serial.println();
          
          byte blueValue = tempFrequency ;

          // check low blue value
          if (blueValue < lowBluevalue) lowBluevalue = blueValue;

          // check high bue value
          if (blueValue > highBluevalue) highBluevalue = blueValue;

          //wait
          delay(smallDelay);

          frequency = 0;
          tempFrequency = 0;
          
          // ---Read Clear values ... S2 HIGH - S3 HIGH---
          for (byte i = 0; i <4; i++)
          {
          digitalWrite(s2, HIGH);
          digitalWrite(s3, HIGH);
          frequency = pulseIn(sensorOut, LOW);
          Serial.print("F=");
          Serial.print(frequency);
          tempFrequency = tempFrequency + frequency;
          Serial.print("TF=");
          Serial.print(tempFrequency);
          Serial.println();
          delay(smallDelay);
          }

          tempFrequency = tempFrequency / 4;
          Serial.print("Shifted=");
          Serial.print(tempFrequency);
          Serial.println();
          
          byte clearValue = tempFrequency ;

          // check low clear value
          if (clearValue < lowClearvalue) lowClearvalue = clearValue;

          // check high clear value
          if (clearValue > highClearvalue) highClearvalue = clearValue;

          //wait
          delay(smallDelay);

        // end read skittle color 4 times loop to get ranges
  
        // drop the skittle
        for (int i= pos2FeederServo; i > pos3FeederServo; i--)
          {
            feedingServo.write (i);
            delay (smallDelay);
          }
               
        } //  end feederCount reading 5 skittles from the feeder tube
  
    // show results for each skittle color
    Serial.print(skittleColor[skittleColorIndex]);

    // print red range
    Serial.print(" skittle ");
    Serial.print("\t");
    Serial.print("RedValue= ");
    Serial.print(lowRedvalue);
    Serial.print("-");
    Serial.print(highRedvalue);
    Serial.print("\t");

    // print green range
    Serial.print("GreenValue= ");
    Serial.print(lowGreenvalue);
    Serial.print("-");
    Serial.print(highGreenvalue);
    Serial.print("\t");

    // print blue range
    Serial.print("BlueValue= ");
    Serial.print(lowBluevalue);
    Serial.print("-");
    Serial.print(highBluevalue);
    Serial.print("\t");

    // print clear range
    Serial.print("ClearValue= ");
    Serial.print(lowClearvalue);
    Serial.print("-");
    Serial.print(highClearvalue);
    Serial.print("\t");

    // new line
    Serial.println();
    
    } // end skittleColorIndex loop

    // finished scanning all five skittle colors STOP
    Serial.println();
    Serial.println("DONE");
    delay(bigDelay);
    delay(bigDelay);
    delay(bigDelay);
  }  
  
