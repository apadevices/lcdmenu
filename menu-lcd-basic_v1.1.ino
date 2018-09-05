// Needed LIBs ...
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>
#include <EEPROM.h>
#include <OneWire.h>
#include <Wire.h>
#include <DS3231.h>

// Declare and set LCD object

  DS3231 clock;
    
    // Init a Time-data structure
    RTCDateTime dt;
    
    // Set sketch compiling time
    //clock.setDateTime(__DATE__, __TIME__);

// set the LCD address to 0x27 for a 16 chars 2 line display
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

byte selected[8] = {
  B01000,
  B01100,
  B01110,
  B01111,
  B01110,
  B01100,
  B01000,
};
byte up[8] = {
  B00100,
  B01110,
  B11111,
  B00100,
  B00100,
  B00100,
  B00100,
};
byte down[8] = {
  B00100,
  B00100,
  B00100,
  B00100,
  B11111,
  B01110,
  B00100,
};
byte updown[8] = {
  B00100,
  B01110,
  B11111,
  B00100,
  B11111,
  B01110,
  B00100,
};
byte degree[8] = {
  B00111,
  B00101,
  B00111,
  B00000,
  B00000,
  B00000,
  B00000,
};

byte roof1[8] = {
  B00100,
  B01010,
  B10001,
  B10001,
  B10001,
  B10001,
  B00000,
};

byte roof2[8] = {
  B00100,
  B01110,
  B11111,
  B11111,
  B11111,
  B11111,
  B00000,
};
// Declare KNOBS (rotary encoders) objects
  //   Best Performance: both pins have interrupt capability
  //   Good Performance: only the first pin has interrupt capability
  //   Low Performance:  neither pin has interrupt capability
  //   Avoid using pins with LEDs attached
  //   PinMode for (DT, CLK) are set to INPUT automaticly

  Encoder knob1(2, 4);  // left
  Encoder knob2(3, 5);  // right

  static byte pin_KB1 = 6;        //KNOB1 - Pump runnunig time/day (short) and (long) pool watter reservoar capacity (m3).
  static byte pin_KB2 = 7;        //KNOB2 - PH dosing set-point (short) and (long) PH PROBE calibration process control.


// GLOBAL VARIABLES

byte KB1_btn = 0;
int KB2_btn = 0;
unsigned long KB1_debounce = 0;
unsigned long KB2_debounce = 0;

unsigned long MENU_timeout = 30000;   //

int newK1, newK2;
int position_K1, position_K2, position_K1_last, position_K2_last;

//LCD CLEARED
boolean cleared;

//POOL CONTROL VARIABLES:
  //L2_1
  byte pool_vol = 0;
  byte filt_hours = 0;
  //L2_2
  float ph_setpoint = 0.0;
  byte ph_solaction = 0;     //activ solution mediator
  int rx_setpoint = 0.0;
  byte rx_solaction = 0;
  //L2_3
  float solar_temp = 0.0;
    //int solar_help = 0.0;
  float pool_max_t = 0.0;
    //PH pump (plus or minus)
  char* ph_medium[] = {"MINUS", "PLUS "};
  boolean ph_plus_medium;   //false means MINUS like ph_medium[0]
    // calibration control
    bool c_ph4_start = false;
    bool c_ph7_start = false;
    bool c_rx475_start = false;
    bool c_rx645_start = false;
  
  // CLOCK & Timings
  boolean set_date = false;     //to start adjasting
  boolean set_time = false;

  byte currenthour;             //time/date variables
  byte currentminute;
  byte currentseconde;
  byte currentday;
  byte currentmonth;
  int currentyear;
        
// ** SUBMENU variables ***************************************************
// L2 MENU texts, items and units
//
byte r_menuItem1, l_menuItem1;
byte r_menuItem2, l_menuItem2;
byte r_menuUnit1, l_menuUnit1;
byte r_menuUnit2, l_menuUnit2;

  //RIGHT SIDE page text items, here you can translate them all
char* r_menuItems[]={"Pool-volume: ", "Filter-dura: ",  //page +1
                     "pH-setpoint: ", "RX-setpoint: ",  //page +2
                     "Absorb-temp: ", "Pool-maxtmp: ",  //page +3
                     "Set-date   : ", "Set-time   : "   //page +4
                     };

  char* r_menuUnits[] = {"m3", "mV", "C", "hd", "  "};

  //LEFT SIDE page test items, here you can translate them all
char* l_menuItems[]={"pH-calibrtn: ", "pH-solution: ", //page -1
                     "RX-calibrtn: ", "RX-solution: ", //page -2
                     "pH-dozing+-: ", "Pool-volume: "  //page -3
                     };
                     
char* l_menuUnits[] = {"m3", "ml", "  "};

// ** SUBMENU variables ***************************************************


//This function will write a 4 byte (32bit) long to the eeprom at
//the specified address to address + 3.
void EEPROMWritelong(int address, long value)
      {
      //Decomposition from a long to 4 bytes by using bitshift.
      //One = Most significant -> Four = Least significant byte
      byte four = (value & 0xFF);
      byte three = ((value >> 8) & 0xFF);
      byte two = ((value >> 16) & 0xFF);
      byte one = ((value >> 24) & 0xFF);

      //Write the 4 bytes into the eeprom memory.
      EEPROM.write(address, four);
      EEPROM.write(address + 1, three);
      EEPROM.write(address + 2, two);
      EEPROM.write(address + 3, one);
      }

//This function will return a 4 byte (32bit) long from the eeprom
//at the specified address to address + 3.
long EEPROMReadlong(long address)
      {
      //Read the 4 bytes from the eeprom memory.
      long four = EEPROM.read(address);
      long three = EEPROM.read(address + 1);
      long two = EEPROM.read(address + 2);
      long one = EEPROM.read(address + 3);

      //Return the recomposed long by using bitshift.
      return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
      }
//

void setup() {
  // put your setup code here, to run once:

// Serial debuger, start
Serial.begin(9600);

  // Initialize the RTC object:
  clock.begin();
   
    // Set sketch compiling time
    //clock.setDateTime(__DATE__, __TIME__);

pinMode(pin_KB1, INPUT);
pinMode(pin_KB2, INPUT);

// Lets set col.& rows of your LCD type[1602 or 2004]and initialize.
 lcd.begin(20,4);
 lcd.backlight();

 lcd.createChar(0, selected);
 lcd.createChar(1, up);
 lcd.createChar(2, down);
 lcd.createChar(3, updown);
 lcd.createChar(4, degree);
 lcd.createChar(5, roof1);  //for clock settings, selection
 lcd.createChar(6, roof2);  //for clock settings, selected

 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print("no data");

pool_vol = EEPROM.read(1);
filt_hours = EEPROM.read(5);
ph_setpoint = (float)(EEPROM.read(10) / 10.0);
rx_setpoint = EEPROMReadlong(15);
solar_temp = (float)EEPROMReadlong(20) / 10;
pool_max_t = (float)EEPROMReadlong(25) / 10;
ph_solaction = EEPROM.read(30);
rx_solaction = EEPROM.read(35);
ph_plus_medium = EEPROM.read(40);

} //setup

