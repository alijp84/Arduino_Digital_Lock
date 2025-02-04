#include <Keypad.h>
#include <LiquidCrystal.h>
#include <Servo.h>

// تنظیمات LCD
LiquidCrystal lcd(12, 11, 10, A0, A1, A2); // RS, E, D4, D5, D6, D7

// تنظیمات Keypad
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; // پین‌های سطر
byte colPins[COLS] = {5, 4, 3};    // پین‌های مربوط به ستون ها
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// تنظیمات سروو
Servo myServo;
const int servoPin = 13; // اتصال پین وسطی سروو به پین 13 آردویینو
int servoOpenAngle = 90; //  زاویه باز شدن سروو
int servoCloseAngle = 0; // زاویه بسته شدن سروو
bool isLocked = true;

// متغیرهای مدیریت رمز
String defaultPassword = "0000";
String inputPassword = "";
bool isAuthenticated = false;

void setup() {
  lcd.begin(16, 2);
  // این جا فرایند حدس رمز باید شروع شود
  lcd.print("Enter Password:");
  myServo.attach(servoPin);
  myServo.write(servoCloseAngle); // شروع در حالت قفل
}

void loop() {
  char key = keypad.getKey();//رمز را هر بار از کاربر میگیرد

  if (key) {
    if (isAuthenticated) {
      handleAuthenticated(key);
    } else {
      handleAuthentication(key);
    }
  }
}

void handleAuthentication(char key) {
  //  کاربر اگر دکمه را فشار دهد وضعیت کلید را بررسی میکند (#)
  if (key == '#') {
    checkPassword();
  } else if (key == '*') {
    // اینم برای پاک کردن رمز ورودی
    inputPassword = "";
    lcd.setCursor(0, 1);
    lcd.clear();
    lcd.print("Cleared          ");
    lcd.clear();
    lcd.print("Enter Password:");
  } else {
    inputPassword += key;
    lcd.setCursor(0, 1);
    lcd.print(inputPassword);
  }
}

void checkPassword() {
  if (inputPassword == defaultPassword) {
    isAuthenticated = true;
    lcd.clear();
    lcd.print("Access Granted");
    myServo.write(servoOpenAngle); // باز کردن قفل
    isLocked = false;
    delay(2000);
    lcd.clear();
    lcd.print("1: Change Pass");//دو نوع گزینه نمایش داده میشود یک برای تغییر رمز lcdاینجا در 
    lcd.setCursor(0, 1);
    lcd.print("2: Exit          ");// دوم برای انصراف  
  } else {
    lcd.clear();
    lcd.print("Access Denied");
    delay(2000);
    lcd.clear();
    lcd.print("Enter Password:");
    inputPassword = "";
  }
}

void handleAuthenticated(char key) {
  if (key == '1') {// رفتن به وضعیت تغییر رمز 
    changePassword();
  } else if (key == '2') {
    logout();
    myServo.write(servoCloseAngle); // بستن قفل هنگام خروج
    isLocked = true;
  }
}

void changePassword() {
  lcd.clear();
  lcd.print("New Password:");
  inputPassword = "";
  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key == '#') {
        // بعد از باز کردن قفل بررسی کند که نباید کاربر همان رمز را دوباره بدهد
        if (defaultPassword == inputPassword){
          lcd.clear();
          lcd.print("Same Password!");
          lcd.clear();
          delay(1000);
          lcd.print("New Password:");
          inputPassword = "";
        }
        else if (inputPassword.length() == 4&& inputPassword !=defaultPassword) {
          defaultPassword = inputPassword;
          lcd.clear();
          lcd.print("Password Saved");//برای اینکه بعد از رمز تغییر کرد باید رمز را ذخیره کند
          delay(2000);
          logout();
          myServo.write(servoCloseAngle); // بستن قفل بعد از تغییر رمز
          isLocked = true;
          return;
        } else {// این شرط نشان میدهد که رمز قفل فقط 4 رقمی میتواند باشد.
          lcd.clear();
          lcd.print("4 digits only!");
          delay(2000);
          lcd.clear();
          lcd.print("New Password:");
          inputPassword = "";
        }
        // برای حذف رمز وارد شده
      } else if (key == '*') {
        inputPassword = "";
        lcd.setCursor(0, 1);
        lcd.print("password cleared.");
        delay(2000);
        lcd.clear();
        lcd.print("Enter Password:");
      } else {
        inputPassword += key;
        lcd.setCursor(0, 1);
        lcd.print(inputPassword);
      }
    }
  }
}
// برای بازگشت به وضعیت اولیه است
void logout() {
  isAuthenticated = false;
  inputPassword = "";
  lcd.clear();
  lcd.print("Enter Password:");
}