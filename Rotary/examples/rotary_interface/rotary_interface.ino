#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

#include <EEPROM.h>
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
const int num = 4; // number of encoders
const int CLK[num] = {12, 25, 15, 14};
const int DT[num] = {13, 26, 33, 32};
const int BTTN[num] = {36, 27, 4, 21};

/* 
 * Initialize an array of Rotary class objects: rotary encoders   
 */
Rotary ro[num] = { Rotary(CLK[0], DT[0], BTTN[0]), 
                   Rotary(CLK[1], DT[1], BTTN[1]),
                   Rotary(CLK[2], DT[2], BTTN[2]),
                   Rotary(CLK[3], DT[3], BTTN[3])};

int b; // button state

/* wifi */
int scan = 0; // number of scans performed
int n = 0; // number of scanned networks
int print_count = 0; // counts how many times some selected info is printed
String ssid = String(); // stores ssid
bool ssid_selected = 0; // true if ssid is selected
String password = String(); // stores password
bool password_set = 0; // true if password set
String id = ""; // ssid displayed
String oldid = ""; // previos ssid displayed
int letter;
int oldletter;
String pswd = "";
String oldpswd = "";
int creds_received = 0;

int eep_read = 0; // EEPROM read?
int start = 0; // server started?

bool eep = false; // true if creds obtained from EEPROM

void setup() 
{ 
  Serial.begin(115200);
  EEPROM.begin(512);

//  for creds updating test
//  clear_eep();
//  update_eep(ssid, password);
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32

  display.clearDisplay();
  display.display();
  
  // text display 
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("rotary encoders 1V0");
  display.display();

  // wifi part   
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

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

  attachInterrupt(digitalPinToInterrupt(CLK[2]), rotate2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DT[2]), rotate2, CHANGE);  

  attachInterrupt(digitalPinToInterrupt(CLK[3]), rotate3, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DT[3]), rotate3, CHANGE);  
}

void loop() 
{ 
  b = checkButton();
  
  if (!start) 
  {
    if (!eep_read)
    {      
      read_eep();
      eep_read = 1;
  
      if (ssid.length() > 1)
      {
        scan = 1;
        if (password.length() > 1)
        {
          creds_received = 1;
          eep = true;
        }
      }
      else
      {
        Serial.println("ssid is empty");
      }
    }
  
    if (!scan) 
    {      
      WiFi.disconnect();
      Serial.println("scan start");    
      n = WiFi.scanNetworks(); 
      scan = 1;    
    }
    else if (!creds_received)
    {      
      if (n <= 0) 
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
            Serial.println(WiFi.SSID(i));
          }
    
          Serial.println("select SSID: ");       
          Serial.println(" "); 
    
          // TODO show this info on dispay
          
          print_count++;  
        }
    
        if (!ssid_selected)
        {
          id = WiFi.SSID(ro[0].getCounter() % n);
          
          if (oldid != id)
          {
            display.clearDisplay();    
            display.display();
            display.setCursor(0,0);
    
            display.println(String(id));
            display.display();  
            oldid = id;
          }
    
          // button logic  
          if (b == 1)
          {
            ssid = id;
            ssid_selected = 1;
            password = String();
            Serial.println("button 0 pressed");
            Serial.println("selected network: " + ssid);
          }
        }
        else if (!password_set)
        { 
          letter = ro[0].getCounter() % 97 + 32;
          
          if (oldpswd != password)
          {
            display.fillRect(0,8,128,8,BLACK);
            display.setCursor(0,8);
            display.print(password);
            oldpswd = password;
            display.display();
          }  
          if (oldletter != letter)
          { 
            display.fillRect(password.length()*6,8,6,8,BLACK); 
            display.setCursor(password.length()*6,8);        
            display.print(char(letter));
            display.display();
    
            Serial.println(char(letter));
            oldletter = letter;
          }
          
          if (b == 1)
          { 
            password = password + char(letter);  
            Serial.println(password);
          }
  
          // TODO if (b == 2) delete previous letter / backspace ascii
    
          if (b == 2)
          {
            Serial.println("password set: " + password);
            password_set = 1;  
            creds_received = 1;
            Serial.println("start: " + String(start));      
          }        
        }
      } 
    }
  }  

  if (!start && creds_received)
  {
    // login
    WiFi.begin(ssid.c_str(), password.c_str());
    display.setCursor(0,16);
    
    unsigned long timeout = 0;
    unsigned long timestart = millis();
    Serial.println("timestart: " + String(timestart)); 
    while (WiFi.status() != WL_CONNECTED && timeout < 15000) 
    {
      delay(500);          
      display.print(">");   
      display.display(); 
      timeout = millis() - timestart;  
      Serial.println("timeout: " + String(timeout));            
    }
    
    if (WiFi.status() == WL_CONNECTED && ((eep && !print_count) || (!eep && print_count)))
    {
      Serial.println("WiFi Connected.");
      display.println(char(1));
      display.display();
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      display.println(WiFi.localIP());
      display.display();

      print_count++;
    } 

    if (WiFi.status() == WL_CONNECTED && !eep) 
    {
      update_eep(ssid, password);
    }    
    if(WiFi.status() != WL_CONNECTED)
    {    
      if (eep) clear_eep();
      reset();
    }
  } 
   
  if (WiFi.status() == WL_CONNECTED && !start) 
  {
    // server part
    Server.begin();  
    Server.on("/",[](){
      Server.send_P(200, "text/html", webpage);
    }); 
  
    // open socket to transfer data
    Socket.begin();
    Socket.onEvent(webSocketEvent);
    start = 1;
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

// send encoder data in json format 
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
    if(i != num - 1) {
      json += ",";  
    }
  }  
  json += "]}";
  
  Socket.broadcastTXT(json.c_str(), json.length());
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length){}
