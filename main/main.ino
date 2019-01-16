//datasheet at https://github.com/Atlantis-Specialist-Technologies/CAN485/blob/master/Documentation/Datasheet%20AT90CANXX.pdf
#include <ASTCanLib.h>
#include <math.h>

#define DEBUG_KNOB true
#define DEBUG_MPH false

//pins used on board
byte const VSS_PIN = 9;                                     //pin 9 on the board corresponds to interrupt 7 on the chip
byte const POS_1_PIN = 2;
byte const POS_2_PIN = 3;
byte const POS_3_PIN = 4;
byte const POS_4_PIN = 5;
byte const POS_5_PIN = 6;
byte const POS_6_PIN = 7;

//other constants
unsigned const int PULSES_PER_MILE = 8000;                  //typical early Ford speed sensor
byte const SPEED_CALC_INTERVAL = 125;                       //read number of pulses approx every 1/8 second
byte const BUFFER_LENGTH = 4;                               //length of MPH buffer
byte const KNOB_BUFFER_LENGTH = 255;                        //length of potentiometer buffer

byte assistMode = 5;                                        //5 = medium/high static assist
volatile unsigned long vssCounter = 0;                      //increment pulses in the interrupt function
unsigned long vssCounterPrevious = 0;                       //used to calculate speed
unsigned long currentMillis = 0;                            //now
unsigned long lastMillis = 0;                               //used to cut time into slices of SPEED_CALC_INTERVAL
float mphBuffer[BUFFER_LENGTH];                             //keep buffer of mph readings (approx .5 second)
byte mphBufferIndex = 0;
byte previousVal = 255;                                     //for the automatic algorithm

byte oldAssistValue = 0;
byte newAssistValue = 0;

//interrupt routine for interrupt 7 (pin 9)
ISR(INT7_vect) {
  vssCounter++;
}

void setup() {
  Serial.begin(9600);
  pinMode(VSS_PIN, INPUT_PULLUP);
  pinMode(POS_1_PIN, INPUT_PULLUP);
  pinMode(POS_2_PIN, INPUT_PULLUP);
  pinMode(POS_3_PIN, INPUT_PULLUP);
  pinMode(POS_4_PIN, INPUT_PULLUP);
  pinMode(POS_5_PIN, INPUT_PULLUP);
  pinMode(POS_6_PIN, INPUT_PULLUP);
  
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
    
    float mph = calculateSpeed();
    sendToCan(mph);

    //calculate assist level
    byte newAssistMode = getMode(assistMode);
    if(newAssistMode != assistMode) {
      Serial.print("new assist mode: ");
      Serial.println(newAssistMode);
      assistMode = newAssistMode;
    }

    switch(assistMode) {
      case 1:
        newAssistValue = calculateMode1(mph);
        break;
      case 2:
        newAssistValue = calculateMode2(mph);
        break;
      case 3:
        newAssistValue = calculateMode3(mph);
        break;
      case 4:
        newAssistValue = calculateMode4(mph);
        break;
      case 5:
        newAssistValue = calculateMode5(mph);
        break;
      case 6:
        newAssistValue = calculateMode6(mph);
        break;
    }

    if(newAssistValue != oldAssistValue) {
      sendToPot(newAssistValue);
      oldAssistValue = newAssistValue;
    }
    
    lastMillis = currentMillis;
  }
}

byte getMode(byte previousMode) {
  byte mode = previousMode;
  if(!digitalRead(POS_1_PIN)) {
    mode = 1;
  }
  else if(!digitalRead(POS_2_PIN)) {
    mode = 2;
  }
  else if(!digitalRead(POS_3_PIN)) {
    mode = 3;
  }
  else if(!digitalRead(POS_4_PIN)) {
    mode = 4;
  }
  else if(!digitalRead(POS_5_PIN)) {
    mode = 5;
  }
  else if(!digitalRead(POS_6_PIN)) {
    mode = 6;
  }
  return mode;
}

//no power assist
byte calculateMode1(float mph) {
  return 0;
}

//~2/3 power assist for parking lot speeds, almost none when faster
byte calculateMode2(float mph) {
  if(mph < 10.0f) {
    return 170;
  }
  else {
    return 25;
  }
}

//sport mode speed sensitive power assist
byte calculateMode3(float mph) {
  return calculateSpeedSensitiveAssist(mph, 65, 140, 20, 15);
}

//comfort mode speed sensitive power assist
byte calculateMode4(float mph) {
  return calculateSpeedSensitiveAssist(mph, 100, 170, 15, 10);
}

//~2/3 power assist at all times
byte calculateMode5(float mph) {
  return 170;
}

//full power assist at all times
byte calculateMode6(float mph) {
  return 255;
}

/*
 * mph:             current speed of the vehicle
 * minAssist:       minimum assist value (at higher speed)
 * maxAssist:       maximum assist value (low speed)
 * maxChange:       how much the assist value can change in a single execution
 * ignoreThreshold: don't change assist at all if target is this close to current
 * returns:         level of assist to send to the steering ECU
 */
byte calculateSpeedSensitiveAssist(float mph, byte minAssist, byte maxAssist, byte maxChange, byte ignoreThreshold) {
  
  byte ret = oldAssistValue;
  byte targetAssist;
  
  //no adjustments made above 65mph or below 10mph
  if(mph > 65.0) {
    targetAssist = minAssist;
  }
  else if(mph < 10.0) {
    targetAssist = maxAssist;
  }
  else {
    targetAssist = map(mph, 10, 65, 0, 255);
  }
  if((targetAssist) > (oldAssistValue + maxChange)) {
    ret += maxChange;
  }
  else if((targetAssist) < (oldAssistValue - maxChange)) {
    ret -= maxChange;
  }
  else if (abs(targetAssist - oldAssistValue) < ignoreThreshold) {
    ret = oldAssistValue;
  }
  else {
    ret = targetAssist;
  }
  return ret;
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

  return val;  
}

void sendToPot(byte pos) {
  if(DEBUG_KNOB) {
    Serial.print("setting digital knob to position ");
    Serial.println(pos);  
  }
  //todo
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