/*
buttonK1 = digitalRead(pin_KB1);
buttonK2 = digitalRead(pin_KB2);

newK1 = knob1.read() / 4;
newK2 = knob2.read() / 4;
  knob1.write(0);
*/

void loop() {
  // put your main code here, to run repeatedly:

//What time is now? RTC reading...
  dt = clock.getDateTime();
        currenthour = (dt.hour);
        currentminute = (dt.minute);
        currentseconde = (dt.second);
        currentday = (dt.day);
        currentmonth = (dt.month);
        currentyear = (dt.year);
  
//buttons checking
 newK1 = knob1.read() / 2.0; //left
 newK2 = knob2.read() / 4.0; //right

// LCD USER MENU:
  position_K1 = (int)newK1;
  position_K2 = (int)newK2;

  //KB1_btn = digitalRead(pin_KB1);
  //  delay(10); //debounce
  //KB2_btn = digitalRead(pin_KB2);
  //delay(10); //debounce

  //LEVEL1
  lcdrefresh();

    if (position_K1 == 0) { //if1
 
      lcdrefresh();  
      MENU_L1();
  
  } // if1 


  //LEVEL 2.1, RIGHT - Volume&Filtration 
  //**********************************************************************************************************
  //RIGHT SUBMENU, LEVEL 2.1
  //**********************************************************************************************************
  
    if (position_K1 == 1) {           //if2

      r_menuItem1 = position_K1 - 1;    //pool-volume
      r_menuItem2 = position_K1;        //filtration
      
      //----------------------------
      r_menuUnit1 = 0;    //m3
      r_menuUnit2 = 3;    // h/d
        
      lcdrefresh();  //lcd.cleaning
     
      MENU_L2(2);  //lcd.printing. Passing 2 to write narrow(down) in the center of lcd.
       selector();  //pointer-switch

      //** variables casting for below while functions ...
      byte pool_vol_local = pool_vol;       //variable for line1
      byte filt_hours_local = filt_hours;   //variable for line2
      
      //1. LINE settings - Volume (m3)
      //**********************************************************************************************************
      if (((digitalRead(pin_KB2) != 1 && KB2_btn == 0 && (millis() - KB2_debounce) >= 500)) && position_K2 == 0) {
        
        KB2_btn = 1;
        KB2_debounce = millis();
        
        lcd.setCursor(0,0);
        lcd.write(byte(0));
        
        Serial.println(r_menuItems[r_menuItem1]);
        }
        else {
          KB2_btn = 0;
          }      
          while (KB2_btn == 1) {           
            
            int newvalue = (knob2.read() / 4);
            int lastvalue;
            byte setpoint;
            
            if (newvalue != lastvalue) {

             if (setpoint <= 1) {;
                //knob2.write( (lastvalue * 4) + 2);
                knob2.write((lastvalue * 4) + 4);
                }
                else if (setpoint >= 100) {;
                  //knob2.write( (lastvalue * 4) - 2);
                  knob2.write((lastvalue * 4) - 4);
                }

              // knob2 reading ...  
              setpoint = pool_vol_local; 
              setpoint += newvalue;
              setpoint = constrain(setpoint,1,99);

              pool_vol = setpoint;
              
              lastvalue = newvalue;
              } 

                //LCD value print inside while
                if (pool_vol < 10) {              //shift from XX number to " "X
                  lcd.setCursor(14,0);
                  lcd.print(" ");
                  lcd.setCursor(15,0);
                  lcd.print(pool_vol);                  
                  }
                  else {
                    lcd.setCursor(14,0);
                    lcd.print(pool_vol);
                    }
            
            if (digitalRead(pin_KB2) != 1 && KB2_btn == 1 && (millis() - KB2_debounce) >= 500 ) {
              
              KB2_debounce = millis();
              knob2.write(12);  //12 - direct to save
              
              //end this while-loop
              KB2_btn = 0;
              }
          
          } //while, line 1

      //2. LINE settings - Filtration, duration (hours)
      //**********************************************************************************************************
      if (((digitalRead(pin_KB2) != 1 && KB2_btn == 0 && (millis() - KB2_debounce) >= 500)) && position_K2 == 1) {
        
        KB2_btn = 1;
        KB2_debounce = millis();
        
        lcd.setCursor(0,1);
        lcd.write(byte(0));
        
        Serial.println(r_menuItems[r_menuItem2]);
        }
        else {
          KB2_btn = 0;
          }
          while (KB2_btn == 1) {

            int newvalue = (knob2.read() / 4); 
            int lastvalue;
            byte setpoint;
            
            if (newvalue != lastvalue) {

             lastvalue = newvalue;

             if (setpoint <= 1) {;            //border value in contrain below
                knob2.write( (lastvalue * 4) + 2);
                }
                else if (setpoint >= 24) {;    //border value in contrain below
                  knob2.write( (lastvalue * 4) - 2);
                }
               Serial.print(lastvalue); 
              setpoint = filt_hours_local;
              setpoint += newvalue;
              setpoint = constrain(setpoint, 1, 24);

              filt_hours = setpoint;
              }

                //LCD value print inside while
                if (filt_hours < 10) {              //shift from XX number to " "X
                  lcd.setCursor(14,1);
                  lcd.print(" ");
                  lcd.setCursor(15,1);
                  lcd.print(filt_hours);                  
                  }
                  else {
                    lcd.setCursor(14,1);
                    lcd.print(filt_hours);
                  }
            
            if (digitalRead(pin_KB2) != 1 && KB2_btn == 1 && (millis() - KB2_debounce) >= 500 ) {
              
              KB2_debounce = millis();
              knob2.write(12);  //12 - direct to save
              KB2_btn = 0;
              }
          } //while

       //**********************************************************************************************************
       //4. LINE, BACK
       if (((digitalRead(pin_KB2) != 1 && KB2_btn == 0 && (millis() - KB2_debounce) >= 500)) && position_K2 == 2) {

        pool_vol = EEPROM.read(1);
        filt_hours = EEPROM.read(5);
  
        menu2_back();
        }

       //**********************************************************************************************************
       //%. LINE, SAVE
       if (((digitalRead(pin_KB2) != 1 && KB2_btn == 0 && (millis() - KB2_debounce) >= 500)) && position_K2 == 3) {
          
          KB2_debounce = millis();
          KB2_btn = 0;
          
            lcd.setCursor(14,3);
            lcd.write(byte(0));
            delay(500);

            if (EEPROM.read(1) != pool_vol) {
              EEPROM.write(1, pool_vol);
                lcd.clear();
                lcd.setCursor(3,2);
                lcd.print("<VOLUME, SAVED>");
                delay(1000);
                lcd.clear();
            } 
            
            if (EEPROM.read(5) != filt_hours) {
              EEPROM.write(5, filt_hours);
                lcd.clear();
                lcd.setCursor(3,2);
                lcd.print("<HOURS, SAVED>");
                delay(1000);
                lcd.clear();
            } 
            
            //knob1.write(0);
            //position_K1 = 0;
            knob2.write(8);   //8 - direct to BACK
            
            lcdrefresh();
       }
  } //if2, end of LEVEL2.1
  

  //LEVEL 2.2, RIGHT - RX&PH Setpoints
  //**********************************************************************************************************
  //RIGHT SUBMENU, LEVEL 2.2
  //**********************************************************************************************************
  
    if (position_K1 == 2) {           //if3

      r_menuItem1 = position_K1 + 0;   //ph-setpoint
      r_menuItem2 = position_K1 + 1;   //rx-setpoint
      
      //----------------------------
      r_menuUnit1 = 4;    //nothing
      r_menuUnit2 = 1;    //mV
        
      lcdrefresh();  //lcd.cleaning
     
      MENU_L2(3);  //lcd.printing. Passing 3 to write narrow(up&down) in the center of lcd.
       selector();  //pointer-switch

      //** variables casting for below while functions ...
      byte ph_setpoint_casted  = (byte)ph_setpoint * 10.0;   //variable for line1
      int rx_setpoint_local = rx_setpoint;                  //variable for line2
      
      //1. LINE settings - pH Setpoint
      //**********************************************************************************************************
      if (((digitalRead(pin_KB2) != 1 && KB2_btn == 0 && (millis() - KB2_debounce) >= 500)) && position_K2 == 0) {
        
        KB2_btn = 1;
        KB2_debounce = millis();
        
        lcd.setCursor(0,0);
        lcd.write(byte(0));
        
        Serial.println(r_menuItems[r_menuItem1]);
        }
        else {
          KB2_btn = 0;
          }
          while (KB2_btn == 1) {
            
            int newvalue = (knob2.read() / 4);
            int lastvalue;
            byte setpoint;
            
            if (newvalue != lastvalue) {

              lastvalue = newvalue;
              
             if (setpoint <= 60) {;
                knob2.write( (lastvalue * 4) + 2);
                }
                else if (setpoint >= 80) {;
                  knob2.write( (lastvalue * 4) - 2);
                }

              // knob2 reading ...  
              setpoint = ph_setpoint_casted; //variable in the top
              setpoint += newvalue;
              setpoint = constrain(setpoint,60,80);

              ph_setpoint = (float)setpoint / 10.0;
              }        
                lcd.setCursor(14,0);
                lcd.print(ph_setpoint, 1);
            
            if (digitalRead(pin_KB2) != 1 && KB2_btn == 1 && (millis() - KB2_debounce) >= 500 ) {
              
              KB2_debounce = millis();
              knob2.write(12);  //12 - direct to save
              KB2_btn = 0;
              }
          
          } //while, line 1

      //2. LINE settings - RX Setpoint (mV)
      //**********************************************************************************************************
      if (((digitalRead(pin_KB2) != 1 && KB2_btn == 0 && (millis() - KB2_debounce) >= 500)) && position_K2 == 1) {
        
        KB2_btn = 1;
        KB2_debounce = millis();
        
        lcd.setCursor(0,1);
        lcd.write(byte(0));
        
        Serial.println(r_menuItems[r_menuItem2]);
        }
        else {
          KB2_btn = 0;
          }
          while (KB2_btn == 1) {

            int newvalue = (knob2.read() / 4);
            int lastvalue;
            int setpoint;
            
            if (newvalue != lastvalue) {

              lastvalue = newvalue;
              
             if (setpoint <= 600) {;            //border value in contrain below
                knob2.write( (lastvalue * 4) + 3);
                }
                else if (setpoint >= 999) {;    //border value in contrain below
                  knob2.write( (lastvalue * 4) - 3);
                }

              setpoint = (rx_setpoint_local / 10);
              setpoint = (setpoint + newvalue) * 10;
              setpoint = constrain(setpoint, 600, 999);

              rx_setpoint = setpoint;
              }
                lcd.setCursor(14,1);
                lcd.print(rx_setpoint);
            
            if (digitalRead(pin_KB2) != 1 && KB2_btn == 1 && (millis() - KB2_debounce) >= 500 ) {
              
              KB2_debounce = millis();
              knob2.write(12);  //12 - direct to save
              KB2_btn = 0;
              }
          } //while

       //**********************************************************************************************************
       //4. LINE, BACK
       if (((digitalRead(pin_KB2) != 1 && KB2_btn == 0 && (millis() - KB2_debounce) >= 500)) && position_K2 == 2) {
          
        ph_setpoint = (float)(EEPROM.read(10) / 10.0);
        rx_setpoint = EEPROMReadlong(15);
        
        menu2_back();
        }

       //**********************************************************************************************************
       //%. LINE, SAVE
       if (((digitalRead(pin_KB2) != 1 && KB2_btn == 0 && (millis() - KB2_debounce) >= 500)) && position_K2 == 3) {
          
          KB2_debounce = millis();
          KB2_btn = 0;
          
            lcd.setCursor(14,3);
            lcd.write(byte(0));
            delay(500);

            if (EEPROM.read(10) != (byte)(ph_setpoint * 10.0)) {
              EEPROM.write(10, (byte)(ph_setpoint * 10.0));
                lcd.clear();
                lcd.setCursor(2,2);
                lcd.print("<req. PH, SAVED>");
                delay(1000);
                lcd.clear();
            } 
            if (EEPROMReadlong(15) != rx_setpoint) {
              EEPROMWritelong(15, rx_setpoint);
                lcd.clear();
                lcd.setCursor(2,2);
                lcd.print("<req. RX, SAVED>");
                delay(1000);
                lcd.clear();
            } 
            
            //knob1.write(0);
            //position_K1 = 0;
            knob2.write(8);   //8 - direct to BACK
            
            lcdrefresh();
       }
  } //if3, end of LEVEL2.2


  // LEVEL 2.3, RIGHT - Absorb&Pool TEMP Setpoints
  //**********************************************************************************************************
  // RIGHT SUBMENU, LEVEL 2.3
  //**********************************************************************************************************
    if (position_K1 == 3) { //if4

      r_menuItem1 = position_K1 + 1;   //Solar-temp
      r_menuItem2 = position_K1 + 2;   //Pool-max_t
      
      //----------------------------
      r_menuUnit1 = 2;    //°C
      r_menuUnit2 = 2;    //°C
        
     lcdrefresh();  //lcd.cleaning
     
      MENU_L2(3);  //lcd.printing. Passing 1 to write narrow(up) in the center of lcd.
       selector();  //pointer-switch

      //** variables casting for below while functions ...
      int solar_temp_casted = (int)solar_temp * 10.0;   //variable for line1
      int pool_max_t_casted = (int)pool_max_t * 10.0;   //variable for line2
      
      //1. LINE settings - Absorber TEMP Setpoint
      //**********************************************************************************************************
      if (((digitalRead(pin_KB2) != 1 && KB2_btn == 0 && (millis() - KB2_debounce) >= 500)) && position_K2 == 0) {
        
        KB2_btn = 1;
        KB2_debounce = millis();
        
        lcd.setCursor(0,0);
        lcd.write(byte(0));
        
        Serial.println(r_menuItems[r_menuItem1]);
        }
        else {
          KB2_btn = 0;
          }
          while (KB2_btn == 1) {
            
            int newvalue = (knob2.read() / 4);
            int lastvalue;
            int setpoint;
            
            if (newvalue != lastvalue) {

             if (setpoint <= 150) {;
                //newvalue = lastvalue;
                knob2.write( (lastvalue * 4) +4);
                }
                else if (setpoint >= 500) {;
                  knob2.write( (lastvalue * 4) - 4);
                }

              // knob2 reading ...  
              setpoint = solar_temp_casted;
              setpoint += newvalue;
              setpoint = constrain(setpoint,150,500);
              
              solar_temp = (float)setpoint / 10;
              
              lastvalue = newvalue;
              }        
                lcd.setCursor(14,0);
                lcd.print(solar_temp, 1);
            
            if (digitalRead(pin_KB2) != 1 && KB2_btn == 1 && (millis() - KB2_debounce) >= 500 ) {
              
              KB2_debounce = millis();
              knob2.write(12);  //12 - direct to save
              KB2_btn = 0;
              }
          
          } //while, line 1

      //2. LINE settings - POOL TEMP Setpoint
      //**********************************************************************************************************
      if (((digitalRead(pin_KB2) != 1 && KB2_btn == 0 && (millis() - KB2_debounce) >= 500)) && position_K2 == 1) {
        
        KB2_btn = 1;
        KB2_debounce = millis();
        
        lcd.setCursor(0,1);
        lcd.write(byte(0));
        
        Serial.println(r_menuItems[r_menuItem2]);
        }
        else {
          KB2_btn = 0;
          }
          while (KB2_btn == 1) {

            int newvalue = (knob2.read() / 4); 
            int lastvalue;
            int setpoint;
            
            if (newvalue != lastvalue) {
             
             if (setpoint <= 200) {;          //border value in contrain below
                knob2.write( (lastvalue * 4) + 4);
                }
                else if (setpoint >= 400) {;  //border value in contrain below
                  knob2.write( (lastvalue * 4) - 4);
                }

              setpoint = pool_max_t_casted;   //variable in the top
              setpoint += newvalue;
              setpoint = constrain(setpoint,200,400);

              pool_max_t = (float)setpoint / 10.0;
              
              lastvalue = newvalue;
              }
                lcd.setCursor(14,1);
                lcd.print(pool_max_t, 1);
            
            if (digitalRead(pin_KB2) != 1 && KB2_btn == 1 && (millis() - KB2_debounce) >= 500 ) {
              
              KB2_debounce = millis();
              knob2.write(12);  //12 - direct to save
              KB2_btn = 0;
              }
          } //while

       //**********************************************************************************************************
       //4. LINE, BACK
       if (((digitalRead(pin_KB2) != 1 && KB2_btn == 0 && (millis() - KB2_debounce) >= 500)) && position_K2 == 2) {

        solar_temp = (float)EEPROMReadlong(20) / 10;
        pool_max_t = (float)EEPROMReadlong(25) / 10;  
        
        menu2_back();
        }

       //**********************************************************************************************************
       //%. LINE, SAVE
       if (((digitalRead(pin_KB2) != 1 && KB2_btn == 0 && (millis() - KB2_debounce) >= 500)) && position_K2 == 3) {
          
          KB2_debounce = millis();
          KB2_btn = 0;
          
            lcd.setCursor(14,3);
            lcd.write(byte(0));
            delay(500);

            if (EEPROMReadlong(20) != (int)(solar_temp * 10.0)) {
              EEPROMWritelong(20, (int)(solar_temp * 10.0));
                lcd.clear();
                lcd.setCursor(1,2);
                lcd.print("<req. SolT, SAVED>");
                delay(1000);
                lcd.clear();
            } 
            if (EEPROMReadlong(25) != (int)(pool_max_t * 10.0)) {
              EEPROMWritelong(25, (int)(pool_max_t * 10.0));
                lcd.clear();
                lcd.setCursor(1,2);
                lcd.print("<req. PMxT, SAVED>");
                delay(1000);
                lcd.clear();
            } 
            
            //knob1.write(0);
            //position_K1 = 0;
            knob2.write(8);   //8 - direct to BACK
            
            lcdrefresh();
       }
  } //if4, end of LEVEL 2.3.

  //LEVEL 2.4, RIGHT - TIME&DATE Setings
  //**********************************************************************************************************
  //RIGHT SUBMENU, LEVEL 2.4 - TIME&DATE
  //**********************************************************************************************************
  
    if (position_K1 == 4) {           //if8

      r_menuItem1 = position_K1 + 2;    //Set-date
      r_menuItem2 = position_K1 + 3;    //Set-time
      
      //----------------------------
      r_menuUnit1 = 5;    //nothing
      r_menuUnit2 = 5;    //nothing
        
      lcdrefresh();  //lcd.cleaning
     
      MENU_L2(1);  //lcd.printing. Passing 1 to write narrow(up) in the center of lcd.
       selector();  //pointer-switch
      
      //1. LINE settings - DATE SET
      //**********************************************************************************************************
      if (((digitalRead(pin_KB2) != 1 && KB2_btn == 0 && (millis() - KB2_debounce) >= 500)) && position_K2 == 0) {
        
        KB2_btn = 1;
        KB2_debounce = millis();
        
        lcd.setCursor(0,0);
        lcd.write(byte(0));
        }
        else {
          KB2_btn = 0;
          }
          
          while (KB2_btn == 1) {
            
            int newvalue = (knob2.read() / 4) + 1;
            int lastvalue;
            
            if (newvalue != lastvalue) {

              lcd.setCursor(14,0);        
            
              if (newvalue %2 == 0) {
                lcd.print("-not-");
                set_date = false;
                } 
                else {
                  lcd.print("START");
                  set_date = true;
                  }
                            
              lastvalue = newvalue;
              }        
          
            if (digitalRead(pin_KB2) != 1 && KB2_btn == 1 && (millis() - KB2_debounce) >= 500 ) {

              KB2_debounce = millis();
              knob2.write(12);  //12 - direct to save
              KB2_btn = 0;
              }
                        
          } //while, line 1

      //2. LINE settings - TIME Set
      //**********************************************************************************************************
      if (((digitalRead(pin_KB2) != 1 && KB2_btn == 0 && (millis() - KB2_debounce) >= 500)) && position_K2 == 1) {
        
        KB2_btn = 1;
        KB2_debounce = millis();
        
        lcd.setCursor(0,1);
        lcd.write(byte(0));
        }
        else {
          KB2_btn = 0;
          }
          
          while (KB2_btn == 1) {
            
            int newvalue = (knob2.read() / 4);
            int lastvalue;
            
            if (newvalue != lastvalue) {

              lcd.setCursor(14,1);        
            
              if (newvalue %2 == 0) {
                lcd.print("-not-");
                set_time = false;
                } 
                else {
                  lcd.print("START");
                  set_time = true;
                  }
                            
              lastvalue = newvalue;
              }        
          
            if (digitalRead(pin_KB2) != 1 && KB2_btn == 1 && (millis() - KB2_debounce) >= 500 ) {

              KB2_debounce = millis();
              knob2.write(12);  //12 - direct to save
              KB2_btn = 0;
              }
                        
          } //while, line 1          
       //**********************************************************************************************************
       //4. LINE, BACK
       if (((digitalRead(pin_KB2) != 1 && KB2_btn == 0 && (millis() - KB2_debounce) >= 500)) && position_K2 == 2) {

        menu2_back();
        }

       //**********************************************************************************************************
       //%. LINE, SAVE
       if (((digitalRead(pin_KB2) != 1 && KB2_btn == 0 && (millis() - KB2_debounce) >= 500)) && position_K2 == 3) {
          
          KB2_debounce = millis();
          KB2_btn = 0;
          
            lcd.setCursor(14,3);
            lcd.write(byte(0));
            delay(500);

            if (set_time == true && set_date == true) {
                lcd.clear();
                lcd.setCursor(2,2);
                lcd.print("<TIME SETTING  &");
                delay(1000);
                lcd.clear();
                lcd.setCursor(2,2);
                lcd.print("<DATE SETTING ->");
                delay(1000);

                //JUMP to time setting procedure, see the void on TimeDateVoid tab
                set_time_now();
                set_time = false;
                
                //JUMP to date setting procedure, see the void on TimeDateVoid tab
                set_date_now();    
                set_date = false;
                
                //clear LCD after time&date was stored
                lcd.setCursor(14,0);        
                lcd.print("-not-");
                lcd.setCursor(14,1);        
                lcd.print("-not-");
            } 
            else if (set_date == true && set_time == false) {
                lcd.clear();
                lcd.setCursor(2,2);
                lcd.print("<DATE SETTING ->");
                delay(1000);

                //JUMP to date setting procedure, see the void on TimeDateVoid tab
                set_date_now();
                
                //clear LCD after
                lcd.setCursor(14,0);        
                lcd.print("-not-");
                
                //new date was stored sucessfuly
                set_date = false;
                } 
            else if (set_time == true && set_date == false) {
                lcd.clear();
                lcd.setCursor(2,2);
                lcd.print("<TIME SETTING ->");
                delay(1000);

                //JUMP to time setting procedure, see the void on TimeSetVoid tab
                set_time_now();

                //clear LCD after
                lcd.setCursor(14,1);        
                lcd.print("-not-");

                //new time was stored sucessfuly
                set_time = false;
                } 

        knob2.write(8);   //8 - direct to BACK
        lcdrefresh();
       } // save
       
    } //if 8


 // MENU END (menu cycle)
 if (position_K1 == 5) { 
  
  newK1 = 1;
  knob1.write(2);
  position_K1 = 1;

  lcdrefresh();
 }
  
  // RIGHT MENU
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // LEFT MENU
  

  
  //LEVEL 2.4, LEFT
  //**********************************************************************************************************
  //LEFT SUBMENU, LEVEL 2.4
  //**********************************************************************************************************
  
    if (position_K1 == -1) {           //if5

      l_menuItem1 = position_K1 + 1;      //pH-calibration RUN
      l_menuItem2 = position_K1 +2 ;      //pH-solution dozing portion
      
      //----------------------------
      l_menuUnit1 = 2;    //nothing
      l_menuUnit2 = 1;    // ml/m3
        
      lcdrefresh();  //lcd.cleaning
     
      MENU_L2(1);  //lcd.printing. Passing 2 to write narrow(down) in the center of lcd.
       selector();  //pointer-switch

      //** variables casting for below while functions ...
      byte ph_solaction_local = ph_solaction;
      
      //1. LINE settings
      //**********************************************************************************************************
      if (((digitalRead(pin_KB2) != 1 && KB2_btn == 0 && (millis() - KB2_debounce) >= 500)) && position_K2 == 0) {
        
        KB2_btn = 1;
        KB2_debounce = millis();
        
        lcd.setCursor(0,0);
        lcd.write(byte(0));
        }
        else {
          KB2_btn = 0;
          }
          
          while (KB2_btn == 1) {

            int newvalue = (knob2.read() / 4);
            int lastvalue;
            
            if (newvalue != lastvalue) {

              lcd.setCursor(14,0);        
            
              if (newvalue %2 == 0) {
                lcd.print("-not-");
                c_ph4_start = false;
                c_ph7_start = false;
                } 
                else {
                  lcd.print("START");
                  c_ph4_start = true;
                  c_ph7_start = true;  
                  }
                            
              lastvalue = newvalue;
              }        
            
            if (digitalRead(pin_KB2) != 1 && KB2_btn == 1 && (millis() - KB2_debounce) >= 500 ) {

              KB2_debounce = millis();
              knob2.write(12);  //12 - direct to save
              KB2_btn = 0;
              }
                        
          } //while, line 1

      //2. LINE settings
      //**********************************************************************************************************
      if (((digitalRead(pin_KB2) != 1 && KB2_btn == 0 && (millis() - KB2_debounce) >= 500)) && position_K2 == 1) {
        
        KB2_btn = 1;
        KB2_debounce = millis();
        
        lcd.setCursor(0,1);
        lcd.write(byte(0));
        }
        else {
          KB2_btn = 0;
          }
          
          while (KB2_btn == 1) {

            int newvalue = (knob2.read() / 4); 
            int lastvalue;
            byte setpoint;
            
            if (newvalue != lastvalue) {
             
             if (setpoint <= 1) {                     //border value in contrain below
                knob2.write( (lastvalue * 4) + 3);
                }
                else if (setpoint >= 60) {            //border value in contrain below
                  knob2.write( (lastvalue * 4) - 3);
                }

              setpoint = ph_solaction_local; 
              setpoint += newvalue;
              setpoint = constrain(setpoint, 1, 60);

              ph_solaction = setpoint;
              
              lastvalue = newvalue;
              }

                //LCD value print inside while
                if (ph_solaction < 10) {              //shift from XX number to " "X
                  lcd.setCursor(14,1);
                  lcd.print(" ");
                  lcd.setCursor(15,1);
                  lcd.print(ph_solaction);                  
                }
                else {
                  lcd.setCursor(14,1);
                  lcd.print(ph_solaction);   
                  }
            
            if (digitalRead(pin_KB2) != 1 && KB2_btn == 1 && (millis() - KB2_debounce) >= 500 ) {
              
              KB2_debounce = millis();
              knob2.write(12);  //12 - direct to save
              KB2_btn = 0;
              }

          } //while, line 2

       //**********************************************************************************************************
       //4. LINE, BACK
       if (((digitalRead(pin_KB2) != 1 && KB2_btn == 0 && (millis() - KB2_debounce) >= 500)) && position_K2 == 2) {

        menu2_back();
        }

       //**********************************************************************************************************
       //%. LINE, SAVE
       if (((digitalRead(pin_KB2) != 1 && KB2_btn == 0 && (millis() - KB2_debounce) >= 500)) && position_K2 == 3) {
          
          KB2_debounce = millis();
          KB2_btn = 0;
          
            lcd.setCursor(14,3);
            lcd.write(byte(0));
            delay(500);

            //the orded of changes has to be reversed becouse of knob.write(0) for calibration, otherwise we lose change at sec. line
            if (EEPROM.read(30) != ph_solaction) {
              EEPROM.write(30, ph_solaction);
                lcd.clear();
                lcd.setCursor(2,2);
                lcd.print("<PH SOL. SAVED>");
                delay(1000);
                lcd.clear();
                }

            if (c_ph4_start == true && c_ph7_start == true) {
                lcd.clear();
                lcd.setCursor(2,2);
                lcd.print("<PH CAL. QUEUED>");
                delay(1000);

            //here you should add calibration()
            
            //reset both knobs position and jump directly to calibration process based on bools variables
            knob1.write(0);     
            knob2.write(0);
            } 

        knob2.write(8);   //8 - direct to BACK
        lcdrefresh();
       } // save
       
  } //if5, end of LEVEL2.1, LEFT


  //LEVEL 2.5, LEFT
  //**********************************************************************************************************
  //LEFT SUBMENU, LEVEL 2.5
  //**********************************************************************************************************
  
    if (position_K1 == -2) {           //if6

      l_menuItem1 = position_K1 + 4;      //RX-calibration RUN
      l_menuItem2 = position_K1 + 5 ;     //RX-solution dozing portion
      
      //---------------------------- Units on the screen
      l_menuUnit1 = 2;    //nothing
      l_menuUnit2 = 1;    // ml/m3
        
      lcdrefresh();  //lcd.cleaning
     
      MENU_L2(3);  //lcd.printing. Passing 3 to write narrow(down+up) in the center of lcd.
       selector();  //pointer-switch

      //** variables casting for below while functions ...
      byte rx_solaction_local = rx_solaction;
      
      //1. LINE settings
      //**********************************************************************************************************
      if (((digitalRead(pin_KB2) != 1 && KB2_btn == 0 && (millis() - KB2_debounce) >= 500)) && position_K2 == 0) {
        
        KB2_btn = 1;
        KB2_debounce = millis();
        
        lcd.setCursor(0,0);
        lcd.write(byte(0));
        }
        else {
          KB2_btn = 0;
          }
          
          while (KB2_btn == 1) {

            int newvalue = (knob2.read() / 4); 
            int lastvalue;
            
            if (newvalue != lastvalue) {
             
              lcd.setCursor(14,0);        
            
              if (newvalue %2 == 0) {
                lcd.print("-not-");
                c_rx475_start = false;
                c_rx645_start = false;
                } 
                else {
                  lcd.print("START");
                  c_rx475_start = true;
                  c_rx645_start = true;  
                  }
                            
              lastvalue = newvalue;
              }        
            
            if (digitalRead(pin_KB2) != 1 && KB2_btn == 1 && (millis() - KB2_debounce) >= 500 ) {
              
              KB2_debounce = millis();
              knob2.write(12);  //12 - direct to save
              KB2_btn = 0;
              }
          
          } //while, line 1

      //2. LINE settings
      //**********************************************************************************************************
      if (((digitalRead(pin_KB2) != 1 && KB2_btn == 0 && (millis() - KB2_debounce) >= 500)) && position_K2 == 1) {
        
        KB2_btn = 1;
        KB2_debounce = millis();
        
        lcd.setCursor(0,1);
        lcd.write(byte(0));
        }
        else {
          KB2_btn = 0;
          }
          
          while (KB2_btn == 1) {

            int newvalue = (knob2.read() / 4); 
            int lastvalue;
            byte setpoint;
            
            if (newvalue != lastvalue) {
             
             if (setpoint <= 1) {                     //border value in contrain below
                knob2.write( (lastvalue * 4) + 3);
                }
                else if (setpoint >= 60) {            //border value in contrain below
                  knob2.write( (lastvalue * 4) - 3);
                }

              setpoint = rx_solaction_local; 
              setpoint += newvalue;
              setpoint = constrain(setpoint, 1, 60);

              rx_solaction = setpoint;
              
              lastvalue = newvalue;
              }

                //LCD value print inside while
                if (rx_solaction < 10) {              //shift from XX number to " "X
                  lcd.setCursor(14,1);
                  lcd.print(" ");
                  lcd.setCursor(15,1);
                  lcd.print(rx_solaction);                  
                }
                else {
                  lcd.setCursor(14,1);
                  lcd.print(rx_solaction);   
                }
            
            if (digitalRead(pin_KB2) != 1 && KB2_btn == 1 && (millis() - KB2_debounce) >= 500 ) {
              
              KB2_debounce = millis();
              knob2.write(12);  //12 - direct to save
              KB2_btn = 0;
              }
          
          } //while, line 2

       //**********************************************************************************************************
       //4. LINE, BACK
       if (((digitalRead(pin_KB2) != 1 && KB2_btn == 0 && (millis() - KB2_debounce) >= 500)) && position_K2 == 2) {

        menu2_back();
        }

       //**********************************************************************************************************
       //%. LINE, SAVE
       if (((digitalRead(pin_KB2) != 1 && KB2_btn == 0 && (millis() - KB2_debounce) >= 500)) && position_K2 == 3) {
          
          KB2_debounce = millis();
          KB2_btn = 0;
          
            lcd.setCursor(14,3);
            lcd.write(byte(0));
            delay(500);

            //the orded of changes has to be reversed becouse of knob.write(0) for calibration, otherwise we lose change at sec. line
            if (EEPROM.read(35) != rx_solaction) {
              EEPROM.write(35, rx_solaction);
                lcd.clear();
                lcd.setCursor(2,2);
                lcd.print("<RX SOL. SAVED>");
                delay(1000);
                lcd.clear();
                } 
            
            if (c_rx475_start == true && c_rx645_start == true) {
                lcd.clear();
                lcd.setCursor(2,2);
                lcd.print("<RX CAL. QUEUED>");
                delay(1000);

            //here you should add calibration()

            //reset both knobs position and jump directly to calibration process based on bools variables
            knob1.write(0);     
            knob2.write(0);
            } 

        knob2.write(8);   //8 - direct to BACK
        lcdrefresh();
       } // save
       
  } //if6, end of LEVEL2.2, LEFT


  //LEVEL 2.6, LEFT
  //**********************************************************************************************************
  //LEFT SUBMENU, LEVEL 2.6
  //**********************************************************************************************************
  
    if (position_K1 == -3) {          //if7

      l_menuItem1 = position_K1 + 7;      //PH-dozing medium selection
      l_menuItem2 = position_K1 + 8 ;     // Pool/Reservoar Volume in m3
     
      //---------------------------- Units on the screen
      l_menuUnit1 = 2;    //nothing
      l_menuUnit2 = 0;    //nothing
        
      lcdrefresh();  //lcd.cleaning
     
      MENU_L2(1);  //lcd.printing. Passing1 to write narrow(up) in the center of lcd.
       selector();  //pointer-switch

      //** variables casting for below while functions ...
      byte pool_vol_local = pool_vol;       //variable for line1
      
      //1. LINE settings
      //**********************************************************************************************************
      if (((digitalRead(pin_KB2) != 1 && KB2_btn == 0 && (millis() - KB2_debounce) >= 500)) && position_K2 == 0) {
        
        KB2_btn = 1;
        KB2_debounce = millis();
        
        lcd.setCursor(0,0);
        lcd.write(byte(0));
        }
        else {
          KB2_btn = 0;
          }
        
          while (KB2_btn == 1) {

            int newvalue = (knob2.read() / 4); 
            int lastvalue;
            
            if (newvalue != lastvalue) {
             
              lcd.setCursor(14,0);        
            
              if (newvalue %2 == 0) {
                ph_plus_medium = false;     //0 = minus
                } 
                else {
                  ph_plus_medium = true;   //1 = plus
                  }
              
              lcd.print(ph_medium[ph_plus_medium]);                            
              
              lastvalue = newvalue;
              }        
            
            if (digitalRead(pin_KB2) != 1 && KB2_btn == 1 && (millis() - KB2_debounce) >= 500 ) {
              
              KB2_debounce = millis();
              knob2.write(12);  //12 - direct to save
              KB2_btn = 0;
              }
          
          } //while, line 1


      //2. LINE settings
      //**********************************************************************************************************
      if (((digitalRead(pin_KB2) != 1 && KB2_btn == 0 && (millis() - KB2_debounce) >= 500)) && position_K2 == 1) {
        
        KB2_btn = 1;
        KB2_debounce = millis();
        
        lcd.setCursor(0,0);
        lcd.write(byte(0));
        }
        else {
          KB2_btn = 0;
          }
          
          while (KB2_btn == 1) {
            
            int newvalue = (knob2.read() / 4);
            int lastvalue;
            byte setpoint;
            
            if (newvalue != lastvalue) {

             if (setpoint <= 1) {;
                knob2.write( (lastvalue * 4) + 2);
                lcdrefresh(); //becouse of zero et the end
                }
                else if (setpoint >= 99) {;
                  knob2.write( (lastvalue * 4) - 2);
                }

              // knob2 reading ...  
              setpoint = pool_vol_local; 
              setpoint += newvalue;
              setpoint = constrain(setpoint,1,99);

              pool_vol = setpoint;
              
              lastvalue = newvalue;
              } 

                //LCD value print inside while
                if (pool_vol < 10) {              //shift from XX number to " "X
                  lcd.setCursor(14,1);
                  lcd.print(" ");
                  lcd.setCursor(15,1);
                  lcd.print(pool_vol);                  
                  }
                  else {
                    lcd.setCursor(14,1);
                    lcd.print(pool_vol);
                    }
            
            if (digitalRead(pin_KB2) != 1 && KB2_btn == 1 && (millis() - KB2_debounce) >= 500 ) {
              
              KB2_debounce = millis();
              knob2.write(12);  //12 - direct to save
              KB2_btn = 0;
              }
          
          } //while, line 1


       //**********************************************************************************************************
       //4. LINE, BACK
       if (((digitalRead(pin_KB2) != 1 && KB2_btn == 0 && (millis() - KB2_debounce) >= 500)) && position_K2 == 2) {

        menu2_back();
        }

       //**********************************************************************************************************
       //5. LINE, SAVE
       if (((digitalRead(pin_KB2) != 1 && KB2_btn == 0 && (millis() - KB2_debounce) >= 500)) && position_K2 == 3) {
          
          KB2_debounce = millis();
          KB2_btn = 0;
          
            lcd.setCursor(14,3);
            lcd.write(byte(0));
            delay(500);

            //the orded of changes has to be reversed becouse of knob.write(0) for calibration, otherwise we lose change at sec. line
            if (EEPROM.read(40) != ph_plus_medium) {
              EEPROM.write(40, ph_plus_medium);
                lcd.clear();
                lcd.setCursor(2,2);
                lcd.print("<MEDI. SLECTED>");
                delay(1000);
                lcd.clear();
                } 
            
            if (EEPROM.read(1) != pool_vol) {
              EEPROM.write(1, pool_vol);
                lcd.clear();
                lcd.setCursor(2,2);
                lcd.print("<VOLUME, SAVED>");
                delay(1000);
                lcd.clear();
            } 

        knob2.write(8);   //8 - direct to BACK
        lcdrefresh();
       } // save
       
  } //if7, end of LEVEL2.2, LEFT




}  //END of MAIN LOOP function


