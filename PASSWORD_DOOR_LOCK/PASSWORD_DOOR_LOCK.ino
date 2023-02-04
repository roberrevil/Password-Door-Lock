#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Servo.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

const byte ROWS = 4; 
const byte COLS = 4; 
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {8, 9, 10, 11}; 
byte colPins[COLS] = {12, 13, 18, 19}; 
Keypad keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const int BUZZER_PIN = 14;

Servo servo;
const int SERVO_PIN = 15;

String password = "12345";
String input = "";
const int MAX_ATTEMPTS = 5;
int attempts = MAX_ATTEMPTS;
bool changingPassword = false;

void setup() {
  lcd.begin(16, 2);
  pinMode(BUZZER_PIN, OUTPUT);
  servo.attach(SERVO_PIN, 544, 2400);
  servo.write(0);
  printMessage("[0] CLOSE", "", true);
}

void loop() {
  char key = keypad.getKey();
  
  if (key) {
    if (changingPassword) {
      if (key == '*' || key == '#') {
        changingPassword = false;
        input = "";
        printMessage("[0] CLOSE", "", true);
        trimmer(100);
      } else {
        input += key;
        printMessage("[*] NEW PIN", input, true);
        trimmer(100);

        if (input.length() == password.length()) {
          changingPassword = false;
          password = input;
          input = "";
          attempts = MAX_ATTEMPTS;
          newPin();
        }
      }
    } else {
      if (key == '*') {
        changingPassword = true;
        input = "";
        printMessage("[*] NEW PIN", "", true);
        trimmer(100);
      } else if (key == '#') {
        changingPassword = false;
        input = "";
        printMessage("[0] CLOSE", "", true);
        trimmer(100);
      } else {
        input += key;
        printMessage("[0] CLOSE", input, true);
        trimmer(100);

        if (input.length() == password.length()) {
          if (input == password) {
            attempts = MAX_ATTEMPTS;
            open();
          } else {
            attempts--;

            if (attempts != 0) {
              error();
            } else {
              block();
            }
          }

          input = "";
        }
      }
    }
  }
}

void printMessage(String msg1, String msg2, bool blink) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(msg1);
  lcd.setCursor(0, 1);
  lcd.print(msg2);

  if (blink)
    lcd.blink();
  else
    lcd.noBlink();
}

void trimmer(int millis) {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(millis);
  digitalWrite(BUZZER_PIN, LOW);
}

void openDoor() {
  for (int i = 0; i < 180; i++) {
    servo.write(i);
    delay(15);
  }
}

void closeDoor() {
  for (int i = 180; 0 < i; i--) {
    servo.write(i);
    delay(15);
  }
}

void open() {
  openDoor();
  trimmer(1000);

  for (int i = 10; 0 < i; i--) {
    printMessage("[1] OPEN", "AUTO-CLOSE: ", false);
    lcd.print(i);
    delay(1000);
  }

  closeDoor();
  trimmer(1000);

  printMessage("[0] CLOSE", "", true);
}

void error() {
  trimmer(1000);

  printMessage("[X] ERROR", "ATTEMPTS: ", false);
  lcd.print(attempts);
  delay(2000);

  printMessage("[0] CLOSE", "", true);
}

void block() {
  trimmer(1000);

  while (true) {
    printMessage("[!] BLOCK", "ATTEMPTS: ", false);
    lcd.print(attempts);
    delay(5000);
  }
}

void newPin() {
  trimmer(1000);

  printMessage("[*] NEW PIN", "NEW PIN: "+password, false);
  delay(5000);

  printMessage("[0] CLOSE", "", true);
}
