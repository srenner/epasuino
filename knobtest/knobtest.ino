byte const POS_1_PIN = 2;
byte const POS_2_PIN = 3;
byte const POS_3_PIN = 4;
byte const POS_4_PIN = 5;
byte const POS_5_PIN = 6;
byte const POS_6_PIN = 7;

void setup() {
  Serial.begin(9600);
  pinMode(POS_1_PIN, INPUT_PULLUP);
  pinMode(POS_2_PIN, INPUT_PULLUP);
  pinMode(POS_3_PIN, INPUT_PULLUP);
  pinMode(POS_4_PIN, INPUT_PULLUP);
  pinMode(POS_5_PIN, INPUT_PULLUP);
  pinMode(POS_6_PIN, INPUT_PULLUP);
}

void loop() {
  if(!digitalRead(POS_1_PIN)) {
    Serial.println(1);
  }
  else if(!digitalRead(POS_2_PIN)) {
    Serial.println(2);
  }
  else if(!digitalRead(POS_3_PIN)) {
    Serial.println(3);
  }
  else if(!digitalRead(POS_4_PIN)) {
    Serial.println(4);
  }
  else if(!digitalRead(POS_5_PIN)) {
    Serial.println(5);
  }
  else if(!digitalRead(POS_6_PIN)) {
    Serial.println(6);
  }
}
