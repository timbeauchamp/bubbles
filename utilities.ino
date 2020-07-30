/*
Bubble Detector
Utilities.ino
  
*/

void printStatus()
{
    Serial.print("current Millis: ");
    Serial.println(millis());

    // Print the buckets
    for(int i=0; i < NUMBUCKETS; i++)
    {
        Serial.print("[");
        Serial.print(bubbles[i]);
        Serial.print("],");        
    }
    Serial.println();
    
    Serial.print("current timeStartingCurrentBucket: ");
    Serial.println(timeStartingCurrentBucket);
    
    Serial.print("Time in Current Bucket (sec): ");
    Serial.println((millis() - timeStartingCurrentBucket)/1000);

    Serial.print("Seconds stored in all buckets: ");
    Serial.println(getSecondsStoredInBuckets());

    Serial.print("Total Bubbles:");
    Serial.println(getTotalBubbles());

    Serial.print("Current BPH:");
    Serial.println(getBPH());
}


void recordBubble()
{
    if((millis() - lastBubbleTime) < 250)
    {
        return;
    }
    
    Serial.print("BUBBLE!\n");    
    bubbles[bubblesOffset]++;
    Serial.print("Current Count:");    
    Serial.println(bubbles[bubblesOffset]);    

    lastBubbleTime = millis();
//    printStatus();
    return;
}

void setMinMax(int value)
{
    if(value < minVal)
    {
        minVal = value;
    }

    if(value > maxVal)
    {
        maxVal = value;        
    }
}

void resetMinMax()
{
        minVal = 1024;
        maxVal = 0;        
}

int getThreshold() 
{
    return (minVal + maxVal)/2;
}

int minutes()  // minutes since startTime
{
    unsigned long seconds = (millis() - startTime)/ 1000;
    int minutes = (seconds) / 60;
    return minutes;
}

void updateBucket()
{
   int mins = minutes();  // Get minutes since startTime
    
   int newBubblesOffset = (mins/MINUTESPERBUCKET) % NUMBUCKETS;

    if(bubblesOffset != newBubblesOffset)
    {
        numBucketsValid++;
        if(numBucketsValid  >= (NUMBUCKETS - 1))
        {
            numBucketsValid = NUMBUCKETS - 1;
        }
        
        timeStartingCurrentBucket = millis();
        bubblesOffset = newBubblesOffset;
        bubbles[bubblesOffset] = 0;
    }
}

double getSecondsStoredInBuckets()
{
    int totalTime = (numBucketsValid * MINUTESPERBUCKET * 60.0) + ((millis() - timeStartingCurrentBucket) / 1000.0);
    return totalTime;
}

int getTotalBubbles()
{
    int  total = 0;
    for(int  i=0; i <12; i++)
    {
        total += bubbles[i];
    }

    return total;
}

int getBPH()
{
    int total = getTotalBubbles();
    double minutes = getSecondsStoredInBuckets() / 60.0;
    
    int bph = total/minutes * 60;
    return bph;
}

void setInBubbleLED()
{
    if(inBubble)
    {
       inBubbleLEDState = HIGH; 
    }
    else
    {
        inBubbleLEDState = LOW;
    }
    digitalWrite(LED_BUILTIN, inBubbleLEDState);     
}

void updateLCD(boolean show_message)
{ 
    String lines[] = {String(),String(),String(),String()};
   

    Serial.println("Updating LCD");
    
    lcd.clear();
    if(show_message)
    {
        lines[0]= "    I'm Alive!      ";
        lines[1]= "  Let's Brew Beer   ";
        lines[2]= "  Ready to count    ";
        lines[3]= "      BUBBLES!      ";
    }
    else
    {
        int bph = getBPH();
        int minutes = millis() / 60000;
        int sampleseconds = getSecondsStoredInBuckets()/60;
        int num_bubbles = getTotalBubbles();

        lines[0]= "Bubbles: %d",num_bubbles;
        lines[1]= "Total Mins: %d", minutes;
        lines[2]= "Bubbles/Hour: %d", bph;
        lines[3]= "Mins Sampled: %d ", sampleseconds/60;
    }
    
    for(int i=0; i < 4; i++)
    {
        lcd.setCursor ( 0, i );            // go to the top left corner
        lcd.print(lines[i]); // write this string on the top row
    }


}

// Seconds per Bubble = color
// < 1            Green
// 1 - 3.9      Green & Yellow
// 3.9 - 15.4   Yellow
// 15.4 - 60    Red & Yellow
// > 60         Red
void updateLEDs()
{
    int bph = getBPH();
    
    if(bph >= 3600)
    {
        setLED0(0);
        setLED1(0);
        setLED2(255);
    }
    else if(912 <= bph && bph < 3600)
    {
        setLED0(0);
        setLED1(255);
        setLED2(255);        
    }
    else if(234 <= bph && bph < 912)
    {
        setLED0(0);
        setLED1(255);
        setLED2(0);        
    }
    else if(60 <= bph && bph < 234)
    {
        setLED0(255);
        setLED1(255);
        setLED2(0);
    }
    else
    {
        setLED0(255);
        setLED1(0);
        setLED2(0);  
    }
    
}

//void setLEDs(int state)
//{
//       setLED0(state);
//       setLED1(state);
//       setLED2(state);
//}

void setLED0(int state)
{
    analogWrite(LED0Pin, state); 
}

void setLED1(int state)
{
    analogWrite(LED1Pin, state); 
}

void setLED2(int state)
{
    analogWrite(LED2Pin, state); 
}


void getSerial()
{
    if (Serial.available() > 0) 
    {
        // read the incoming byte:
        byte incomingByte = Serial.read();
    
//        Serial.println("Got Serial Event ");
//        Serial.println(incomingByte, DEC);
        recordBubble();
      }
}
