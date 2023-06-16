// compiler error handling
#include "Compiler_Errors.h"

// touch includes
#include <MPR121.h>
#include <MPR121_Datastream.h>
#include <Wire.h>

// touch constants
const uint32_t BAUD_RATE = 115200;
const uint8_t MPR121_ADDR = 0x5C;
const uint8_t MPR121_INT = 4;

// serial monitor behaviour constants
const bool WAIT_FOR_SERIAL = false;

// MPR121 datastream behaviour constants
const bool MPR121_DATASTREAM_ENABLE = true;

void setup() {
  Serial.begin(BAUD_RATE);
  pinMode(LED_BUILTIN, OUTPUT);

  if (WAIT_FOR_SERIAL) {
    while (!Serial);
  }

if (!MPR121.begin(MPR121_ADDR)) {
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

  MPR121.setInterruptPin(MPR121_INT);

  if (MPR121_DATASTREAM_ENABLE) {
    MPR121.restoreSavedThresholds();
    MPR121_Datastream.begin(&Serial);
  } else {
    MPR121.setTouchThreshold(40);
    MPR121.setReleaseThreshold(20);
  }

    MPR121.setFFI(FFI_10);
  MPR121.setSFI(SFI_10);
  MPR121.setGlobalCDT(CDT_4US);  // reasonable for larger capacitances

  digitalWrite(LED_BUILTIN, HIGH);  // switch on user LED while auto calibrating electrodes
  delay(1000);
  MPR121.autoSetElectrodes();  // autoset all electrode settings
  digitalWrite(LED_BUILTIN, LOW);

  
}

void loop() {
  MPR121.updateAll();

  // only make an action if we have one or fewer pins touched
  // ignore multiple touches
  if (MPR121.getNumTouches() <= 1) {
    for (int i=0; i < 12; i++) {  // check which electrodes were pressed
      if (MPR121.isNewTouch(i)) {
          

          digitalWrite(LED_BUILTIN, HIGH);

    
      } else {
        if (MPR121.isNewRelease(i)) {
     

          digitalWrite(LED_BUILTIN, LOW);
        }
      }
    }
  }

  if (MPR121_DATASTREAM_ENABLE) {
    MPR121_Datastream.update();
  }
}
