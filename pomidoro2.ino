#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>

#define BUTTON_PIN 2 // PB3
#define BUZZER_PIN 3 // PB1

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);

enum DeviceState { OFF, RUNNING_25, RUNNING_5, ALARM_25, ALARM_5 };
DeviceState currentState = OFF;
unsigned long timerStart;
unsigned long displayTimeout = 5000;
const unsigned long interval25 = 12000; // 25UL * 60 * 1000;
const unsigned long interval5 = 9000; // 5UL * 60 * 1000;
const unsigned long alarmDuration = 3000;
bool displayOn = false;

unsigned long lastButtonPress = 0;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  u8g2.begin();
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), interruptButtonHandler, CHANGE);
}

void loop() {
  if (currentState == RUNNING_25 || currentState == RUNNING_5) {
    if (true || displayOn) {
      displayTimeRemaining();
    }

    if (millis() + 1000 - timerStart > ((currentState == RUNNING_25) ? interval25 : interval5)) {
      currentState = (currentState == RUNNING_25) ? ALARM_25 : ALARM_5;
      triggerAlarm();
    } else if (millis() - lastButtonPress > displayTimeout) {
      u8g2.clearBuffer();
      u8g2.sendBuffer();
      displayOn = false;
    }
  }

  buttonLoop();
}

void resetTimer() {
  timerStart = millis();
  lastButtonPress = millis();
}

void displayTimeRemaining() {
  unsigned long remaining = ((currentState == RUNNING_25) ? interval25 : interval5) - (millis() - timerStart);
  unsigned int minutes = remaining / 60000;
  unsigned int seconds = (remaining / 1000) % 60;
  displayTime(minutes, seconds);
}

void displayTime(unsigned int minutes, unsigned int seconds) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_crox5hb_tr);
  u8g2.setCursor(0, 28);
  u8g2.print(minutes);
  u8g2.print(":");
  if (seconds < 10) u8g2.print("0");
  u8g2.print(seconds);
  u8g2.sendBuffer();
}

void triggerAlarm() {
  for (int i = 0; i < 3; i++) {
    displayTime(0, 0);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);
    u8g2.clearBuffer();
    u8g2.sendBuffer();
    digitalWrite(BUZZER_PIN, LOW);
    delay(500);
  }

  if (currentState == ALARM_25) {
    resetTimer();
    currentState = RUNNING_5;
  } else {
    resetTimer();
    currentState = RUNNING_25;
  }
}

