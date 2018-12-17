//datasheet at https://github.com/Atlantis-Specialist-Technologies/CAN485/blob/master/Documentation/Datasheet%20AT90CANXX.pdf
#include <ASTCanLib.h>
#include <math.h>

#define DEBUG_KNOB true
#define DEBUG_MPH false
#define DEBUG_AUTO false

//pins used on board
byte const VSS_PIN = 9;                                     //pin 9 on the board corresponds to interrupt 7 on the chip

//other constants
unsigned const int PULSES_PER_MILE = 8000;                  //typical early Ford speed sensor
byte const SPEED_CALC_INTERVAL = 125;                       //read number of pulses approx every 1/8 second
byte const BUFFER_LENGTH = 4;                               //length of MPH buffer
byte const KNOB_BUFFER_LENGTH = 255;                        //length of potentiometer buffer

bool automaticMode = 0;                                     //1 = automatic (speed sensitive), 0 = manual (user turns knob)
volatile unsigned long vssCounter = 0;                      //increment pulses in the interrupt function
unsigned long vssCounterPrevious = 0;                       //used to calculate speed
unsigned long currentMillis = 0;                            //now
unsigned long lastMillis = 0;                               //used to cut time into slices of SPEED_CALC_INTERVAL
float mphBuffer[BUFFER_LENGTH];                             //keep buffer of mph readings (approx .5 second)
byte mphBufferIndex = 0;
byte previousVal = 255;                                     //for the automatic algorithm

//interrupt routine for interrupt 7 (pin 9)
ISR(INT7_vect) {
  vssCounter++;
}

void setup() {
  Serial.begin(9600);
  pinMode(VSS_PIN, INPUT_PULLUP);
  //set trigger for interrupt 7 (pin 9) to be falling edge (see datasheet)
  EICRB |= ( 1 << ISC71);
  EICRB |= ( 0 << ISC70);

  //enable interrupt 7 (pin 9) (see datasheet)
  EIMSK |= ( 1 << INT7);
  
  Serial.println("Finished initialization");
}

void loop() {
  currentMillis = millis();
  
  //perform speed calculation on an interval of SPEED_CALC_INTERVAL
  if(currentMillis - lastMillis >= SPEED_CALC_INTERVAL && currentMillis > 500) {
    
    //calculate miles per hour
    float mph = calculateSpeed();
    sendToCan(mph);
    lastMillis = currentMillis;
  }
}

byte calculateAutomaticKnobValue(float mph) {
    byte val = 255;
    if(mph > 65.0) {
      mph = 65.0;
    }
    else if(mph <= 5.0) {
      mph = 0.0;
      val = 255;
      previousVal = 255;
    }
    byte subtractBy = 255 - map(65.0 - mph, 0, 65, 0, 255);
    val = val - subtractBy;

    //if accelerating
    if(val > previousVal) {
      if((val - previousVal) > 5) {
        val = previousVal + 6;
      }
      else {
        val = previousVal;
      }
    }
    //if decelerating
    else if(val < previousVal) {
      if((previousVal - val) > 4) {
        val = previousVal - 5;
      }
      else {
        val = previousVal;
      }      
    }

    previousVal = val;

  if(DEBUG_AUTO) {
    Serial.print("digital knob: ");
    Serial.println(val);
  }
  return val;  
}

void sendToPot(byte pos) {
  
}

float calculateSpeed() {
  long pulses = vssCounter - vssCounterPrevious;
    vssCounterPrevious = vssCounter;
    float pulsesPerSecond = (float)pulses * ((float)1000 / ((float)currentMillis - (float)lastMillis));
    float pulsesPerMinute = pulsesPerSecond * 60.0;
    float pulsesPerHour = pulsesPerMinute * 60.0;
    float milesPerHour = pulsesPerHour / (float)PULSES_PER_MILE;
    
    if(mphBufferIndex >= BUFFER_LENGTH - 1) {
      mphBufferIndex = 0;
    }
    else {
      mphBufferIndex++;
    }
    mphBuffer[mphBufferIndex] = milesPerHour;

    float mphSum = 0.0;
    for(byte i = 0; i < BUFFER_LENGTH; i++) {
      mphSum += mphBuffer[i];
    }
    float smoothedMPH = mphSum / (float)BUFFER_LENGTH;

    if(DEBUG_MPH) {
      Serial.print("MPH: ");
      Serial.println(smoothedMPH);
    }
    
    return smoothedMPH;
}

void sendToCan(float mph) {
  //send vssCounter on the CAN bus to be interpreted as an odometer reading
  //send calculated speed on the CAN bus
}