//------------------------------------------------------------------------------------------------------------------ VOIDs


void lcdrefresh() {
  
    if (position_K1 != position_K1_last) {
    position_K1_last = position_K1;
    cleared = false;  
    } //if1
    
/*  if (position_K2 != position_K2_last) {
    position_K2_last = position_K2;
    cleared = false;  
    }
*/    //if2
  
  if (cleared == false) {
    lcd.clear();
    cleared = true;    
  } //if3
  
} //lcdrefresh

void selector() {
  
       //cursor movement - menu item selector
     switch (position_K2) {
      case 0:
        //lcdrefresh();
        lcd.setCursor(0,1);
        lcd.print(" ");
        lcd.setCursor(0,3);
        lcd.print(" ");
        lcd.setCursor(14,3);     
        lcd.print(" ");

        lcd.setCursor(0,0);
        lcd.print(">");
        break;
      case 1:
//        lcdrefresh();
        lcd.setCursor(0,0);
        lcd.print(" ");
        lcd.setCursor(0,3);
        lcd.print(" ");
        lcd.setCursor(14,3);     
        lcd.print(" ");
        
        lcd.setCursor(0,1);
        lcd.print(">");
        break;
      case 2:
        //lcdrefresh();
        lcd.setCursor(0,0);
        lcd.print(" ");
        lcd.setCursor(0,1);
        lcd.print(" ");
        lcd.setCursor(14,3);     
        lcd.print(" ");
        
        lcd.setCursor(0,3);
        lcd.print(">");
        break;
      case 3:
        //lcdrefresh();
        lcd.setCursor(0,0);
        lcd.print(" ");
        lcd.setCursor(0,1);
        lcd.print(" ");
        lcd.setCursor(0,3);     
        lcd.print(" ");
        
        lcd.setCursor(14,3);     
        lcd.print(">");
        break;
      default:
        //lcdrefresh();
        lcd.setCursor(0,0);
        lcd.print(" ");
        lcd.setCursor(0,1);
        lcd.print(" ");
        lcd.setCursor(0,3);     
        lcd.print(" ");
        
        lcd.setCursor(14,3);     
        lcd.print(">");
        knob2.write(1);
        break;
     } //switch
     
} //selector

