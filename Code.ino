#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#define TRIG_PIN 11
#define ECHO_PIN 10

RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myServo;

long duration;
int distance;

const int servoPin = 3;
const int ledMerah = 4;
const int buzzerPin = 7;
const int relayPin = 8;
const int waterLevelPin = A0;
int waterLevel = 0;
int servo = 0;

// Jadwal makan 
const int jadwalPagi = 8;
const int jadwalMenit1 = 00;
const int jadwalSiang = 14;
const int jadwalMenit2 = 45;
const int jadwalSore = 17;
const int jadwalMenit3 = 00;

void setup() {
  Wire.begin();
  lcd.begin(16, 2);
  lcd.backlight();

  pinMode(buzzerPin, OUTPUT);
  pinMode(ledMerah, OUTPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  myServo.attach(servoPin);
  myServo.write(0);
  digitalWrite(relayPin, LOW);
  Serial.begin(9600);

  if (!rtc.begin()) {
    lcd.print("RTC Tidak Ditemukan");
    while (1)
      ;
  }

  if (rtc.lostPower()) {
    lcd.print("Mengatur Waktu...");
  }
}

void loop() {
  DateTime now = rtc.now();
  myServo.write(145);
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2;

  waterLevel = analogRead(waterLevelPin);
  Serial.print("Nilai Water Level: ");
  Serial.println(waterLevel);
  Serial.print("Jarak: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Waktu
  lcd.setCursor(0, 0);
  lcd.print("Waktu: ");
  lcd.print(now.hour());
  lcd.print(':');
  if (now.minute() < 10) lcd.print('0');
  lcd.print(now.minute());
  lcd.print(':');
  if (now.second() < 10) lcd.print('0');
  lcd.print(now.second());

  // Jadwal Makan
  lcd.setCursor(0, 1);
  lcd.print(jadwalPagi);
  lcd.print(":00 ");
  lcd.print(jadwalSiang);
  lcd.print(":16 ");
  lcd.print(jadwalSore);
  lcd.print(":10 ");

  if ((now.hour() == jadwalPagi && now.minute() == jadwalMenit1) || 
      (now.hour() == jadwalSiang && now.minute() == jadwalMenit2) || 
      (now.hour() == jadwalSore && now.minute() == jadwalMenit3)) {

    if (distance < 10) {  
      if (servo == 0) {  
        for (int i = 0; i < 2; i++) {
          tone(buzzerPin, 800);
          myServo.write(145); 
          delay(1000);
          myServo.write(90);
          noTone(buzzerPin);
          delay(1000);
        }
        servo = 1;
        digitalWrite(ledMerah, LOW);
        tone(buzzerPin, 800);
        delay(1000);
        noTone(buzzerPin);
      }
    } else if (distance > 13) {  
      for (int i = 0; i < 10; i++) {
        digitalWrite(ledMerah, HIGH);
        delay(250);
        digitalWrite(ledMerah, LOW);
        delay(250);
        tone(buzzerPin, 500);
        delay(250);
        noTone(buzzerPin);
      }
    }

    if (waterLevel < 100) {
      lcd.setCursor(0, 1);
      lcd.print("Mengisi Air   ");
      digitalWrite(relayPin, LOW);
    } else if (waterLevel > 300) {
      digitalWrite(relayPin, HIGH);
    }

  } else {
    servo = 0;
    myServo.write(145);
    noTone(buzzerPin);
    digitalWrite(relayPin, HIGH);
  }
}
