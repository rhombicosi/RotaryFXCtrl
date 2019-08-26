void read_eep()
{
  // read eeprom for ssid and pass
  Serial.println("Reading EEPROM ssid");
  String esid;
  String lttr;
  for (int i = 0; i < 32; ++i)
    {  
      lttr = String(char(EEPROM.read(i)));    
      if (lttr != "") 
      {
        esid += char(EEPROM.read(i));
      }
    }
  Serial.print("SSID: ");
  Serial.println(esid);
  Serial.println("Reading EEPROM pass");
  String epass; // "";
  String pass;
  for (int i = 32; i < 96; ++i)
    {
      pass = String(char(EEPROM.read(i))); 
      if (pass != "")
      {
        epass += char(EEPROM.read(i));
      }
    }
  Serial.print("PASS: ");
  Serial.println(epass); 

  ssid = esid;
  password = epass;
}

void update_eep()
{
  Serial.println("clearing eeprom");
  for (int i = 0; i < 96; ++i) { EEPROM.write(i, 0); }
  String qsid; 
  qsid = ssid;
  String qpass;
  qpass = password;
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
      EEPROM.write(32+i, qpass[i]);
      Serial.print("Wrote: ");
      Serial.println(qpass[i]); 
    }    
  EEPROM.commit();
  Serial.print("saved to eeprom...");    
}

void clear_eep()
{
  Serial.println("clearing eeprom");
  for (int i = 0; i < 96; ++i) { EEPROM.write(i, 0); }
}

void reset()
{
  display.clearDisplay();  
  display.setCursor(0,0);
  display.println("rotary encoders 1V0");
  display.display();
  
  readeep = 0;
  scan = 0;
  ssid_selected = 0;
  credsreceived = 0;
  passwordset = 0;

  print_count = 0;
 
  start = 0; // server started?

  eep = false; // true if creds obtained from EEPROM
}
