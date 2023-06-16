
void receiveFromSerial() {


  //Receive data from Touchboard Serial Port
  while (Serial.available() > 0 ) {
    char inChar = Serial.read();
    if (isDigit(inChar))inString += (char)inChar;

    //Analog Pins Values

  
    if (inChar == 'a') {
      analogValues[0] = inString.toInt();
      inString = "";
    }
    if (inChar == 'b') {
      analogValues[1] = inString.toInt();
      inString = "";
    }
    if (inChar == 'c') {
      analogValues[2] = inString.toInt();
      inString = "";
    }
    if (inChar == 'd') {
      analogValues[3] = inString.toInt();
      inString = "";
    }
    if (inChar == 'e') {
      analogValues[4] = inString.toInt();
      inString = "";
    }
      if (inChar == '<') {
      analogValues[5] = inString.toInt();
      inString = "";
     // StringReady = true;
    }

    if (inChar == 'l') {
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
    }


  }

}



void ProximitySendToOsc(int valElectrode[]) {
  for (byte i = 0; i < 12; i++) {
    String addr_prefix = "/proximity/";
    String addr_suffix = String(i);
    String completeaddress = addr_prefix + addr_suffix;
    const char * addr = completeaddress.c_str();

    OSCMessage msg(addr);
    msg.add(valElectrode[i]);
    Udp.beginPacket(outIp, outPort);
    msg.send(Udp);
    Udp.endPacket();
    msg.empty();
    delay(5); //delay(10)
  }
}

/*
  void ToggleSendToOsc(String indElectrode, int valElectrode) {
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
  delay(1); //delay(10)
  }
*/

void AnalogSendToOsc(int valAnalog[]) {
  for (int i = 0; i < 6; i++) {
    String addr_prefix = "/analog/";
    String addr_suffix = String(i);
    String completeaddress = addr_prefix + addr_suffix;
    const char * addr = completeaddress.c_str();

    OSCMessage msg(addr);
    msg.add(valAnalog[i]);
    Udp.beginPacket(outIp, outPort);
    msg.send(Udp);
    Udp.endPacket();
    msg.empty();
    delay(5); //delay(10)
  }

}
