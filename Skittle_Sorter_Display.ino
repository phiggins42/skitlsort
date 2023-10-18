//James & John Rinkel
//2023.07.10

//
//  This sketch reads the RX pin from the master
//  arduino that is sorting skittles and displays
//  the color and count of the skittle just sorted
//  to the LCD.
//
//  The circuit:
//   LCD RS pin to digital pin 7
//   LCD Enable pin to digital pin 8
//   LCD D4 pin to digital pin 9
//   LCD D5 pin to digital pin 10
//   LCD D6 pin to digital pin 11
//   LCD D7 pin to digital pin 12
//   LCD R/W pin to ground
//   LCD VSS pin to ground
//   LCD VCC pin to 5V
//   10K resistor: 
//   ends to +5V and ground
//   wiper to LCD VO pin (pin 3)
//

// include the LCD library
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// declare an array to store the colors counts
// 0=red, 1=yellow, 2=green, 3=orange, 4=purple, 5=total
int skittleCount[6];   //five colors and one for the total count

// declare buffers for formatting lcd output
char line0Buffer[16];
char line1Buffer[16];

void setup()
  {
// set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

// set up the serial connection from the skittle sorting system
  Serial.begin(9600);

// zero the counts for the colors
  for (byte x=0; x<=5; x++)
  {
    skittleCount[x] = 0;
  }

//set the cursor to column 0, line 0
  lcd.setCursor(0, 0);
  
// display a message
  lcd.print("Waiting");
  }

void loop()
  { 
// - check for new serial data - and display accordingly
    if (Serial.available() > 0)
    {
      int y = Serial.read();      // The "Serial.read" command returns a byte
      if (y >= 1 && y <= 5)       // Check to see if its data we want and not serial msg to the monitor
        {
        byte color = --y;         // decrement the nbr recieved to use as a subscript
        skittleCount[color] +=1;  // increment the color count r=0, y=1, g=2, o=3, p=4, t=5
        skittleCount[5] +=1;      // increment the total count
                                  
        //format the first line of the lcd
        sprintf(line0Buffer, "R=%2i Y=%2i G=%2i", skittleCount[0], skittleCount[1], skittleCount[2]);
        
        //format the second line of the lcd
        sprintf(line1Buffer, "O=%2i P=%2i T=%2i", skittleCount[3], skittleCount[4], skittleCount[5]);
        
        // set the cursor to line 0
        lcd.setCursor(0, 0);
        
        //print line one
        lcd.print(line0Buffer);
        
        // set the cursor to line 1
        lcd.setCursor(0,1);
       
        //print line two
        lcd.print(line1Buffer);
        
        }  // end if char we want loop
    }  // end if serial available loop
  } //end void loop
  
///////////////////////////////////////////////////////////////////////////////////
