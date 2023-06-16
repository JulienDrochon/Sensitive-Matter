#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>

/*
   This example serves a "hello world" on a WLAN and a SoftAP at the same time.
   The SoftAP allow you to configure WLAN parameters at run time. They are not setup in the sketch but saved on EEPROM.

   Connect your computer or cell phone to wifi network ESP_ap with password 12345678. A popup may appear and it allow you to go to WLAN config. If it does not then navigate to http://192.168.4.1/wifi and config it there.
   Then wait for the module to connect to your wifi and take note of the WLAN IP it got. Then you can disconnect from ESP_ap and return to your regular WLAN.

   Now the ESP8266 is in your network. You can reach it through http://192.168.x.x/ (the IP you took note of) or maybe at http://esp8266.local too.

   This is a captive portal because through the softAP it will redirect any http request to http://192.168.4.1/
*/


/* Set these to your desired softAP credentials. They are not configurable at runtime */
#ifndef APSSID
#define APSSID "B-IO_settings_JD02"
#define APPSK "12345678"
#endif

const char *softAP_ssid = APSSID;
const char *softAP_password = APPSK;

/* hostname for mDNS. Should work at least on windows. Try http://esp8266.local */
const char *myHostname = "b-io_esp_jd02";

/* Don't set this wifi credentials. They are configurated at runtime and stored on EEPROM */
char ssid[33] = "";
char password[65] = "";
char oscIP[33] = "";

// DNS server
const byte DNS_PORT = 53;
DNSServer dnsServer;

// Web server
ESP8266WebServer server(80);

/* Soft AP network parameters */
IPAddress apIP(172, 217, 28, 1);
IPAddress netMsk(255, 255, 255, 0);


/** Should I connect to WLAN asap? */
boolean connect;

/** Last time I tried to connect to WLAN */
unsigned long lastConnectTry = 0;

/** Current WLAN status */
unsigned int status = WL_IDLE_STATUS;

/**Receive from Serial */
String inString = "";
//Analog Sensor Values
//char analogSplitters[6] = {'a', 'b', 'c', 'd', 'e', '<'};
int analogValues[6] = {};
//Proximity Sensor Values
//char proximitySplitters[12] = {'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', '#'};
//int ElectrodeIndex[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
int ElectrodeValues[12] = {};
//Touched Sensor Values

/** Osc */
WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
IPAddress outIp(0, 0, 0, 0);     // remote IP of your computer
const unsigned int outPort = 9999;          // remote port to receive OSC
const unsigned int localPort = 8888;        // local port to listen for OSC packets (actually not used for sending)
boolean StringReady = false;

void setup() {

  Serial.begin(115200);
  delay(1000);
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(softAP_ssid, softAP_password);
  delay(500);  // Without delay I've seen the IP address blank


  /* Setup the DNS server redirecting all the domains to the apIP */
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);

  /* Setup web pages: root, wifi config pages, SO captive portal detectors and not found. */
  server.on("/", handleRoot);
  server.on("/wifi", handleWifi);
  server.on("/wifisave", handleWifiSave);
  server.on("/osc", handleOsc);
  server.on("/oscsave", handleSaveOsc);
  server.on("/generate_204", handleRoot);  // Android captive portal. Maybe not needed. Might be handled by notFound handler.
  server.on("/fwlink", handleRoot);        // Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
  server.onNotFound(handleNotFound);
  server.begin();  // Web server start
  loadCredentials();           // Load WLAN credentials from network
  
   if (oscIP != "OK" && oscIP != "") {
   outIp.fromString(oscIP);
  }
  
  connect = strlen(ssid) > 0;  // Request WLAN connect if there is a SSID
  Udp.begin(localPort);
}

void connectWifi() {
  // Serial.println("Connecting as wifi client...");
  WiFi.disconnect();
  WiFi.begin(ssid, password);
  int connRes = WiFi.waitForConnectResult();
  // Serial.print("connRes: ");
  // Serial.println(connRes);
}

void loop() {
  if (connect) {
    // Serial.println("Connect requested");
    connect = false;
    connectWifi();
    lastConnectTry = millis();
  }
  {
    unsigned int s = WiFi.status();
    if (s == 0 && millis() > (lastConnectTry + 60000)) {
      /* If WLAN disconnected and idle try to connect */
      /* Don't set retry time too low as retry interfere the softAP operation */
      connect = true;
    }
    if (status != s) {  // WLAN status change
      //   Serial.print("Status: ");
      //   Serial.println(s);
      status = s;
      if (s == WL_CONNECTED) {
        // Setup MDNS responder
        if (!MDNS.begin(myHostname)) {
        } else {
          // Add service to MDNS-SD
          MDNS.addService("http", "tcp", 80);
        }
      } else if (s == WL_NO_SSID_AVAIL) {
        WiFi.disconnect();
      }
    }
    if (s == WL_CONNECTED) {
      MDNS.update();
    }
  }
  // Do work:
  // DNS
  dnsServer.processNextRequest();
  // HTTP
  server.handleClient();
  delay(1);
  if (outIp != IPAddress(0, 0, 0, 0) && WiFi.localIP()!= IPAddress(0, 0, 0, 0)) {
    StringReady = false;
    receiveFromSerial();
    if (StringReady) {
      AnalogSendToOsc(analogValues);
      // ToggleSendToOsc();
      ProximitySendToOsc(ElectrodeValues);
    }
  }
}
