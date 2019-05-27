#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <Ticker.h>

#include <Rotary.h>

/* server part */
Ticker timer;

char * ssid = "HEXECONTAHEDRON";
char * password = "salamander";

WebServer server;
WebSocketsServer Socket = WebSocketsServer(81);

/* HTML page is loaded from a separate header file 
 * that file is stored within the project 
 * and should be included in the following way
 */
char webpage[] PROGMEM =
#include "webpage.h"
;

/* encoder part */
const int CLK[2] = {14, 32};
const int DT[2] = {15, 33};

Rotary ro[2] = { Rotary(CLK[0], DT[0]), Rotary(CLK[1], DT[1])};

void setup() { 

  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
          Serial.print(".");
          delay(500);
  }
  
  Serial.println("");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/",[](){
    server.send_P(200, "text/html", webpage);
  });  
  server.begin();
  
  Socket.begin();
  Socket.onEvent(webSocketEvent);   

  timer.attach(0.2, getData);
  
  for(int i = 0; i < 2; i++) {
    pinMode(CLK[i], INPUT_PULLUP);
    pinMode(DT[i], INPUT_PULLUP);
  }
  
  attachInterrupt(digitalPinToInterrupt(CLK[0]), rotate0, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DT[0]), rotate0, CHANGE);

  attachInterrupt(digitalPinToInterrupt(CLK[1]), rotate1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DT[1]), rotate1, CHANGE);
}

void loop() {
  server.handleClient();
  Socket.loop();
}

void rotate0() 
{
  ro[0].process();
  ro[0].rotate();
}

void rotate1()
{
  ro[1].process();
  ro[1].rotate();
}

void getData(){
  String json = "{\"encoder\": [";
  
  for (int i = 0; i < 2; i++){
    json += CLK[i];
    if(i != 1) {
      json += ",";  
    }
  }
  
  json += "]}";
  Socket.broadcastTXT(json.c_str(), json.length());
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length){
}
