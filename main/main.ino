//datasheet at https://github.com/Atlantis-Specialist-Technologies/CAN485/blob/master/Documentation/Datasheet%20AT90CANXX.pdf
#include <ASTCanLib.h>  


unsigned const int PULSES_PER_MILE = 8000;                  //typical early Ford sensor
byte const MODE_PIN = 8;                                    //switch to select auto or manual potentiometer control
bool automaticMode = 0;                                     //1 = automatic (speed sensitive), 0 = manual (user turns knob)
byte const VSS_PIN = 9;                                     //pin 9 on the board corresponds to interrupt 7 on the chip
volatile unsigned long vssCounter = 0;                      //increment pulses in the interrupt function
unsigned long vssCounterPrevious = 0;                       //used to calculate speed
unsigned long currentMillis = 0;                            //now
unsigned long lastMillis = 0;                               //used to cut time into slices of SPEED_CALC_INTERVAL
byte const SPEED_CALC_INTERVAL = 125;                       //read number of pulses approx 8 times per second
byte const BUFFER_LENGTH = 4;                               //store this instead of always calculating it
float mphBuffer[BUFFER_LENGTH] = {0.0f, 0.0f, 0.0f, 0.0f};  //keep buffer of mph readings (approx .5 second)
byte bufferIndex = 0;
byte const KNOB_PIN = 14;                                   //a0
int knobPosition = 0;
int knobBuffer[BUFFER_LENGTH] = {0,0,0,0};                  //keep buffer of potentiometer readings (approx .5 second)

//interrupt routine for interrupt 7 (pin 9)
ISR(INT7_vect) {
  vssCounter++;
}

void setup() {
  Serial.begin(9600);
  pinMode(VSS_PIN, INPUT_PULLUP);
  pinMode(MODE_PIN, INPUT);
  //set trigger for interrupt 7 (pin 9) to be falling edge (see datasheet)
  EICRB |= ( 1 << ISC71);
  EICRB |= ( 0 << ISC70);

  //enable interrupt 7 (pin 9) (see datasheet)
  EIMSK |= ( 1 << INT7);
  
  Serial.println("Finished initialization");
}

void loop() {

  currentMillis = millis();

  automaticMode = digitalRead(MODE_PIN);


  
  //perform speed calculation on an interval of SPEED_CALC_INTERVAL
  if(currentMillis - lastMillis >= SPEED_CALC_INTERVAL && currentMillis > 500) {


    
    long pulses = vssCounter - vssCounterPrevious;
    vssCounterPrevious = vssCounter;

    //calculate miles per hour
    float pulsesPerSecond = (float)pulses * ((float)1000 / ((float)currentMillis - (float)lastMillis));
    float pulsesPerMinute = pulsesPerSecond * 60.0;
    float pulsesPerHour = pulsesPerMinute * 60.0;
    float milesPerHour = pulsesPerHour / (float)PULSES_PER_MILE;
    
    if(bufferIndex >= BUFFER_LENGTH - 1) {
      bufferIndex = 0;
    }
    else {
      bufferIndex++;
    }
    mphBuffer[bufferIndex] = milesPerHour;

    float mphSum = 0.0;
    for(byte i = 0; i < BUFFER_LENGTH; i++) {
      mphSum += mphBuffer[i];
    }
    float smoothedMPH = mphSum / (float)BUFFER_LENGTH;
    //Serial.println(smoothedMPH);
    sendToCan(smoothedMPH);
    lastMillis = currentMillis;


    knobPosition = analogRead(KNOB_PIN);
    knobBuffer[bufferIndex] = knobPosition;
    int knobSum = 0;
    for(int i = 0; i < BUFFER_LENGTH; i++) {
      knobSum += knobBuffer[i];
    }
    int smoothedKnobPosition = knobSum / BUFFER_LENGTH;

    
    Serial.println(smoothedKnobPosition);
  }
}

void calculateSpeed() {
  
}

void sendToCan(float mph) {
  //send vssCounter on the CAN bus to be interpreted as an odometer reading
  //send calculated speed on the CAN bus
}

