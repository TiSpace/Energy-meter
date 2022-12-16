
//=======================================================================
//                    WiFi starten
//=======================================================================
void initWiFi() {

  WiFiManager wifiMulti;
  //reset settings - for testing
  //wifiMulti.resetSettings();


  Serial.println("Connecting ...");
  int i = 0;
  //while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
  while (!wifiMulti.autoConnect("Energiemesser")) { // Wait for the Wi-Fi to connect
    delay(250);
    Serial.print('.');
  }
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to the computer


  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }


  if (WiFi.status() == WL_CONNECTED) //If WiFi connected to hot spot then start mDNS
  {
    if (MDNS.begin(mDNSName)) {  //Start mDNS with name esp8266  //https://circuits4you.com/2017/12/31/esp8266-mdns/
      Serial.print("MDNS started, Name: ");
      Serial.println(mDNSName);
    }
  }

  server.begin();

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

}

////=======================================================================
////                    WiFi starten
////=======================================================================
//void initWiFi_konventionell() {
//  // We start by connecting to a WiFi network
//  Serial.print("Connecting to ");
//  Serial.println(ssid);
//  // Set WiFi to station mode and disconnect from an AP if it was previously connected
//  WiFi.mode(WIFI_STA);
//  WiFi.disconnect();
//  WiFi.begin(ssid, password);
//  delay(100);
//
//  while (WiFi.status() != WL_CONNECTED)
//  {
//    delay(500);
//    Serial.print(".");
//  }
//
//
//  if (WiFi.status() == WL_CONNECTED) //If WiFi connected to hot spot then start mDNS
//  {
//    if (MDNS.begin(mDNSName)) {  //Start mDNS with name esp8266  //https://circuits4you.com/2017/12/31/esp8266-mdns/
//      Serial.print("MDNS started, Name: ");
//      Serial.println(mDNSName);
//    }
//  }
//
//  server.begin();
//
//  Serial.println("");
//  Serial.println("WiFi connected");
//  Serial.print("IP address: ");
//  Serial.println(WiFi.localIP());
//
//}


//=======================================================================
//                    Content-Management
//=======================================================================
String getContentType(String filename) { // convert the file extension to the MIME type
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain";
}

//=======================================================================
//                    FileRead handler
//=======================================================================
bool handleFileRead(String path) { // send the right file to the client (if it exists)

  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";         // If a folder is requested, send the index file
  String contentType = getContentType(path);            // Get the MIME type

  if (SPIFFS.exists(path)) {                            // If the file exists
    File file = SPIFFS.open(path, "r");                 // Open it
    size_t sent = server.streamFile(file, contentType); // And send it to the client
    file.close();                                       // Then close the file again
    return true;
  } else {
    Serial.println("\tFile Not Found");
    return false;                                         // If the file doesn't exist, return false
  }
}

//=======================================================================
//                    Page-Handler
//=======================================================================
void pageHandler() {
  //https://fipsok.de/Esp8266-Webserver/ds18b20-list-esp8266.tab    https://fipsok.de/Esp8266-Webserver/ds18b20-esp8266.tab

  server.on("/messen", getReading);
  server.on("/setup", getSetting);
  server.on("/modified", doModification);

}


//=======================================================================
//                    Page-Handler case Modification
//=======================================================================
void doModification() {
  // https://techtutorialsx.com/2016/10/22/esp8266-webserver-getting-query-parameters/

  for (int i = 0; i < server.args(); i++) {
    if (server.argName(i) == "interval")
    {
      sendToThingspeak = atof(server.arg(i).c_str());
      writeIntIntoEEPROM(sendToThingspeakEEPROMAdr, sendToThingspeak);
      EEPROM.commit();
    }
    if (server.argName(i) == "storeTS")
    {
      if (server.arg(i) == "true") {
        sendToThingspeakEnable = 1;
      }
      else {
        sendToThingspeakEnable = 0;
      }
      EEPROM.write(sendToThingspeakEEPROMAdr + 2, sendToThingspeakEnable);
      EEPROM.commit();
    }


    if (server.argName(i) == "reset")
    {
      if (server.arg(i) == "1")
      {
        Serial.println("RESET Energy counter");
#ifndef debugMode
        pzem.resetEnergy();
#endif
      }
    }
  }

  server.send(200, "application/json", "ok");//buf);
  //https://werner.rothschopf.net/201809_arduino_esp8266_server_client_5.htm
  // https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WebServer/README.rst    arg - get request argument value, use arg("plain") to get POST body
  // https://github.com/esp8266/Arduino/issues/4517
  //https://stackoverflow.com/questions/14551194/how-are-parameters-sent-in-an-http-post-request
}


//=======================================================================
//                    Page-Handler case Read Setting
//=======================================================================
void getSetting() {
  String payload = "[" ;
  String payloadSetup = "[" ;
  payloadSetup +=  "\"" + (String)sendToThingspeak + "\",";
  payloadSetup +=  "\"" + (String)sendToThingspeakEnable + "\"]";

  server.send(200, "application/json", payloadSetup);
  Serial.print("\t\tschicke JON Daten");
  Serial.println(payloadSetup);
}


//=======================================================================
//                    Page-Handler case Reading
//=======================================================================
void getReading() {
  readMeasure(); //uodate measure

  String payload = "[" ;
  payload +=  "\"" + (String)Messwerte.voltage + "\",";
  payload +=  "\"" + (String)Messwerte.current + "\",";
  payload +=  "\"" + (String)Messwerte.power + "\",";
  payload +=  "\"" + (String)Messwerte.energy + "\",";
  payload +=  "\"" + (String)Messwerte.frequency + "\",";
  payload +=  "\"" + (String)Messwerte.pf + "\",";
  payload +=  "\"" + (String)__DATE__ + " at " + (String)__TIME__ + "\",";
  payload +=  "\"" + myFile + "\",";
  //payload +=  "\"" + (String)__FILENAME__ + "\",";
  payload +=  "\"" + (String)WiFi.macAddress() + "\",";
  payload +=  "\"" + (String)WiFi.RSSI() + "\",";
  payload +=  "\"" + (String)WiFi.SSID() + "\",";
  payload +=  "\"" + WiFi.localIP().toString() + "\"]";

  server.send(200, "application/json", payload);
  Serial.print("\t\tschicke JON Daten");
  Serial.println(payload);


}
