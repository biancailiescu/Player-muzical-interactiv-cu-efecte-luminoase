#include <LiquidCrystal_I2C.h> 
LiquidCrystal_I2C lcd(0x27, 16, 2); 

void printLCDMultiline(String text) {
  lcd.clear();
  String line1 = text.substring(0, 16);
  String line2 = "";

  if (text.length() > 16) {
    line2 = text.substring(16, 32);
  }

  lcd.setCursor(0, 0);
  lcd.print(line1);

  lcd.setCursor(0, 1);
  lcd.print(line2);
}
void setup()
{
  Serial.begin(9600);

  DDRD &= ~((1 << PD2) | (1 << PD3) | (1 << PD4));

  PORTD |= (1 << PD2) | (1 << PD3) | (1 << PD4);

  lcd.init();     
  lcd.backlight(); 
}

void loop()
{

  if (!(PIND & (1 << PD2))) {
    Serial.println("Buton pentru redarea melodiei");
    printLCDMultiline("Redarea melodiei");
  }
  else if (!(PIND & (1 << PD3))) {
    Serial.println("Buton pentru selectare melodiei");
    printLCDMultiline("Selectarea melodiei");
  }
  else if (!(PIND & (1 << PD4))) {
    Serial.println("Buton pentru selectarea modului de iluminare");
    printLCDMultiline("Selectarea modului de iluminare");
  }

  delay(100);
}
