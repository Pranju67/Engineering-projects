```
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 16, 2);  // Initialize with I2C address and screen size

unsigned int f;
unsigned int h;

void setup() {
  Serial.begin(9600);

  // Initialize LCD with 16 columns and 2 rows
  lcd.begin(16, 2);  // Correctly initialize LCD
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("  Welcome To");
  lcd.setCursor(0, 1);
  lcd.print("JustDoElectronic");
  lcd.clear();
  delay(3000); // Delay for 3 seconds
}

void loop() {
  f = analogRead(1);
  h = analogRead(2);

  // Check if f is less than 75
  if (f < 75) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  Plz Give Me");
    lcd.setCursor(0, 1);
    lcd.print("     Water   ");
    delay(3000); // Wait for 3 seconds
    lcd.clear();
  }
  // Check if h is equal to 83
  else if (h == 83) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  I Want to ");
    lcd.setCursor(0, 1);
    lcd.print("     Sleep   ");
    delay(3000); // Wait for 3 seconds
    lcd.clear();
  }

  delay(200); // Small delay to avoid rapid loop
}

```