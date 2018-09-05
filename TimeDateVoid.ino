
//lcd nastaveni casu
void set_time_now() {     

  lcd.clear();
  knob2.write(8);    //reset last knob position

  //display actual time on LCD
  lcd.setCursor(2,0);
  lcd.print("HH");
  lcd.setCursor(5,0);
  lcd.print("MM");
  lcd.setCursor(8,0);
  lcd.print("SS");
  //----------------------
  
  // hours
  if (currenthour < 10) {
    lcd.setCursor(2,1);  
    lcd.print("0");
    lcd.setCursor(3,1);
    lcd.print(currenthour);
    } else {
      lcd.setCursor(2,1);
      lcd.print(currenthour);
      }
  lcd.setCursor(4,1);
  lcd.print(":");
  
  //minutes
  if (currentminute < 10) {
    lcd.setCursor(5,1);  
    lcd.print("0");
    lcd.setCursor(6,1);
    lcd.print(currentminute);
    } else {
      lcd.setCursor(5,1);
      lcd.print(currentminute);
      }

  lcd.setCursor(7,1);
  lcd.print(":");

  //seconds
  if (currentseconde < 10) {
    lcd.setCursor(8,1);  
    lcd.print("0");
    lcd.setCursor(9,1);
    lcd.print(currentseconde);
    } else {
      lcd.setCursor(8,1);
      lcd.print(currentseconde);
      }

  //----------------------
  lcd.setCursor(15,3);
  lcd.print("*SAVE");

  //cursor position under digits
  byte cursor_position;
  
  //variables for bellow while fncs.
  boolean saved = false;
  boolean settime = false;
  byte knob2value = 0;

  // WHILE LOOP UNTIL *SAVE will selected/pressed
  while (!saved) {    //while loop 1

    //time digit (number) settime depend on cursor position
    cursor_position = (knob2.read() / 4);
    
    // ***********************************************************
    // HOURS set function 
    if (cursor_position == 2) {   //position under first number
      
      lcd.setCursor(5,2);
      lcd.print(" ");
      lcd.setCursor(8,2);
      lcd.print(" ");
      lcd.setCursor(14,3);
      lcd.print(" ");
      
      lcd.setCursor(2,2);
      lcd.write(byte(5));   //unslectted pointer under first digit

      //pressed-button - now going to change "hours"
      if (digitalRead(pin_KB2) != 1 && (millis() - KB2_debounce) >= 500) {  
      
        KB2_debounce = millis();

        settime = true;
        
        //keep current hour to be adjusted down in next while
        knob2value = currenthour;
        knob2.write(knob2value * 4);
        } 

        //setting loop fnc.
        while (settime) {    //first digit selected for a change

          lcd.setCursor(2,2);
          lcd.write(byte(6));   //number (for HOURS) is now selected

          knob2value = (knob2.read() / 4);

          // range (0-24)
          if (knob2value > 24) {
            knob2value = 0;
            knob2.write(0);
            } 

          // value postion shifting
          if (knob2value < 10) {
            lcd.setCursor(2,1);  
            lcd.print("0");
            lcd.setCursor(3,1);
            lcd.print(knob2value);
            } else {
              lcd.setCursor(2,1);
              lcd.print(knob2value);
              }

        //if KB2-button is pressed, this loop is canceld and value is set but not yet saved
        if (digitalRead(pin_KB2) != 1 && (millis() - KB2_debounce) >= 500) {
      
            KB2_debounce = millis();

            //control variable for this while loop 
            settime = false;        

            //passing value to time variable
            currenthour = knob2value;

            //put cursor back on prev. position
            knob2.write(cursor_position * 4);
          }
        } //while
    } // hours


    // ***********************************************************
    // MINUTES set function 
    else if (cursor_position == 3) {   //position under minutes number
      
      lcd.setCursor(2,2);
      lcd.print(" ");
      lcd.setCursor(8,2);
      lcd.print(" ");
      lcd.setCursor(14,3);
      lcd.print(" ");
      
      lcd.setCursor(5,2);
      lcd.write(byte(5));   //roof unslectted pointer under secound digit

      //pressed-button - now going to change "minutes"
      if (digitalRead(pin_KB2) != 1 && (millis() - KB2_debounce) >= 500) {  
      
        KB2_debounce = millis();

        //jump to while loop
        settime = true;
        
        //keep current minute to be adjusted down in next while
        knob2value = currentminute;
        knob2.write(knob2value * 4);
        } 

        //setting loop fnc.
        while (settime) {    //first digit selected for a change

          lcd.setCursor(5,2);
          lcd.write(byte(6));   //number (for MINUTE) is now selected

          knob2value = (knob2.read() / 4);

          // range (0-60)
          if (knob2value > 60) {
            knob2value = 0;
            knob2.write(0);
            } 

          // value postion shifting
          if (knob2value < 10) {
            lcd.setCursor(5,1);  
            lcd.print("0");
            lcd.setCursor(6,1);
            lcd.print(knob2value);
            } else {
              lcd.setCursor(5,1);
              lcd.print(knob2value);
              }

        //if KB2-button is pressed, this loop is canceld and value is set but not yet saved
        if (digitalRead(pin_KB2) != 1 && (millis() - KB2_debounce) >= 500) {
      
            KB2_debounce = millis();

            //control variable for this while loop 
            settime = false;        

            //passing value to time variable
            currentminute = knob2value;

            //put cursor back on prev. position
            knob2.write(cursor_position * 4);
          }
        } //while
    } //minutes
      
    
    // ***********************************************************
    // SECONDS set function 
    else if ( cursor_position == 4) {   //position under seconds number

      lcd.setCursor(2,2);
      lcd.print(" ");
      lcd.setCursor(5,2);
      lcd.print(" ");
      lcd.setCursor(14,3);
      lcd.print(" ");
            
      lcd.setCursor(8,2);
      lcd.write(byte(5));   //roof unselected pointer under third digit

      //pressed-button - now going to change "seconds"
      if (digitalRead(pin_KB2) != 1 && (millis() - KB2_debounce) >= 500) {  
      
        KB2_debounce = millis();

        settime = true;
        
        //keep current second to be adjusted down in next while
        knob2value = currentseconde;
        knob2.write(knob2value * 4);
        } 

        //setting loop fnc.
        while (settime) {    //first digit selected for a change

          lcd.setCursor(8,2);
          lcd.write(byte(6));   //number (for SECOND) is now selected

          knob2value = (knob2.read() / 4);

          // range (0-60)
          if (knob2value > 60) {
            knob2value = 0;
            knob2.write(0);
            } 

          // value postion shifting
          if (knob2value < 10) {
            lcd.setCursor(8,1);  
            lcd.print("0");
            lcd.setCursor(9,1);
            lcd.print(knob2value);
            } else {
              lcd.setCursor(8,1);
              lcd.print(knob2value);
              }

        //if KB2-button is pressed, this loop is canceld and value is set but not yet saved
        if (digitalRead(pin_KB2) != 1 && (millis() - KB2_debounce) >= 500) {
      
            KB2_debounce = millis();

            //control variable for this while loop 
            settime = false;        

            //passing value to time variable
            currentseconde = knob2value;

            //put cursor back on prev. position
            knob2.write(cursor_position * 4);
          }
        } //while
    } //seconds
      
     // if 4
    else if ( cursor_position == 5) {   //position under third number

      lcd.setCursor(2,2);
      lcd.print(" ");
      lcd.setCursor(5,2);
      lcd.print(" ");
      lcd.setCursor(8,2);
      lcd.print(" ");
      
      lcd.setCursor(14,3);
      lcd.print(">");   // pointer under third digit

      //pressed-button - now going to write them all
      if (digitalRead(pin_KB2) != 1 && (millis() - KB2_debounce) >= 500) {  
      
        KB2_debounce = millis();
        
        lcd.setCursor(14,3);
        lcd.write(byte(0));

        // Manual clock&date set (YYYY, MM, DD, HH, II, SS)
        clock.setDateTime(dt.year, dt.month, dt.day, currenthour, currentminute, currentseconde);

          lcd.clear();
          lcd.setCursor(3,2);
          lcd.print("<TIME, SAVED>");
          
        knob2.write(0);
        delay(1000);

          lcd.clear();

        // cancel of time-change loop
        saved = true;
        }
    } 
    else {
      if (cursor_position >= 6) {
        knob2.write(8);
        cursor_position = 2;        
        }
      if (cursor_position <= 1) {
        knob2.write(24);
        cursor_position = 4;        
        }
      } //else
  
  } // SAVE while loop

} //void set-time


