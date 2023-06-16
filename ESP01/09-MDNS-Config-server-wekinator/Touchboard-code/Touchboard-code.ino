// touch  includes
#include <MPR121.h>
#include <Wire.h>
#define MPR121_ADDR 0x5C
#define MPR121_INT 4
// serial port communication
#include <SoftwareSerial.h>
SoftwareSerial mySerial(4, 5); // RX, TX

#define holdKey true  // set this to false if you want to have a single quick keystroke
// true means the key is pressed and released when you press and release the electrode respectively


// define LED_BUILTIN for older versions of Arduino
#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

char proximitySplitters[12] = {'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', '#'};
//const int analogPins[6] = {0, 1, 2, 3, 4, 5};
char analogSplitters[6] = {'a', 'b', 'c', 'd', 'e', '<'};

void setup()
{
  mySerial.begin(115200);
  delay(5000);

  pinMode(LED_BUILTIN, OUTPUT);

  if (!MPR121.begin(MPR121_ADDR)) {
    switch (MPR121.getError()) {
      case NO_ERROR:
        Serial.println("no error");
        break;
      case ADDRESS_UNKNOWN:
        Serial.println("incorrect address");
        break;
      case READBACK_FAIL:
        Serial.println("readback failure");
        break;
      case OVERCURRENT_FLAG:
        Serial.println("overcurrent on REXT pin");
        break;
      case OUT_OF_RANGE:
        Serial.println("electrode out of range");
        break;
      case NOT_INITED:
        Serial.println("not initialised");
        break;
      default:
        Serial.println("unknown error");
        break;
    }
    while (1);
  }

  MPR121.setInterruptPin(MPR121_INT);

  MPR121.updateFilteredData();

  // this is the touch threshold - setting it low makes it more like a proximity trigger
  // default value is 40 for touch
 MPR121.setTouchThreshold(1);
  
  // this is the release threshold - must ALWAYS be smaller than the touch threshold
  // default value is 20 for touch
  MPR121.setReleaseThreshold(0);  
  
  // this restores thresholds saved in EEPROM
  // MPR121.restoreSavedThresholds();
}

void loop() {
 MPR121.updateAll();
 
  for (int i = 0; i < 6; i++) {
    mySerial.print(analogRead(i));
    mySerial.print(analogSplitters[i]);
  }


  for (int i = 0; i < 12; i++) {
    mySerial.print(MPR121.getFilteredData(i));
    mySerial.print(proximitySplitters[i]);
  }


  delay(10); //delay(10)

}
