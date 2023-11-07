#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

//Placeholder Wifi information. Change before you upload to hardware!!
const char* ssid = "Test";
const char* password = "123";

//Init server
ESP8266WebServer server(80);

//Could consider adding static ip-addressing here? Remember to change per-device in that case

//handleRoot happens whenever the server gets a GET-Request
void handleRoot() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "Hello World");
  blink();
}
//HandlePost happens whenever the servers gets a POST-request
void handlePost() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  String message = "Hello World (POST)";
  server.send(200, "text/plain", message);
  blink();
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
  


  // Connect to Wi-Fi with strings ssid and password
  WiFi.begin(ssid, password);
  //While it ISNT-connected it will delay and print Serial until connected. Easier way could be to use ext LCD to establish connection. 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    blink();
  }

  //On Connect: 
  Serial.println("Connected to WiFi. IP-address: ");
  Serial.println(WiFi.localIP()); 


  // Define web server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/", HTTP_POST, handlePost);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}