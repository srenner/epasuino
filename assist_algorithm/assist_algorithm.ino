void setup() {
  Serial.begin(9600);
}

void loop() {
  //version1();
  version2();
  Serial.println("=============");
  delay(20000);

}

void version1() {
  //linear progression from 5mph to 65mph
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
byte previousVal = 255;
void version2() {
  //springy linear progression
  byte val = 255;
  float mph = 0.0;
  for(byte i = 0; i < 100; i++) {
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

    if((val - previousVal) > 3) {
      val = previousVal + 3;
    }
    else if((previousVal - val) > 3) {
      val = previousVal - 3;
    }
    else {
      val = previousVal;
    }
    previousVal = val;
    Serial.print("assist level ");
    Serial.print(val);
    Serial.print(" @ ");
    Serial.print(trueMPH);
    Serial.println(" mph");
    delay(150);
  }
  
  
}

