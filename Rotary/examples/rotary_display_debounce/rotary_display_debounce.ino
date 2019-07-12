#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <Ticker.h>
#include <Rotary.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

/* server part */
Ticker timer;
WebServer Server;
WebSocketsServer Socket = WebSocketsServer(81);

/* 
 * header file containing HTML page 
 * should be included in the following way
 */
char webpage[] PROGMEM =
#include "webpage.h"
;

/* initialise encoders */
const int num = 2; // number of encoders
const int CLK[num] = {14, 15};
const int DT[num] = {32, 33};
const int BTTN[num] = {25, 21};
Rotary ro[num] = { Rotary(CLK[0], DT[0], BTTN[0]), Rotary(CLK[1], DT[1], BTTN[1])};
int b;

/* wifi */
int scan = 0; // number of scans performed
int n = 0; // number of scanned networks
int print_count = 0; // counts how many times some selected info is printed
String SSID = ""; // SSID
bool SSID_selected = 0; 

String psswd = String();
bool PSWD_set = 0;

String id = ""; 
String oldid = "";

int letter;
int oldletter;

void setup() 
{ 
  Serial.begin(115200);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  
  // wifi part   
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();  

  // server part
  Server.begin();  
  Server.on("/",[](){
    Server.send_P(200, "text/html", webpage);
  }); 

  // open socket to transfer data
  Socket.begin();
  Socket.onEvent(webSocketEvent);
  timer.attach(0.01, getEncoderData);

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
  // attachInterrupt(digitalPinToInterrupt(BTTN[1]), press1, FALLING); 

  // Clear the buffer.
  display.clearDisplay();
  display.display();
  
  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("rotary encoders");
  display.display();
}

void loop() 
{  
  b = checkButton();
  
  unsigned long t0 = millis();
  unsigned long dif = millis() - t0;

  if (!scan)
  {
    Serial.println("scan start");
    n = WiFi.scanNetworks(); 
    scan = 1;
    dif = millis() - t0;
    Serial.println("scan time: " +  String(dif));
  }
  
  if (n == 0) 
  {
    Serial.println("no networks found");
    scan = 0;
  }
  else 
  { 
    if (print_count == 0) 
    {      
      Serial.println(String(n) + " networks found");
      
      for (int i = 0; i < n; ++i) 
      {         
        Serial.print(i + 1);
        Serial.print(": "); 
        Serial.println( WiFi.SSID(i));
      }

      Serial.println("select SSID: ");       
      Serial.println(" "); 
      
      print_count++;  
    }

    if (!SSID_selected)
    {
      id = WiFi.SSID(ro[0].getCounter() % n);  
      // Serial.println("select network " + String(ro[0].getCounter() % n) + " : " + id);
      
      if (oldid != id)
      {
        display.clearDisplay();    
        display.display();
        display.setCursor(0,0);
        display.println(String(id));
        display.display();
  
        oldid = id;
      }

      //button logic  
      if (b == 1)
      {
        SSID = id;
        SSID_selected = 1;
        Serial.println("button 0 pressed");
        Serial.println("selected network: " + SSID);
      }
      
      //delay(10);
    }
    else
    {      
      // Serial.println("button state " + String(digitalRead(BTTN[0])));
      letter = ro[0].getCounter() % 97 + 32;

      if (oldletter != letter && !PSWD_set)
      {
        display.fillRect(0,8,128,8,BLACK);
        display.display();
        display.setCursor(0,8);
        display.print(psswd);
        display.print(char(letter));
        display.display();

        Serial.println(char(letter));
        oldletter=letter;
      }
      
      if (b == 1 && !PSWD_set)
      {        
        psswd = psswd + char(letter);  
        Serial.println(psswd);
      }

      if (b == 3)
      {
        Serial.println("password set: " + psswd);
        PSWD_set = 1;        
      }
      // Serial.println("");
    }    

    if(PSWD_set)
    {
      // login
      WiFi.begin(SSID.c_str(), psswd.c_str());
      display.setCursor(0,16);
      while (WiFi.status() != WL_CONNECTED) 
      {
        delay(500);
        
        display.print(".");   
        display.display();                 
      }

      if (print_count == 1)
      {
        Serial.println("WiFi Connected.");
        display.println("WiFi Connected.");
        display.display();
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        display.println(WiFi.localIP());
        display.display();

        print_count++;
      }
     } 
     delay(10);
  }  
  
  Server.handleClient();
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

// serve encoder data in json format 
void getEncoderData(){
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
    if(i != 1) {
      json += ",";  
    }
  }  
  json += "]}";
  
  Socket.broadcastTXT(json.c_str(), json.length());
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length){};

