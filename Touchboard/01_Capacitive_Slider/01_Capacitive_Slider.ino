#include <MPR121.h>
#include <Wire.h>
#include "Compiler_Errors.h"

// mapping and filter definitions
#define LOW_DIFF 0
#define HIGH_DIFF 50
#define filterWeight 0.3f // 0.0f to 1.0f - higher value = more smoothing
float lastProx = 0;

// the electrode to monitor
#define ELECTRODE 0

 int lastreading = 0;
 
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  analogWrite(LED_BUILTIN, 0);

  Serial.begin(9600);
  while (!Serial); // uncomment to use serial monitor
  if (!MPR121.begin(0x5C)) {
    Serial.println("error setting up MPR121");
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

  // slow down some of the MPR121 baseline filtering to avoid
  // filtering out slow hand movements
  MPR121.setRegister(MPR121_NHDF, 0x01); //noise half delta (falling)
  MPR121.setRegister(MPR121_FDLF, 0x3F); //filter delay limit (falling)
}

void loop() {

  // update all of the data from the MPR121
  MPR121.updateAll();

  // read the difference between the measured baseline and the measured continuous data
  int reading = MPR121.getBaselineData(ELECTRODE) - MPR121.getFilteredData(ELECTRODE);
  int delta = lastreading - reading;

  // print out the reading value for debug
  Serial.println("Reading :");
  Serial.println(reading);
  Serial.println("Delta :");
  Serial.println(delta);

  // constrain the reading between our low and high mapping values
  unsigned int prox = constrain(reading, LOW_DIFF, HIGH_DIFF);

  // implement a simple (IIR lowpass) smoothing filter
  lastProx = (filterWeight * lastProx) + ((1 - filterWeight) * (float)prox);

  Serial.println("lastProx :");
  Serial.println(lastProx);
  // map the LOW_DIFF..HIGH_DIFF range to 0..255 (8-bit resolution for analogWrite)
  // uint8_t thisOutput = (uint8_t)map(lastProx,LOW_DIFF,HIGH_DIFF,0,255);

  // output the mapped value to the LED
  //analogWrite(LED_BUILTIN, thisOutput);
  lastreading = reading;

  delay(250);

}
