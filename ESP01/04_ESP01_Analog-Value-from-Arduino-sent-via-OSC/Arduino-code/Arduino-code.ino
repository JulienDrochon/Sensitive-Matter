#include <SoftwareSerial.h>
SoftwareSerial mySerial(4, 5); // RX, TX

void setup()
{
  Serial.begin(9600);
  mySerial.begin(9600);
  delay(5000);
}

void loop() {
  mySerial.println(analogRead(A0));
  delay(100);
}
