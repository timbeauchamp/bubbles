#include <Wire.h>
#include <LiquidCrystal.h>

/*
Bubble Detector
  
*/

#define NUMBUCKETS 12
#define MINUTESPERBUCKET 1


const int sensorPin = A0;   // Analog input pin that the detector bridge is connected to

const int LED0Pin = 3;     // Green LED
const int LED1Pin = 5;     // Yellow LED
const int LED2Pin = 6;     // Red LED

int sensorValue = 0;        // value read from the sensor

int bubbles[NUMBUCKETS];  // Buckets of bubbles
int bubblesOffset = 0;  // current  bucket of bubbles
int numBucketsValid = 0;    
unsigned long lastBubbleTime = 0;  // millis of last bucket
unsigned long startTime = 0; // millis that this started 
unsigned long totalBucketTime = 0; // millis for all buckets 
unsigned long timeStartingCurrentBucket = 0;  // obvious
unsigned long lastLCDUpdate = 0;  // millis of last bucket
boolean initialized = false;

boolean inBubble = false;   // Are we currently in a bubble?

int minVal;   // min and max sensor value
int maxVal;

int inBubbleLEDState = LOW;

// initial Setup
void setup() 
{
    // initialize serial communications at 9600 bps:
    Serial.begin(9600);

    //initialize the indicator LED:
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED0Pin, OUTPUT);
    pinMode(LED1Pin, OUTPUT);
    pinMode(LED2Pin, OUTPUT);

    resetMinMax();
    timeStartingCurrentBucket = millis();
    startTime = millis();

    lastLCDUpdate = millis();
    initialize_LCD();
    
    Serial.println("Initialized");
}

/********************************/
// include the library code
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
/*********************************************************/
void initialize_LCD()
{
    // uses:
    // I2C LCD2004    SunFounder Mars board
    // GND GND
    // VCC 5V
    // SDA A4 /pin 20 mega2560
    // SCL A5 /pin 21 mega2560
    
  lcd.init();  //initialize the lcd
  lcd.backlight();  //open the backlight 
  updateLCD(true);
}

void loop() 
{
    int updateBucket();

    readSensor();
    updateLEDs();

    if((millis() > (lastLCDUpdate + 1000 * 5)) || (millis() < lastLCDUpdate))
    {
        if(!initialized)
        {
            initialized = true;
            bubbles[0] = 0;            
        }
        lastLCDUpdate = millis();
        updateLCD(false);
    }
    getSerial();
    delay(100);
}

void readSensor()
{
//   read the analog in value:
   sensorValue = analogRead(sensorPin);

    setMinMax(sensorValue);

    if(sensorValue < getThreshold() )
    {
        if(inBubble)
        {
            
        }
        else
        {
            inBubble = true;

            inBubble = true;
            recordBubble();
            Serial.print("\nMin:Max:Thresh = ");
            Serial.print(minVal);
            Serial.print(":");
            Serial.println(maxVal);
            Serial.print(":");
            Serial.println(getThreshold());

            Serial.print("CurrentBucket = ");
            Serial.println(bubblesOffset);
            Serial.print("Current count in Bucket = ");
            Serial.println(bubbles[bubblesOffset]);
            Serial.print("CurrentTotal = ");
            Serial.println(getTotalBubbles());
            Serial.print("Current Bubbles Per Hour = ");
            Serial.println(getBPH());

//          resetMinMax();
        }
    }
    else
    {
        if(inBubble)
        {
            inBubble = false;    
        }
        else
        {
        
        } 
    }

    setInBubbleLED();
 // print the results to the serial monitor:
 // Serial.print("sensor = ");
 // Serial.println(sensorValue);

}