/* 4-Way Button:  Click, Double-Click, Press+Hold, and Press+Long-Hold Test Sketch

By Jeff Saltzman
Oct. 13, 2009

To keep a physical interface as simple as possible, this sketch demonstrates generating four output events from a single push-button.
1) Click:  rapid press and release
2) Double-Click:  two clicks in quick succession
3) Press and Hold:  holding the button down
4) Long Press and Hold:  holding the button for a long time 
*/

//=================================================
//  MULTI-CLICK:  One Button, Multiple Events

// Button timing variables
int debounce = 20;          // ms debounce period to prevent flickering when pressing or releasing the button
int DCgap = 250;            // max ms between clicks for a double click event
int holdTime = 1000;        // ms hold period: how long to wait for press+hold event
int longHoldTime = 3000;    // ms long hold period: how long to wait for press+hold event

// Button variables
boolean buttonVal = HIGH;   // value read from button
boolean buttonLast = HIGH;  // buffered value of the button's previous state
boolean DCwaiting = false;  // whether we're waiting for a double click (down)
boolean DConUp = false;     // whether to register a double click on next release, or whether to wait and click
boolean singleOK = true;    // whether it's OK to do a single click
long downTime = -1;         // time the button was pressed down
long upTime = -1;           // time the button was released
boolean ignoreUp = false;   // whether to ignore the button release because the click+hold was triggered
boolean waitForUp = false;        // when held, whether to wait for the up event
boolean holdEventPast = false;    // whether or not the hold event happened already
boolean longHoldEventPast = false;// whether or not the long hold event happened already

int checkButton() {  
   int event = 0;  
   buttonVal = digitalRead(BTTN[0]);
   // Serial.println("button pressed " + String(buttonVal));
   // Button pressed down
   if (buttonVal == LOW && buttonLast == HIGH && (millis() - upTime) > debounce)
   {
       downTime = millis();
       ignoreUp = false;
       waitForUp = false;
       singleOK = true;
       holdEventPast = false;
       longHoldEventPast = false;
       if ((millis()-upTime) < DCgap && DConUp == false && DCwaiting == true)  DConUp = true;
       else  DConUp = false;
       DCwaiting = false;
   }
   // Button released
   else if (buttonVal == HIGH && buttonLast == LOW && (millis() - downTime) > debounce)
   {        
       if (not ignoreUp)
       {
           upTime = millis();
           if (DConUp == false) DCwaiting = true;
           else
           {
               event = 2;
               DConUp = false;
               DCwaiting = false;
               singleOK = false;
           }
       }
   }
   // Test for normal click event: DCgap expired
   if ( buttonVal == HIGH && (millis()-upTime) >= DCgap && DCwaiting == true && DConUp == false && singleOK == true && event != 2)
   {
       event = 1;
       DCwaiting = false;
   }
   // Test for hold
   if (buttonVal == LOW && (millis() - downTime) >= holdTime) {
       // Trigger "normal" hold
       if (not holdEventPast)
       {
           event = 3;
           waitForUp = true;
           ignoreUp = true;
           DConUp = false;
           DCwaiting = false;
           //downTime = millis();
           holdEventPast = true;
       }
       // Trigger "long" hold
       if ((millis() - downTime) >= longHoldTime)
       {
           if (not longHoldEventPast)
           {
               event = 4;
               longHoldEventPast = true;
           }
       }
   }
   buttonLast = buttonVal;
   return event;
}
