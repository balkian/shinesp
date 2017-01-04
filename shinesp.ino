#define FASTLED_ESP8266_RAW_PIN_ORDER
#include "FastLED.h"

#define NUM_LEDS 150
#define DATA_PIN 0
#define led 13

CRGB leds[NUM_LEDS];

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <ArduinoOTA.h>

const char* customssid = "led";
ESP8266WebServer httpServer(80);

struct credential {
  String ssid;
  String pass;
};

int connected = 0;

//
//bool tryNetworks(){
//  WiFi.mode(WIFI_STA);
//  int n = WiFi.scanNetworks();
//  Serial.println("scan done");
//  if (n == 0)
//    Serial.println("no networks found");
//  else
//  {
//    Serial.print(n);
//    Serial.println("networks found");
//    for (int i = 0; i < n; ++i) {
//      Serial.print("Trying network:");
//      Serial.println(WiFi.SSID(i));
//      for (int j=0; j< ssids.length; j++) {
//      // Print SSID and RSSI for each network found
//        if (WiFi.SSID(i) == ssids[j]){
//          WiFi.mode(WIFI_STA);
//          WiFi.begin(ssid, password);
//          if (WiFi.waitForConnectResult() == WL_CONNECTED) {
//            Serial.println("Connection Established!...");
//            return true;
//          }
//        }
//      }
//     }
//  }
//  Serial.println("Could not connect to any network");
//  return false
//}

bool connect(struct credential cred) {
  WiFi.mode(WIFI_STA);
  Serial.println("Trying to connect to " + cred.ssid + " with " + cred.pass);
  if (cred.ssid.length() < 1) {
    return false;
  }
  Serial.println("Length " + cred.ssid.length());
  WiFi.begin(cred.ssid.c_str(), cred.pass.c_str());
  int c = 0;
  while ( c < 20 ) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Ready");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      return true;
    }
    delay(500);
    Serial.print(WiFi.status());
    c++;
  }
  Serial.println("Connect timed out");
  return false;
}

void setCredentials(struct credential cred) {
  String qsid = cred.ssid;
  Serial.println(qsid);
  Serial.println("");
  String qpass = cred.pass;
  Serial.println(qpass);
  Serial.println("");

  Serial.println("writing eeprom ssid:");
  for (int i = 0; i < qsid.length(); ++i)
  {
    EEPROM.write(i, qsid[i]);
    Serial.print("Wrote: ");
    Serial.println(qsid[i]);
  }
  Serial.println("writing eeprom pass:");
  for (int i = 0; i < qpass.length(); ++i)
  {
    EEPROM.write(32 + i, qpass[i]);
    Serial.print("Wrote: ");
    Serial.println(qpass[i]);
  }
  EEPROM.commit();
}

void handleSetCredentials() {
  struct credential cred;
  cred.ssid = "";
  cred.pass = "";

  cred.ssid = httpServer.arg("ssid");
  cred.pass = httpServer.arg("pass");

  if (cred.ssid != "" && cred.pass != "") {
    setCredentials(cred);
    String out = "Credentials set as: " + cred.ssid + " : " + cred.pass;
    httpServer.send(200, "text/plain", out);
    ESP.reset();
  } else {
    httpServer.send(400, "text/plain", "Invalid credentials");
  }
}

void clearCredentials() {
  for (int i = 0; i < 96; ++i) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
}


void handleClearCredentials() {
  clearCredentials();
  httpServer.send(200, "text/plain", "Cleared!");
}

void handleWhite() {
  fill_solid(leds, NUM_LEDS, CRGB(255, 255, 255));
  FastLED.show();
  httpServer.send(200, "text/plain", "White!");
}

void handleColor() {
  int r = httpServer.arg("r").toInt();
  int g = httpServer.arg("g").toInt();
  int b = httpServer.arg("b").toInt();
  char msg[400];
  fill_solid( leds, NUM_LEDS, CRGB(r,g,b));
  FastLED.show();
  snprintf ( msg, 400, "RGB=(%d, %d, %d)", r, g, b);
  httpServer.send(200, "text/plain", msg);
}

void handleOff() {
  fill_solid( leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  httpServer.send(200, "text/plain", "Off!");
}

void handleBrightness() {
  int value = httpServer.arg("value").toInt();
  if (value > 0) {
    FastLED.setBrightness(value);
    httpServer.send(200, "text/plain", "Brightness set to " + String(value));
    FastLED.show();

  } else {
    httpServer.send(400, "text/plain", "Invalid value");
  }

}

void handleRoot() {
  digitalWrite ( led, 1 );
  char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf ( temp, 400,

             "<html>\
    <head>\
      <meta http-equiv='refresh' content='5'/>\
      <title>ESP8266 Demo</title>\
      <style>\
        body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
      </style>\
    </head>\
    <body>\
      <h1>Hello from ESP8266!</h1>\
      <p>Uptime: %02d:%02d:%02d</p>\
      <img src=\"/test.svg\" />\
    </body>\
  </html>",

             hr, min % 60, sec % 60
           );
  httpServer.send ( 200, "text/html", temp );
  digitalWrite ( led, 0 );
}

void setupAP() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  WiFi.softAP(customssid);
  Serial.println("softap");
  Serial.println("");
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
}

void handleLoop() {
  for (int dot = 0; dot < NUM_LEDS; dot++) {
    leds[dot] = CRGB::Blue;
    FastLED.show();
    // clear this led for the next time around the loop
    leds[dot] = CRGB::Black;
    delay(30);
  }
  httpServer.send ( 404, "text/plain", "Done!" );
}


struct credential getCredentials() {
  Serial.println("Getting credentials from EEPROM");
  struct credential cred;
  cred.ssid = "";
  cred.pass = "";
  for (int i = 0; i < 32; ++i)
  {
    cred.ssid += char(EEPROM.read(i));
  }
  Serial.print("SSID: ");
  Serial.println(cred.ssid);
  Serial.println("Reading EEPROM pass");
  for (int i = 32; i < 96; ++i)
  {
    cred.pass += char(EEPROM.read(i));
  }
  Serial.print("PASS: ");
  Serial.println(cred.pass);
  return cred;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");

  EEPROM.begin(512);
  delay(10);
  WiFi.mode(WIFI_STA);

  struct credential cred = getCredentials();
  if (!connect(cred)) {
    setupAP();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("led");

  // No authentication by default
  ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();




  if ( MDNS.begin ( "esp8266" ) ) {
    Serial.println ( "MDNS responder started" );
  }
  MDNS.addService("http", "tcp", 80);
  httpServer.on ( "/loop", handleLoop );
  httpServer.on ( "/", handleRoot );
  httpServer.on ( "/clear", handleClearCredentials );
  httpServer.on ( "/credentials", handleSetCredentials );
  httpServer.on ( "/white", handleWhite );
  httpServer.on ( "/color", handleColor );
  httpServer.on ( "/off", handleOff );
  httpServer.on ( "/brightness", handleBrightness );
  httpServer.begin();

  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
//  FastLED.setDither(0);
  FastLED.setBrightness(100);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 20000);
}

void loop() {
  ArduinoOTA.handle();
  httpServer.handleClient();
  FastLED.show();
}
