#include <WiFi.h>
#include <WebSocketClient.h>
#include <Ticker.h>

#include <Rotary.h>

const char* ssid = "SSID";
const char* password = "PSSWRD";

char path[] = "/";   //identifier of this device
char host[] = "10.0.0.4";  //replace this ip address with the ip address of your Node.Js server
const int espport = 3000;

// Use WiFiClient class to create TCP connections
WiFiClient client;
// WebSocketClient object uses client under the hood
WebSocketClient webSocketClient;
Ticker send_timer;
Ticker get_timer;

boolean handshakeFailed = 0;

String data = "";

// encoders
const int num = 4; // number of encoders
const int CLK[num] = {12, A1, 33, 32}; // A1 = 25
const int DT[num] = {13, A0, 15, 14}; // A0 = 26 
const int BTTN[num] = {36, 27, A5, 21}; // A5 = 4

Rotary ro[num] = {  Rotary(CLK[0], DT[0], BTTN[0]), 
                    Rotary(CLK[1], DT[1], BTTN[1]),
                    Rotary(CLK[2], DT[2], BTTN[2]),
                    Rotary(CLK[3], DT[3], BTTN[3])  };

void setup()
{
    Serial.begin(115200);
    delay(10);
    
    // connecting to a WiFi network
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP()); //
    delay(1000);

    // connect to websocket server
    wsconnect();
    
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
  
    attachInterrupt(digitalPinToInterrupt(CLK[2]), rotate2, CHANGE);
    attachInterrupt(digitalPinToInterrupt(DT[2]), rotate2, CHANGE);  
  
    attachInterrupt(digitalPinToInterrupt(CLK[3]), rotate3, CHANGE);
    attachInterrupt(digitalPinToInterrupt(DT[3]), rotate3, CHANGE);  

    delay(1000);
    if (client.connected()) 
    {
       Serial.println("setup data transmission timers");       
       send_timer.attach(0.15, sendEncoderDataJSON);
       // receiving data is optional and delays data send
       get_timer.attach(0.4, getEncoderDataJSON);
    }
}

void loop()
{
//    alternative way of data transmission
//    without timers
//    works more reliable 
//    if (client.connected()) 
//    {
//        getEncoderDataJSON();
//        delay(10);
//        sendEncoderDataJSON();
//        delay(10);
//    }
}

//***************function definitions*******************************************************************
void wsconnect()
{
    // Connect to the websocket server
    if (client.connect(host, espport)) 
    {
        Serial.println("Connected");
    } 
    else 
    {
        Serial.println("Connection failed.");
        delay(1000);  
       
        if(handshakeFailed)
        {
            handshakeFailed=0;
            ESP.restart();
        }
        handshakeFailed=1;
    }
    
    // Handshake with the server
    webSocketClient.path = path;
    webSocketClient.host = host;
    
    if (webSocketClient.handshake(client)) {
        Serial.println(!client.connected());
        Serial.println("Handshake successful");
        
    } else {      
        Serial.println("Handshake failed.");
        delay(4000);  
        
        if(handshakeFailed) {
            handshakeFailed = 0;
            ESP.restart();
        }
        handshakeFailed = 1;
    }
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

void rotate2() 
{
    ro[2].process();
    ro[2].rotate();
}

void rotate3()
{
    ro[3].process();
    ro[3].rotate();  
}

void getEncoderDataJSON()
{
    webSocketClient.getData(data);
    if (data.length() > 0) 
    {     
        Serial.println(data);
    }
}

void sendEncoderDataJSON()
{
    String json = "{\"encoders\": [";  
    for (int i = 0; i < num; i++){
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
      json += "}";
      if(i != num - 1) {
        json += ",";  
      }
    }  
    json += "]}";
    webSocketClient.sendData(json);
}