void MENU_L2(byte lcdwrite) {

  // timeout if display stay untached. Menu timeout 60s.
  if (millis() >= (MENU_timeout + 60000) ) {
    MENU_timeout = millis(); 
    position_K1 = 0;
    position_K2 = 0;
    knob1.write(0);
    knob1.write(0);
    } //lcd-timout

  // RIGHT SIDE MENU2
  if (position_K1 >= 1) {             // check if previous turn of knob1 was to right or left

    //------------------ first line on LCD  
    lcd.setCursor(1,0);
    lcd.print(r_menuItems[r_menuItem1]);

    lcd.setCursor(14,0);
      switch (r_menuItem1) {
        // L2-page 1, volume&filtration
        case 0:
        lcd.print(pool_vol);
        break;
        // L2-page 2, PH
        case 2:
        lcd.print(ph_setpoint, 1);
        break;
        // L2-page 3, solar temp. setpoints  
        case 4:
        lcd.print(solar_temp, 1);
        break;
        // L2-page 4, date setting()
        case 6:
         if (set_date) lcd.print("START");
          else lcd.print("-not-");
          break;
        default:
        lcd.print("Err");
        break;
        } //swtich
     
      if (r_menuItem1 == 4) {
        lcd.setCursor(18,0);
        lcd.write(byte(4));     //°
        lcd.setCursor(19,0);  
        lcd.print(r_menuUnits[r_menuUnit1]); 
        } 
        else if (r_menuItem1 == 6) {
          lcd.setCursor(19,0);
          lcd.print(r_menuUnits[r_menuUnit1]); 
          }
          else {
            lcd.setCursor(18,0);
            lcd.print(r_menuUnits[r_menuUnit1]);  
            }
    
    //------------------ secound line on LCD
    lcd.setCursor(1,1);
    lcd.print(r_menuItems[r_menuItem2]);
  
    lcd.setCursor(14,1);
      switch (r_menuItem2) {
        // L2-page 1, filtration
        case 1:
        lcd.print(filt_hours);
        break;    
        // L2-page 2, RX
        case 3:
        lcd.print(rx_setpoint);
        break;
        // L2-page 3, pool_max temp. setpoints  
        case 5:
        lcd.print(pool_max_t, 1);
        break;
        // L2-page 4, time setting()
        case 7:
         if (set_time) lcd.print("START");
          else lcd.print("-not-");
          break;        
        default:
        lcd.print("Err");
        break;
        } // switch
      
      if (r_menuItem2 == 5) {
        lcd.setCursor(18,1);
        lcd.write(byte(4));     //°
        lcd.setCursor(19,1);  
        lcd.print(r_menuUnits[r_menuUnit2]); 
        } else if (r_menuItem2 == 7) {
            lcd.setCursor(19,1);
            lcd.print(r_menuUnits[r_menuUnit2]); 
            } 
            else {
              lcd.setCursor(18,1);
              lcd.print(r_menuUnits[r_menuUnit2]); 
              }

  } // if knob1 is on right side
  //------------------   

  // LEFT SIDE MENU2
  if (position_K1 <= -1) {             // check if previous turn of knob1 was to right or left

    //------------------ first line on LCD  
    lcd.setCursor(1,0);
    lcd.print(l_menuItems[l_menuItem1]);

    lcd.setCursor(14,0);
      switch (l_menuItem1) {
        // L2-page 1, PH calibration() starting point 
        case 0:
         if (c_ph4_start && c_ph7_start) lcd.print("START");
          else lcd.print("-not-");
          break;
        // L2-page 2, RX calibration() starting point 
        case 2:
          if (c_rx475_start && c_rx645_start) lcd.print("START");
           else lcd.print("-not-");
           break;
        // L2-page 3, PH dozing medium selection
        case 4:
        lcd.print(ph_medium[ph_plus_medium]);
        break;
        default:
        lcd.print("Err");
        break;
        } //swtich
        
        lcd.setCursor(19,0);  
        lcd.print(l_menuUnits[l_menuUnit1]); 
    
    //------------------ secound line on LCD
    lcd.setCursor(1,1);
    lcd.print(l_menuItems[l_menuItem2]);
  
    lcd.setCursor(14,1);
      switch (l_menuItem2) {
        // L2-page 1, Amount of pH sol. dozing 
        case 1:
        lcd.print(ph_solaction);
          lcd.setCursor(17,2);  
          lcd.print("/");
          lcd.setCursor(18,2);  
          lcd.print(l_menuUnits[0]);
        break;    
        // L2-page 2, Amount of RX sol. dozing
        case 3:
        lcd.print(rx_solaction);
          lcd.setCursor(17,2);  
          lcd.print("/");
          lcd.setCursor(18,2);  
          lcd.print(l_menuUnits[0]);
        break;
        // L2-page 3, Pool or reservoar volume (m3)
        case 5:
        lcd.print(pool_vol);
        break;
        default:
        lcd.print("Err");
        break;
        } // switch

      lcd.setCursor(18,1);
      lcd.print(l_menuUnits[l_menuUnit2]); 
  
  } // if knob1 is on left side
  //------------------   LEFT SIDE
  
  lcd.setCursor(1,3);
  lcd.print("*BACK");
  lcd.setCursor(15,3);
  lcd.print("*SAVE");
  
  lcd.setCursor(10,3);
  lcd.write(byte(lcdwrite)); 
} //MENU_L2


void menu2_back() {
  
  KB2_debounce = millis();
  KB2_btn = 0;

  lcd.setCursor(0,3);
  lcd.write(byte(0));
  delay(500);

  knob1.write(0);
  knob2.write(0);
            
  position_K1 = 0;
  
  lcdrefresh();
}




