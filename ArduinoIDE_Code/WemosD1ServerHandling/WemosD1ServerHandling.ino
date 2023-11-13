#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "secret.h"

//Display Imports
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Define Display values
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64


//Construct Adafruit SSD1306
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//Wifi information. bases itself off of secret.h
const char* ssid = SECRET_SSID;
const char* password = SECRET_WPA;
const char* unlock_key = SECRET_KEY;

//Init server
ESP8266WebServer server(80);

//Could consider adding static ip-addressing here? Remember to change per-device in that case

//handleRoot happens whenever the server gets a GET-Request
void handleRoot() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(401, "text/plain", "This isn't how you're supposed to use this device!");
  blink();
  display.setCursor(2, 10);
  display.clearDisplay();
  display.println("GET-Received");
  display.display();
}

//function which sole purpose is to display information
void displayStatus() {
    //On Connect: 
    display.clearDisplay();
    display.println("Connected to Wifi: ");
    display.println(WiFi.localIP());
    display.display();
    Serial.println("Connected to WiFi. IP-address: ");
    Serial.println(WiFi.localIP()); 
    String IP = WiFi.localIP().toString();
    if (IP.startsWith("169.254")) {
      display.clearDisplay();
      display.println("Received invalid IP-address. Attempting to restart..");
      display.display();
      delay(10000);
      setup();
    }

    }
    
//HandlePost happens whenever the servers gets a POST-request
void handlePost() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  display.setCursor(0, 10);
  display.clearDisplay();
  String postData = server.arg("plain");
  if(postData.indexOf(unlock_key) != -1) {
    server.send(200, "text/plain", "correct code");
    display.println("Correct code!");
    display.display();
    delay(10000);
    displayStatus();
  }
  else {
    server.send(401, "text/plain", "NOT AUTHORIZED");
    display.println("POST-Received but not authenticated");
    display.display();
    delay(10000);
    displayStatus();
  }
}

//Function for blinking BUILT-IN LED
void blink() {
  digitalWrite (BUILTIN_LED, LOW); 
  delay (900); 
  digitalWrite(BUILTIN_LED, HIGH); 
  delay(900);
}

void setup() {
  Serial.begin(19200);
  pinMode(BUILTIN_LED, OUTPUT);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Error allocating memory ");
    }
  delay(2000);
  
//Prepares display for use
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(7, 0);

  // Connect to Wi-Fi with strings ssid and password
  WiFi.begin(ssid, password);
  int displayloadingbar = 0;
  //While it ISNT-connected it will delay and print Serial until connected.
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.println("Connecting to WiFi...");
    display.clearDisplay();

    //Simple dot-based loading screen to show that something is happening
    if(displayloadingbar == 0) {
      display.println("Connecting to WiFi");
      displayloadingbar = displayloadingbar + 1;
    }
    else if(displayloadingbar == 1) {
      display.println("Connecting to WiFi.");
      displayloadingbar = displayloadingbar + 1;
    }
    else if(displayloadingbar == 2) {
      display.println("Connecting to WiFi..");
      displayloadingbar = displayloadingbar + 1;
    }
    else if(displayloadingbar == 3) {
      display.println("Connecting to WiFi...");
      displayloadingbar = 0;
    }
    display.display();
    //blink();
    display.setCursor(0, 10);
  }

  
  
  displayStatus();

  // Define web server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/", HTTP_POST, handlePost);

  server.begin();
  Serial.println("HTTP server started");
}

unsigned long lastNetworkCheck = 0;
const unsigned long networkCheckInterval = 30000;

void loop() {
  server.handleClient();

//Checks if we still have Wi-Fi every thirty seconds, and if we don't, restarts setup
//Nice looking loading screen
  int disconnectionloadingbar = 0;
  unsigned long currentMillis = millis();
  if(currentMillis - lastNetworkCheck >= networkCheckInterval) {
    if(WiFi.status() != WL_CONNECTED) {
      
      display.clearDisplay();
      display.println("Disconnected....");
      display.println("Attempting to reconnect");
      display.display();
      delay(10000);
      setup();
    }
  }
 
}
