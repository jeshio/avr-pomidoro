#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>

#define BUTTON_PIN 2 // PB3
#define BUZZER_PIN 3 // PB1

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);

unsigned long counter = 0;

volatile uint32_t debounce;
volatile bool buttonStateChanged = false;
volatile bool buttonState = LOW;

unsigned long lastButtonPress = 0;
bool longPressHandled = false;

void interruptButtonHandler() {
  if (digitalRead(BUTTON_PIN) == LOW || millis() - debounce >= 5) {
    debounce = millis();

    buttonState = digitalRead(BUTTON_PIN);
    buttonStateChanged = true;
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  u8g2.begin();
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), interruptButtonHandler, CHANGE);
}

void loop() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_crox5hb_tr);
  u8g2.setCursor(0, 17);
  u8g2.println(counter);
  u8g2.sendBuffer();

  if (buttonStateChanged) {
    buttonStateChanged = false;

    if (buttonState == LOW) {
      lastButtonPress = millis();
      longPressHandled = false;
    } else {
      lastButtonPress = millis();
      onButtonShortPress();
    }
  }
  // продолжается удержание
  else if (buttonState == HIGH && !longPressHandled) {
    if (millis() - lastButtonPress > 1000) {
      longPressHandled = true;
      onButtonLongPress();
    }
  }
}

void onButtonShortPress() {
  counter += 1;
}

void onButtonLongPress() {
  counter += 10;
}

