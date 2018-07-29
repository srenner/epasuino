int ledPin = 13;
int sensePin = 8;
volatile int value = 0;

// Install the interrupt routine.
ISR(INT6_vect) {
    value = digitalRead(sensePin);
}

void setup() {
  Serial.begin(9600);
  Serial.println("Initializing ihandler");
  pinMode(ledPin, OUTPUT);
  pinMode(sensePin, INPUT);
  Serial.println("Processing initialization");

  // Global Enable INT0 interrupt
  EIMSK |= ( 1 << INT6);
  // Signal change triggers interrupt
  MCUCR |= ( 1 << ISC00);
  MCUCR |= ( 0 << ISC01);
  Serial.println("Finished initialization");
}

void loop() {
  if (value) {
    Serial.println("Value high!");
    digitalWrite(ledPin, HIGH);
  } else {
    Serial.println("Value low!");
    digitalWrite(ledPin, LOW);
  }
  delay(100);
}

