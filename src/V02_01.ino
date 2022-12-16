/*
  orientieren an https://fipsok.de/Esp8266-Webserver/ds18b20-list-esp8266.tab für Datenübergabe

  Setting:

  Generic ESP8266 Module
  Flash Size 1MBk FS: 64kB  OTA: ~470kB

  Historie: 2.01 zusätzlich OTA (benötigt 1MB Variante!)
*/


//#define debugMode
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <PZEM004Tv30.h>
#include <SoftwareSerial.h>
#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include "setting.h"
#include <FS.h>   // Include the SPIFFS library
#include <EEPROM.h>
#include <ArduinoOTA.h>

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__) //to get Arduino Filename extracted from __FILE__




int status = WL_IDLE_STATUS;
WiFiClient  client;
WiFiManager wifiManager;
ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80

SoftwareSerial pzemSWSerial(PZEM_RX_PIN, PZEM_TX_PIN);
PZEM004Tv30 pzem(pzemSWSerial);



String getContentType(String filename); // convert the file extension to the MIME type
bool handleFileRead(String path);       // send the right file to the client (if it exists)

String myFile;

//String Argument_Name, Clients_Response;

void setup() {
  /* Debugging serial */
  Serial.begin(BAUDRATE);
  while (!Serial) {} // wiat until Serial is ready





  // hole Werte aus EEPROM
  EEPROM.begin(100); // Puffergröße die verwendet werden soll
  sendToThingspeak = readIntFromEEPROM(sendToThingspeakEEPROMAdr);
  if (sendToThingspeak < 30) sendToThingspeak = 71;
  sendToThingspeakEnable = EEPROM.read(sendToThingspeakEEPROMAdr + 2);



  initWiFi();
  provideOTA();
  versionsInfo();
  ThingSpeak.begin(client);

  SPIFFS.begin();                           // Start the SPI Flash Files System

  server.onNotFound([]() {                              // If the client requests any URI
    if (!handleFileRead(server.uri()))                  // send it if it exists
      server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
  });

  pageHandler();  //how to handle page requests

  myFile = __FILENAME__;
}

void loop() {

  MDNS.update(); // muss aktivi sein!
  server.handleClient();
ArduinoOTA.handle();
  static unsigned long lastUpdateThingspeak = 0;

  if ((millis() - lastUpdateThingspeak) >   (1000 * sendToThingspeak)) {
    lastUpdateThingspeak = millis();
    readMeasure();

    if (sendToThingspeakEnable == 1) {  // storage to Thingspeak enabled?
#ifndef debugMode
      ThingSpeak.setField(1, Messwerte.voltage);
      ThingSpeak.setField(2, Messwerte.current);
      ThingSpeak.setField(3, Messwerte.power);
      ThingSpeak.setField(4, Messwerte.energy);
      ThingSpeak.setField(5, Messwerte.frequency);
      ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
      Serial.println("Update Thingspeak!");
#endif
    }
  }

  yield();

}


void readMeasure() {

  Serial.print("Custom Address:");
  Serial.println(pzem.readAddress(), HEX);

#ifndef debugMode
  // Read the data from the sensor
  Messwerte.voltage = pzem.voltage();
  Messwerte.current = pzem.current();
  Messwerte.power = pzem.power();
  Messwerte.energy = pzem.energy();
  Messwerte.frequency = pzem.frequency();
  Messwerte.pf = pzem.pf();
#else

  // Read the data from the sensor
  Messwerte.voltage = 227.01; //pzem.voltage();
  Messwerte.current = 0.874; //pzem.current();
  Messwerte.power = 17;//pzem.power();
  Messwerte.energy = 0.456; //pzem.energy();
  Messwerte.frequency = 49.99;//pzem.frequency();
  Messwerte.pf = 0.97; // pzem.pf();
#endif


  // Check if the data is valid
  if (isnan(Messwerte.voltage)) {
    Serial.println("Error reading voltage");
  } else if (isnan(Messwerte.current)) {
    Serial.println("Error reading current");
  } else if (isnan(Messwerte.power)) {
    Serial.println("Error reading power");
  } else if (isnan(Messwerte.energy)) {
    Serial.println("Error reading energy");
  } else if (isnan(Messwerte.frequency)) {
    Serial.println("Error reading frequency");
  } else if (isnan(Messwerte.pf)) {
    Serial.println("Error reading power factor");
  } else {

    // Print the values to the Serial console
    Serial.print("Voltage: ");      Serial.print(Messwerte.voltage);      Serial.println("V");
    Serial.print("Current: ");      Serial.print(Messwerte.current);      Serial.println("A");
    Serial.print("Power: ");        Serial.print(Messwerte.power);        Serial.println("W");
    Serial.print("Energy: ");       Serial.print(Messwerte.energy, 3);     Serial.println("kWh");
    Serial.print("Frequency: ");    Serial.print(Messwerte.frequency, 1); Serial.println("Hz");
    Serial.print("PF: ");           Serial.println(Messwerte.pf);
  }

  Serial.println();
}

//=======================================================================
//                    Versionsinfo
//=======================================================================
void versionsInfo()
{
  Serial.print("\nArduino is running Sketch: ");
  Serial.println(__FILE__);
  Serial.print("Compiled on: ");
  Serial.print(__DATE__);
  Serial.print(" at ");
  Serial.print(__TIME__);
  Serial.print("\n\n");
}