//--------------------------------------------------------------------------------------------------------------------


//lcd nastaveni casu
void set_date_now() {     

  lcd.clear();
  knob2.write(8);    //reset last knob position

  //display actual time on LCD
  lcd.setCursor(2,0);
  lcd.print("DD");
  lcd.setCursor(5,0);
  lcd.print("MM");
  lcd.setCursor(8,0);
  lcd.print("YYYY");
  //----------------------
  // days
  if (currentday < 10) {
    lcd.setCursor(2,1);  
    lcd.print("0");
    lcd.setCursor(3,1);
    lcd.print(currentday);
    } else {
      lcd.setCursor(2,1);
      lcd.print(currentday);
      }
  lcd.setCursor(4,1);
  lcd.print("-");
  
  //months
  if (currentmonth < 10) {
    lcd.setCursor(5,1);  
    lcd.print("0");
    lcd.setCursor(6,1);
    lcd.print(currentmonth);
    } else {
      lcd.setCursor(5,1);
      lcd.print(currentmonth);
    }

  lcd.setCursor(7,1);
  lcd.print("-");

  //years
  lcd.setCursor(8,1);
  lcd.print(currentyear);
  
  //----------------------
  lcd.setCursor(15,3);
  lcd.print("*SAVE");

  //cursor position under digits
  byte cursor_position;
  
  //variables for bellow while fncs.
  boolean saved = false;
  boolean setdate = false;
  byte knob2value = 0;

  // WHILE LOOP UNTIL *SAVE will selected/pressed
  while (!saved) {    //while loop 1

    //time digit (number) settime depend on cursor position
    cursor_position = (knob2.read() / 4);
    
    // ***********************************************************
    // DAYS set function 
    if (cursor_position == 2) {   //position under first number
      
      lcd.setCursor(5,2);
      lcd.print(" ");
      lcd.setCursor(8,2);
      lcd.print(" ");
      lcd.setCursor(14,3);
      lcd.print(" ");
      
      lcd.setCursor(2,2);
      lcd.write(byte(5));   //roof unslectted pointer under first digit

      //pressed-button - now going to change "days"
      if (digitalRead(pin_KB2) != 1 && (millis() - KB2_debounce) >= 500) {  
      
        KB2_debounce = millis();

        setdate = true;
        
        //keep current day to be adjusted down in next while
        knob2value = currentday;
        knob2.write(knob2value * 4);
        } 

        //setting loop fnc.
        while (setdate) {    //first digit selected for a change

          lcd.setCursor(2,2);
          lcd.write(byte(6));   //number (for DAY) is now selected

          knob2value = (knob2.read() / 4);

          // range (0-31)
          if (knob2value > 31) {
            knob2value = 0;
            knob2.write(0);
            } 

          // value postion shifting
          if (knob2value < 10) {
            lcd.setCursor(2,1);  
            lcd.print("0");
            lcd.setCursor(3,1);
            lcd.print(knob2value);
            } else {
              lcd.setCursor(2,1);
              lcd.print(knob2value);
              }

        //if KB2-button is pressed, this loop is canceld and value is set but not yet saved
        if (digitalRead(pin_KB2) != 1 && (millis() - KB2_debounce) >= 500) {
      
            KB2_debounce = millis();

            //control variable for this while loop 
            setdate = false;        

            //passing value to the date variable
            currentday = knob2value;

            //put cursor back on prev. position
            knob2.write(cursor_position * 4);
          }
        } //while
    } // days


    // ***********************************************************
    // MONTHS set function 
    else if (cursor_position == 3) {   //position under the months number
      
      lcd.setCursor(2,2);
      lcd.print(" ");
      lcd.setCursor(8,2);
      lcd.print(" ");
      lcd.setCursor(14,3);
      lcd.print(" ");
      
      lcd.setCursor(5,2);
      lcd.write(byte(5));   //"roof" pointer under secound digit

      //pressed-button - now going to change "months"
      if (digitalRead(pin_KB2) != 1 && (millis() - KB2_debounce) >= 500) {  
      
        KB2_debounce = millis();

        //jump to while loop
        setdate = true;
        
        //keep current month to be adjusted down in next while
        knob2value = currentmonth;
        knob2.write(knob2value * 4);
        } 

        //setting loop fnc.
        while (setdate) {    //first digit selected for a change

          lcd.setCursor(5,2);
          lcd.write(byte(6));   //number (for MONTH) is now selected

          knob2value = (knob2.read() / 4);

          // range (0-12)
          if (knob2value > 12) {
            knob2value = 0;
            knob2.write(0);
            } 

          // value postion shifting
          if (knob2value < 10) {
            lcd.setCursor(5,1);  
            lcd.print("0");
            lcd.setCursor(6,1);
            lcd.print(knob2value);
            } else {
              lcd.setCursor(5,1);
              lcd.print(knob2value);
              }

        //if KB2-button is pressed, this loop is canceld and value is set but not yet saved
        if (digitalRead(pin_KB2) != 1 && (millis() - KB2_debounce) >= 500) {
      
            KB2_debounce = millis();

            //control variable for this while loop 
            setdate = false;        

            //passing value to the date variable
            currentmonth = knob2value;

            //put cursor back on prev. position
            knob2.write(cursor_position * 4);
          }
        } //while
    } //months
      
    
    // ***********************************************************
    // YEARS set function 
    else if ( cursor_position == 4) {   //position under the year number

      lcd.setCursor(2,2);
      lcd.print(" ");
      lcd.setCursor(5,2);
      lcd.print(" ");
      lcd.setCursor(14,3);
      lcd.print(" ");
            
      lcd.setCursor(8,2);
      lcd.write(byte(5));   //"roof" unselected pointer under third digit

      //pressed-button - now going to change "years"
      if (digitalRead(pin_KB2) != 1 && (millis() - KB2_debounce) >= 500) {  
      
        KB2_debounce = millis();

        setdate = true;
        
        //keep current year to be adjusted down in next while
        knob2value = (byte)(currentyear - 2000);
        knob2.write(knob2value * 4);
        } 

        //setting loop fnc.
        while (setdate) {    //last digit selected for a change

          lcd.setCursor(8,2);
          lcd.write(byte(6));   //number (for YEAR) is now selected

          knob2value = (knob2.read() / 4);

          // range (0 - 60)
          if (knob2value > 60) {
            knob2value = 0;
            knob2.write(0);
            } 

          // value postion shifting
          if (knob2value < 10) {
            lcd.setCursor(8,1);  
            lcd.print("200");
            lcd.setCursor(11,1);
            lcd.print(knob2value);
            } else {
              lcd.setCursor(8,1);
              lcd.print("20");
              lcd.setCursor(10,1);
              lcd.print(knob2value);
              }

        //if KB2-button is pressed, this loop is canceld and value is set but not yet saved
        if (digitalRead(pin_KB2) != 1 && (millis() - KB2_debounce) >= 500) {
      
            KB2_debounce = millis();

            //control variable for this while loop 
            setdate = false;        

            //passing value to date variable
            currentyear = (int)knob2value + 2000;

            //put cursor back on prev. position
            knob2.write(cursor_position * 4);
          }
        } //while
    } //year
      
     // if 4
    else if ( cursor_position == 5) {   //position under third number

      lcd.setCursor(2,2);
      lcd.print(" ");
      lcd.setCursor(5,2);
      lcd.print(" ");
      lcd.setCursor(8,2);
      lcd.print(" ");
      
      lcd.setCursor(14,3);
      lcd.print(">");   // pointer in front of *SAVE

      //pressed-button - now going to write them all
      if (digitalRead(pin_KB2) != 1 && (millis() - KB2_debounce) >= 500) {  
      
        KB2_debounce = millis();
        
        lcd.setCursor(14,3);
        lcd.write(byte(0));

        // Manual clock&date set (YYYY, MM, DD, HH, II, SS)
        clock.setDateTime(currentyear, currentmonth, currentday, dt.hour, dt.minute, dt.second);

          lcd.clear();
          lcd.setCursor(3,2);
          lcd.print("<DATE, SAVED>");
        
        knob2.write(0);
        delay(1000);

          lcd.clear();
          
        // cancel of date-change loop
        saved = true;
        }
    } 
    else {
      if (cursor_position >= 6) {
        knob2.write(8);
        cursor_position = 2;        
        }
      if (cursor_position <= 1) {
        knob2.write(24);
        cursor_position = 4;        
        }
      } //else
  
  } // SAVE while loop
  
} // void set-date

