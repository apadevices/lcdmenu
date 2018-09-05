
void MENU_L1() {
  lcd.setCursor(0,0);
  lcd.print("Ta XX.XX");
  lcd.setCursor(9,0);
  lcd.print("*");
  lcd.setCursor(11,0);
  lcd.print("pH  XX.XX");
  //------------------
  lcd.setCursor(0,1);
  lcd.print("Tp XX.XX");
  lcd.setCursor(9,1);
  lcd.print("*");
  lcd.setCursor(11,1);
  lcd.print("RX  XX.XX");
  //------------------
  lcd.setCursor(0,2);
  lcd.print("Ts XX.XX");
  lcd.setCursor(9,2);
  lcd.print("*");
  lcd.setCursor(11,2);
  lcd.print("BAR XX.XX");  
  //------------------
  lcd.setCursor(0,3);
  lcd.print("Tr XX.XX");
  lcd.setCursor(9,3);
  lcd.print("*");
  lcd.setCursor(11,3);
  lcd.print("H%D XX.XX"); 
   
} //MENU_L1
