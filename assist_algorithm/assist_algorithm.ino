void setup() {
  Serial.begin(9600);
}

void loop() {
  version1();
  Serial.println("=============");
  delay(10000);

}

void version1() {
  float mph = 0.0;
  for(byte i = 0; i < 80; i++) {
    float trueMPH = (float)i;
    mph = (float)i;
    

    byte val = 255;
    if(mph > 65.0) {
      mph = 65.0;
    }
    else if(mph <= 5.0) {
      mph = 0.0;
    }
    byte subtractBy = 255 - map(65.0 - mph, 0, 65, 0, 255);
    val = val - subtractBy;

    Serial.print("assist level ");
    Serial.print(val);
    Serial.print(" @ ");
    Serial.print(trueMPH);
    Serial.println(" mph");

    delay(350);
    
  }
}

