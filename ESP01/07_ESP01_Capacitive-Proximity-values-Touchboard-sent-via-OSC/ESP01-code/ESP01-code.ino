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
const IPAddress outIp(192, 168, 0, 14);     // remote IP of your computer
//const IPAddress outIp(192, 168, 1, 188);
const unsigned int outPort = 9999;          // remote port to receive OSC
const unsigned int localPort = 8888;        // local port to listen for OSC packets (actually not used for sending)

String inString = "";
char proximitySplitters[12] = {'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', '#'};
int ElectrodeIndex[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
int ElectrodeValues[12] = {};

void setup() {
  Serial.begin(115200);

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
    for (int i = 0; i < sizeof(ElectrodeValues); i++) {
      if (inChar == '#') {
        ElectrodeValues[i] = inString.toInt();
        inString = "";
        StringReady = true;
      } else if (inChar == proximitySplitters[i]) {
        ElectrodeValues[i] = inString.toInt();
        inString = "";
      }
    }
    /* if (inChar == 'l') {
       ElectrodeValues[0] = inString.toInt();
       inString = "";
      }

      if (inChar == 'm') {
       ElectrodeValues[1] = inString.toInt();
       inString = "";
      }
      if (inChar == 'n') {
       ElectrodeValues[2] = inString.toInt();
       inString = "";
      }
      if (inChar == 'o') {
       ElectrodeValues[3] = inString.toInt();
       inString = "";
      }
      if (inChar == 'p') {
       ElectrodeValues[4] = inString.toInt();
       inString = "";
      }
      if (inChar == 'q') {
       ElectrodeValues[5] = inString.toInt();
       inString = "";
      }
      if (inChar == 'r') {
       ElectrodeValues[6] = inString.toInt();
       inString = "";
      }
      if (inChar == 's') {
       ElectrodeValues[7] = inString.toInt();
       inString = "";
      }
      if (inChar == 't') {
       ElectrodeValues[8] = inString.toInt();
       inString = "";
      }
      if (inChar == 'u') {
       ElectrodeValues[9] = inString.toInt();
       inString = "";
      }
      if (inChar == 'v') {
       ElectrodeValues[10] = inString.toInt();
       inString = "";
      }
      if (inChar == '#') {
       ElectrodeValues[11] = inString.toInt();
       inString = "";
       StringReady = true;
      }*/
  }
  if (StringReady) {
    ProximitySendToOsc(ElectrodeIndex, ElectrodeValues);
  }
  delay(1);
}

void ProximitySendToOsc(int indElectrode[], int valElectrode[]) {
  for (byte i = 0; i < sizeof(indElectrode); i++) {
    String addr_prefix = "/proximity/";
    String addr_suffix = String(indElectrode[i]);
    String completeaddress = addr_prefix + addr_suffix;
    const char * addr = completeaddress.c_str();

    OSCMessage msg(addr);
    msg.add(valElectrode[i]);
    Udp.beginPacket(outIp, outPort);
    msg.send(Udp);
    Udp.endPacket();
    msg.empty();
    delay(10);
  }
}
