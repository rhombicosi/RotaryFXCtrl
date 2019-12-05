#include <WiFi.h>
#include <WiFiMulti.h>
#include <SocketIoClient.h>
#include <Ticker.h>
#include <Rotary.h>

#define USE_SERIAL Serial

const char* ssid = "ssid"; 
const char* password = "password";

const char* user = "\"username\"";

const char* host = "10.0.0.01";  //replace this ip address with the ip address of your Node.Js server
const uint16_t espport = 5000;

WiFiMulti WiFiMulti;
SocketIoClient webSocket;

void event(const char* payload, size_t length) {
  Serial.printf("got message: %s\n", payload);
}

Ticker send_timer;
String data = "";

// encoders
const int num = 2; // number of encoders
const int CLK[num] = {25, 15}; // encoders clock pins A1 = 25
const int DT[num] = {26, 13}; // encoders data pins A0 = 26
const int BTTN[num] = {36, 14}; // button pins A4 = 36

Rotary ro[num] = {  Rotary(CLK[0], DT[0], BTTN[0]), 
                    Rotary(CLK[1], DT[1], BTTN[1]) };

// prototyping
void rotate0();
void rotate1();
void sendEncoderDataJSON();

void setup()
{
    USE_SERIAL.begin(115200);
    // USE_SERIAL.setDebugOutput(true);
    
    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();
    
    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }
  
    WiFiMulti.addAP("HEXECONTAHEDRON", "salamander");
    
    while(WiFiMulti.run() != WL_CONNECTED) {
      delay(100);
    }    

    // connect to websocket server
    webSocket.begin("10.0.0.4", 5000);  

    webSocket.emit("new esp32", user);
    
    // setup software pullups for encoders
    for(int i = 0; i < num; i++) {
        pinMode(CLK[i], INPUT_PULLUP);
        pinMode(DT[i], INPUT_PULLUP);
        pinMode(BTTN[i], INPUT_PULLUP);
    }
    
    attachInterrupt(digitalPinToInterrupt(CLK[0]), rotate0, CHANGE);
    attachInterrupt(digitalPinToInterrupt(DT[0]), rotate0, CHANGE);
    
    attachInterrupt(digitalPinToInterrupt(CLK[1]), rotate1, CHANGE);
    attachInterrupt(digitalPinToInterrupt(DT[1]), rotate1, CHANGE);  

    send_timer.attach(0.1, sendEncoderDataJSON);
}

void loop()
{
    webSocket.loop();    
}

// function definitions
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

void sendEncoderDataJSON()
{
    String json = "{\"encoders\": [";  
    for (int i = 0; i < num; i++) {
        json += "{\"pins\": [";
        json += CLK[i];
        json += ",";
        json += DT[i];
        json += "],";
        json += "\"counter\": ";
        json += ro[i].getCounter();
        json += ",";
        json += "\"angle\": ";
        json += ro[i].getAngle();
        json += ",";
        json += "\"period\": ";
        json += ro[i].getPeriod();
        json += ",";
        json += "\"result\": ";
        json += ro[i].getResult();
        json += "}";
        if(i != num - 1) {
            json += ",";  
        }
    }  
    json += "]}";
    webSocket.emit("esp encoders", json.c_str());
}
