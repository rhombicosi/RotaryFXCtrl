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

//char * ssid = "******";
//char * password = "******";

WebServer Server;
WebSocketsServer Socket = WebSocketsServer(81);

/* 
 * header file containing HTML page 
 * should be included in the following way
 */
char webpage[] PROGMEM =
#include "webpage.h"
;

const int num = 2;
/* encoder part */
const int CLK[num] = {14, 15};
const int DT[num] = {32, 33};
const int BTTN[num] = {25, 21};

Rotary ro[num] = { Rotary(CLK[0], DT[0], BTTN[0]), Rotary(CLK[1], DT[1], BTTN[1])};

// button 
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 250;    // the debounce time; increase if the output flickers
unsigned long lastHoldTime = 0;
unsigned long holdDelay = 3000;

int n = 0; // number of scanned networks
int scan = 0;
int print_count = 0;
String net = "";
bool SSID_selected = 0; 

String psswd = String();
bool PSWD_set = 0;

String id = ""; 
String oldid = "";

int letter;
int oldletter;

volatile int state[num] = { HIGH, HIGH };
volatile int state_short[num] = { HIGH, HIGH };
volatile int state_long[num] = { HIGH, HIGH };

volatile unsigned long current_high[num];
volatile unsigned long current_low[num];

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
//  attachInterrupt(digitalPinToInterrupt(BTTN[0]), press0, FALLING);
//  attachInterrupt(digitalPinToInterrupt(BTTN[0]), hold0, RISING);

  attachInterrupt(digitalPinToInterrupt(BTTN[0]), read_button, CHANGE);

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
  unsigned long  t0 = millis();
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
      
      print_count = 1;  
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
      if (check0())
      {
        net = id;
        SSID_selected = 1;
        Serial.println("button 0 pressed");
        Serial.println("selected network: " + net);
      }
      
      //delay(10);
    }
    else
    {      
      // Serial.println("button state " + String(digitalRead(BTTN[0])));
      letter = ro[0].getCounter() % 97 + 33;

      if (oldletter != letter)
      {
        display.fillRect(0,8,5,8,BLACK);
        display.display();
        display.setCursor(0,8);
        display.print(char(letter));
        display.display();

        Serial.println(char(letter));
        oldletter=letter;
      }
      
//      if(check0() && !PSWD_set)
      if (state[0] == LOW && !PSWD_set)
      {        
        psswd = psswd + char(letter);   
        
        lastHoldTime = 0;     
        state[0] = HIGH;
        Serial.println(psswd);
      }

//      if(hold0())
      if (state_long[0] == LOW)
      {
        Serial.println("password set: " + psswd);
        PSWD_set = 1;
        
      }
      // Serial.println("");
    }    

    if(PSWD_set)
    {
      // login
      WiFi.begin(net.c_str(), psswd.c_str());
      display.setCursor(0,16);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        
        display.print(".");   
        display.display();           
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

// button routine
// update last time button was pressed
void press0() 
{
  lastDebounceTime = millis();
}

// return true if key pressed
bool check0()
{
  if (lastDebounceTime!=0) 
  {
    if (millis()-lastDebounceTime > debounceDelay && millis()-lastDebounceTime < holdDelay)
    {
      lastDebounceTime = 0;
      return true;
    }
  } 
  
  lastHoldTime = lastDebounceTime;
  return false;
}

bool hold0()
{
  if (lastHoldTime!=0)
  {
    if (millis() - lastHoldTime > holdDelay && digitalRead(BTTN[0]) == LOW)
    {
      Serial.println("holdtime " + String(millis() - lastHoldTime));
      lastHoldTime = 0;
      return true;
    }
  }
  return false;
}

void read_button()
{
  //cycles through the buttons to find out which one was pressed or released
  for(int i=0; i<num; i++)
  {
    //if this is true the button was just pressed down
    if(digitalRead(BTTN[i]) == LOW)
    {
      //note the time the button was pressed
      current_high[i] = millis();
      state[i] = LOW;
    }
    //if no button is high one had to be released. The millis function will increase while a button is hold down the loop function will be cycled (no change, so no interrupt is active) 
     if(digitalRead(BTTN[i] == HIGH) && state[i] == LOW)
    {
      current_low[i] = millis();
      if((current_low[i] - current_high[i]) > 50 && (current_low[i] - current_high[i]) < 800)
      {
        state_short[i] = !state_short[i];
        state[i] = HIGH;
      }
      else if((current_low[i] - current_high[i]) >= 800 && (current_low[i] - current_high[i]) < 4000)
      {
        state_long[i] = !state_long[i];
        state[i] = HIGH;
      }
    }
  }
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

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length){}
