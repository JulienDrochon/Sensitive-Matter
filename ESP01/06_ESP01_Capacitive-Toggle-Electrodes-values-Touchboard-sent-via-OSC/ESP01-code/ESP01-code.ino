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
#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <ESP8266WiFiMulti.h>   // Include the Wi-Fi-Multi library
#include <ESP8266mDNS.h>        // Include the mDNS library

ESP8266WiFiMulti wifiMulti;

//OSC
WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
const IPAddress outIp(192, 168, 0, 20);     // remote IP of your computer
const unsigned int outPort = 9999;          // remote port to receive OSC
const unsigned int localPort = 8888;        // local port to listen for OSC packets (actually not used for sending)

String inString = "";
String stringToInteger;
String ElectrodeIndex = "";
int ElectrodeValue = 0;


void setup() {
  Serial.begin(9600);

  wifiMulti.addAP("SFR-aea0", "QITEL7AX2FEV");   // add Wi-Fi networks you want to connect to
  wifiMulti.addAP("Pedagogie", "CRJ629%fZLm");

  while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(1000);
  }
  Udp.begin(localPort);
  while (!Serial) {}
}

void loop() {

  boolean StringReady = false;
  //
  while (Serial.available() > 0 ) {
    char inChar = Serial.read();
    //int testInt = Serial.parseInt();

    if (isDigit(inChar))inString += (char)inChar;
    if (inChar == '|') {
      ElectrodeIndex = inString;
      inString = "";

    }
    if (inChar == '>') {
      ElectrodeValue = inString.toInt();
      inString = "";
      StringReady = true;
    }
  }
  if (StringReady) {
    ToggleSsendToOsc(ElectrodeIndex, ElectrodeValue );
  }
  delay(10);
}

void ToggleSsendToOsc(String indElectrode, int valElectrode) {
  String addr_prefix = "/toggle/";
  String addr_suffix = indElectrode;
  String completeaddress = addr_prefix + addr_suffix;
  const char * addr = completeaddress.c_str();

  OSCMessage msg(addr);
  msg.add(valElectrode);
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
  delay(10);
}
