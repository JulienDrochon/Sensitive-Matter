/*---------------------------------------------------------------------------------------------

  Open Sound Control (OSC) library for the ESP8266/ESP32

  Example for sending messages from the ESP8266/ESP32 to a remote computer
  The example is sending "hello, osc." to the address "/test".

  This example code is in the public domain.

  Upload Speed : 115200
  Utiliser osc Shark sur l'ordinateur pour vérifier la bonne reception des message OSC
  https://github.com/hypebeast/OSCShark/releases/tag/v0.0.1
  --------------------------------------------------------------------------------------------- */
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <WiFiUdp.h>
#include <OSCMessage.h>

char ssid[] = "SFR-aea0";          // your network SSID (name)
char pass[] = "QITEL7AX2FEV";                    // your network password

//char ssid[] = "Pedagogie";          // your network SSID (name)
//char pass[] = "CRJ629%fZLm";                    // your network password

WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
//const IPAddress outIp(192, 168, 0, 20);     // remote IP of your computer
const IPAddress outIp(192,168,1,194);
const unsigned int outPort = 9999;          // remote port to receive OSC
const unsigned int localPort = 8888;        // local port to listen for OSC packets (actually not used for sending)

int val = 0;
String inString ="";
int stringToInteger;

void setup() {
  Serial.begin(9600);
  // Connect to WiFi network
  WiFi.begin(ssid, pass);
  Udp.begin(localPort);
  while (!Serial) {}
}

void loop() {

  boolean StringReady = false;


  while (Serial.available() > 0) {
    int inChar = Serial.read();
    StringReady = true;
    if (isDigit(inChar))inString += (char)inChar;
    if (inChar == '\n') {
      stringToInteger = inString.toInt();
      inString="";
    }

  }

  if (StringReady) {
    OSCMessage msg("/analog0");
    msg.add(stringToInteger);
    Udp.beginPacket(outIp, outPort);
    msg.send(Udp);
    Udp.endPacket();
    msg.empty();
  }
  delay(100);
}
