```
// 13  Buzzer
// 12 Door
// 2,3,4,5,6,7,8,9 Keypad

#include <Wire.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

#define Buzzer      13
#define DoorLock    12
#define DoorS       11

String input_password;
String password;
int keyCount = 0;
const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 4; //four columns
int OTPflag = 0;
int flag3 = 0;
int PassCode = 0;
String ID = "";
char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte pin_rows[ROW_NUM] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );
String numbers[10] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};

void setup()
{
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  pinMode(DoorS, INPUT);
  pinMode(DoorLock, OUTPUT);
  digitalWrite(DoorLock, HIGH);
  pinMode(Buzzer, OUTPUT);
  digitalWrite(Buzzer, HIGH);

  input_password.reserve(32); // maximum input characters is 33, change if needed

  lcd.setCursor(0, 0);
  lcd.print("  Smart Door Lock   ");
  lcd.setCursor(0, 1);
  lcd.print("        System      ");
  lcd.setCursor(0, 3);
  lcd.print("       Project by - ");
  delay(5000);
  lcd.clear();

  lcd.setCursor(0, 1);
  lcd.print(" Pranjal Bhanushali ");
  lcd.setCursor(0, 3);
  lcd.print("    Roll No. : 7    ");
  delay(5000);
  lcd.clear();
}

void keypress()
{
  digitalWrite(Buzzer, LOW);
  delay(50);
  digitalWrite(Buzzer, HIGH);
}

void correctPass()
{
  digitalWrite(Buzzer, LOW);
  delay(1000);
  digitalWrite(Buzzer, HIGH);
}

void GetPW()
{
  char key = keypad.getKey();
  if (key)
  {
    keyCount++;
    //Serial.println(key);
    lcd.setCursor((0 + keyCount), 1);
    lcd.print(key);
    lcd.setCursor((1 + keyCount), 1);
    //lcd.print("      ");
    keypress();

    if (key == '*')
    {
      OTPflag = 1;
      flag3 = 0;
      keyCount = 0;
      input_password = ""; // clear input password
    }
    else if (key == '#')
    {
      OTPflag = 1;
      if (password == input_password)
      {
        lcd.setCursor(0, 0);
        lcd.print("    OTP ACCEPTED    ");
        correctPass();
        digitalWrite(DoorLock, LOW);
        delay(5000);
        digitalWrite(DoorLock, HIGH);
        keypress();delay(50);keypress();delay(50);keypress();
        input_password = "";
        PassCode = 0;
        password = "";
        keyCount = 0;
        flag3 = 0;
        OTPflag = 0;
        delay(5000);
      }
      else
      {
        lcd.setCursor(0, 0);
        lcd.print("    OTP INCORRECT   ");
        keypress();delay(50);keypress();delay(50);keypress();        OTPflag = 0;
        delay(1000);
        input_password = ""; // clear input password
        PassCode = 0;
        keyCount = 0;
        flag3 = 0;
        delay(5000);
      }
      input_password = ""; // clear input password
    }
    else
    {
      input_password += key; // append new character to input password string
    }
  }
}

void GetPWC()
{
  char key = keypad.getKey();
  if (key)
  {

    lcd.setCursor(0, 3);
    lcd.print("Selected Code: ");
    lcd.print(key);
    lcd.print("    ");
    keypress();

    if (key == 'A')
    {
      delay(1000);
      ID = "@Pranjalbhanushali";
      PassCode = 1;
      key = "";
    }
    else if (key == 'B')
    {
      delay(1000);
      ID = "@Pranjalbhanushali";
      PassCode = 1;
      key = "";
    }
    else if (key == 'C')
    {
      delay(1000);
      ID = "@Pranjalbhanushali";
      PassCode = 1;
      key = "";
    }
    else if (key == 'D')
    {
      delay(1000);
      ID = "@Pranjalbhanushali";
      PassCode = 1;
      key = "";
    }
    else
    {
      lcd.setCursor(15, 3);
      lcd.print("     ");
      key = "";
    }
  }
}

void loop()
{
  lcd.setCursor(0, 0);
  lcd.print(" Please Select Code ");
  lcd.setCursor(0, 1);
  lcd.print("   A   B   C   D    ");
  lcd.setCursor(0, 2);
  lcd.print("                    ");
  lcd.setCursor(0, 3);
  lcd.print("                    ");
  correctPass();
  delay(3000);
  while (PassCode == 0)
  {
    int DoorS_In = digitalRead(DoorS);
    //Serial.println(DoorS_In);
    if(DoorS_In == HIGH)
    {
      keypress();delay(50);keypress();
      Serial.print("@Pranjalbhanushali"); Serial.print(","); Serial.println("CodeRED");
    }
    GetPWC();
  }
  lcd.clear();
  keypress();delay(50);keypress();
  lcd.setCursor(0, 0);
  lcd.print("   Sending OTP to   ");
  lcd.setCursor(0, 1);
  lcd.print("  Telegram Account  ");
  lcd.setCursor(0, 2);
  lcd.print(" :");lcd.print(ID);
  delay(3000);
  password = numbers[random(0, 10)] + numbers[random(0, 10)] + numbers[random(0, 10)] + numbers[random(0, 10)] + numbers[random(0, 10)] + numbers[random(0, 10)];
  Serial.print(ID); Serial.print(","); Serial.println(password);
  lcd.clear();
  lcd.setCursor(0, 2);
  lcd.print("      OTP Sent      ");
  correctPass();
  delay(3000);
  lcd.clear();
  OTPflag = 1;
  while (OTPflag == 1)
  {
    lcd.setCursor(0, 0);
    lcd.print("   : Enter  OTP :   ");
    lcd.setCursor(0, 3);
    lcd.print("  *:Clear  #:Enter  ");
    if (flag3 == 0)
    {
      lcd.setCursor(0, 1);
      lcd.print(":______");
      flag3 = 1;
    }
    GetPW();
  }
}
```