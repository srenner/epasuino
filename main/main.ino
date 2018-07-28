volatile byte ledState = HIGH;
unsigned long duration;
volatile unsigned long pulseCount;
const byte interruptPin = 9;

void setup() {

  //turn on internal LED for no real reason
  pinMode(LED_BUILTIN, OUTPUT);


  
  digitalWrite(LED_BUILTIN, ledState);

 
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(9, handlePulse, CHANGE);
  pulseCount = 0;

  
  Serial.begin(9600);
  Serial.println("setting up");
}

void loop() {
  // put your main code here, to run repeatedly:
  //digitalWrite(LED_BUILTIN, ledState);


  duration = pulseIn(interruptPin, HIGH);

  //Serial.println(duration);

  Serial.println(pulseCount);
  delay(100);
  
}

void handlePulse() {
  //Serial.println("handling pulse");
  //ledState = !ledState;
  pulseCount++;
}

