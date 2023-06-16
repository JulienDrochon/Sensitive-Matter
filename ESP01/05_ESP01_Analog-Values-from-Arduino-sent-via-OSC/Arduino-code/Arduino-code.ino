#include <SoftwareSerial.h>
SoftwareSerial mySerial(4, 5); // RX, TX

char analogSplitters[6] = {'a', 'b', 'c', 'd', 'e', '<'};
int analogValues[6] = {};

void setup()
{
  mySerial.begin(115200);
  delay(5000);
}

void loop() {
  for (int i = 0; i < sizeof(analogValues); i++;) {
    mySerial.print(analogRead(i));
    mySerial.print(analogSplitters[i]);
  }
 
  delay(10);
}
