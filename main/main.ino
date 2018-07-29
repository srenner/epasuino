int ledPin = 13;
int vssPin = 9; //pin 9 corresponds to interrupt 7 on the chip
volatile unsigned long vssCounter = 0;

//interrupt routine for interrupt 7 (pin 9)
ISR(INT7_vect) {
  vssCounter++;
}

void setup() {
  Serial.begin(9600);
  //Serial.println("Initializing ihandler");
  pinMode(ledPin, OUTPUT);
  pinMode(vssPin, INPUT_PULLUP);
    
  //set trigger for interrupt 7 (pin 9) to be falling edge
  EICRB |= ( 1 << ISC71);
  EICRB |= ( 0 << ISC70);

  //enable interrupt 7 (pin 9)
  EIMSK |= ( 1 << INT7);
  
  Serial.println("Finished initialization");
}

void loop() {
  Serial.println(vssCounter);
  delay(250);
}

