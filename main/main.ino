//datasheet at https://github.com/Atlantis-Specialist-Technologies/CAN485/blob/master/Documentation/Datasheet%20AT90CANXX.pdf

unsigned const int PULSES_PER_MILE = 8000;      //typical early Ford sensor
//unsigned const long MILLIS_IN_MINUTE = 60000;   //spread the word
int vssPin = 9;                                 //pin 9 on the board corresponds to interrupt 7 on the chip
volatile unsigned long vssCounter = 0;          //increment pulses in the interrupt function
unsigned long vssCounterPrevious = 0;           //used to calculate speed
unsigned long currentMillis = 0;                //now
unsigned long lastMillis = 0;                   //used to cut time into slices of speedCalcInterval
byte speedCalcInterval = 125;                   //read number of pulses approx 8 times per second
float mphBuffer[4];                             //keep buffer of mph readings (approx .5 second)

//interrupt routine for interrupt 7 (pin 9)
ISR(INT7_vect) {
  vssCounter++;
}

void setup() {
  Serial.begin(9600);
  pinMode(vssPin, INPUT_PULLUP);
    
  //set trigger for interrupt 7 (pin 9) to be falling edge (see datasheet)
  EICRB |= ( 1 << ISC71);
  EICRB |= ( 0 << ISC70);

  //enable interrupt 7 (pin 9) (see datasheet)
  EIMSK |= ( 1 << INT7);
  
  Serial.println("Finished initialization");
}

void loop() {

  currentMillis = millis();

  //perform speed calculation on an interval of speedCalcInterval
  if(currentMillis - lastMillis >= speedCalcInterval && currentMillis > 500) {

    long pulses = vssCounter - vssCounterPrevious;
    vssCounterPrevious = vssCounter;
    
    //Serial.print("total pulses: ");
    //Serial.println(vssCounter);

    //calculate miles per hour
    float pulsesPerSecond = (float)pulses * ((float)1000 / ((float)currentMillis - (float)lastMillis));
    float pulsesPerMinute = pulsesPerSecond * 60.0;
    float pulsesPerHour = pulsesPerMinute * 60.0;

    //previousMilesPerHour = currentMilesPerHour;
    
    float milesPerHour = pulsesPerHour / (float)PULSES_PER_MILE;

    
    if(milesPerHour > 0.0) {
      Serial.print("miles per hour: ");
      Serial.println(milesPerHour);
    }
    

    //Serial.println(vssCounter);
    lastMillis = currentMillis;
  }
}

void calculateSpeed() {
  
}

void sendToCan() {
  //send vssCounter on the CAN bus to be interpreted as an odometer reading
  //send calculated speed on the CAN bus
}

