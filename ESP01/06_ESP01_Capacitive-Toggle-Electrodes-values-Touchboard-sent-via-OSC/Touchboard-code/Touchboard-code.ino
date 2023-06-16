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

// touch behaviour definitions
#define firstPin 0
#define lastPin 11

// define LED_BUILTIN for older versions of Arduino
#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

void setup()
{
  mySerial.begin(9600);
  //Serial.begin(9600);
  delay(5000);

  pinMode(LED_BUILTIN, OUTPUT);

  //while (!Serial) ;
  if (!MPR121.begin(MPR121_ADDR)) {
    //Serial.println("error setting up MPR121");
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
  MPR121.setTouchThreshold(40);    // lower numbers make the board more sensitive
  MPR121.setReleaseThreshold(20);  // make sure that the release threshold is always
  // lower than the touch threshold

  MPR121.updateAll();
}

void loop() {
  //if (MPR121.touchStatusChanged()) {

    MPR121.updateTouchData();

    // only make an action if we have one or fewer pins touched
    // ignore multiple touches

    if (MPR121.getNumTouches() <= 1) {
      for (int i = 0; i < 12; i++) { // Check which electrodes were pressed
        if (MPR121.isNewTouch(i)) {
          mySerial.print(i);

          mySerial.print("|");
          mySerial.print("1");
          mySerial.print(">");
          digitalWrite(LED_BUILTIN, HIGH);
          //mySerial.print("<12, 24, 36>");

        } else {
          if (MPR121.isNewRelease(i)) {
            //sendToSerial(i, "0");
            mySerial.print(i);
            mySerial.print("|");
            mySerial.print("0");
            mySerial.print(">");
            digitalWrite(LED_BUILTIN, LOW);
          }
        }
      }
    }
 // }
  delay(10);

}

void sendToSerial(int electrode, String state) {
  mySerial.print("e");
  mySerial.print(electrode);
  mySerial.println();
}
