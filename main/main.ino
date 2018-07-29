int ledPin = 13;
int sensePin = 9;
volatile int previousValue = 0;
volatile byte newValue = 1;
volatile unsigned long counter = 0;

// Install the interrupt routine.
ISR(INT7_vect) {
  newValue = !newValue;
  counter++;
}

void setup() {
  Serial.begin(9600);
  //Serial.println("Initializing ihandler");
  pinMode(ledPin, OUTPUT);
  pinMode(sensePin, INPUT_PULLUP);
  //Serial.println("Processing initialization");

    
  // Signal change triggers interrupt
  EICRB |= ( 1 << ISC71);
  EICRB |= ( 0 << ISC70);

  // Global Enable INT6 interrupt
  EIMSK |= ( 1 << INT7);

  
  Serial.println("Finished initialization");
}

void loop() {
  //digitalWrite(ledPin, newValue);
  Serial.println(counter);
  delay(250);
}

