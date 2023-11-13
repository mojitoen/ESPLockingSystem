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
  display.setCursor(0, 10);
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
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);



  // Connect to Wi-Fi with strings ssid and password
  WiFi.begin(ssid, password);
  int displayloadingbar = 0;
  //While it ISNT-connected it will delay and print Serial until connected. Easier way could be to use ext LCD to establish connection. 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
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
    blink();
    display.setCursor(0, 10);
  }

  
  
  displayStatus();

  // Define web server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/", HTTP_POST, handlePost);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
