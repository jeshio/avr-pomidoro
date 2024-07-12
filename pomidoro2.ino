#include <avr/io.h>
#include <avr/interrupt.h>
#include <Tiny4kOLED.h>

#define BUTTON_PIN PB3
#define BUZZER_PIN PB1

enum DeviceState { OFF, RUNNING_25, RUNNING_5, ALARM_25, ALARM_5 };
DeviceState currentState = OFF;
unsigned long timerStart;
unsigned long displayTimeout = 5000;
const unsigned long interval25 = 25UL * 60 * 1000;
const unsigned long interval5 = c5UL * 60 * 1000;
const unsigned long alarmDuration = 3000;
bool displayOn = false;

unsigned long lastButtonPress = 0;

void setup() {
  oled.begin(128, 32, sizeof(tiny4koled_init_128x32br), tiny4koled_init_128x32br);
  oled.setFont(FONT8X16);
  oled.clear();
  oled.off();

  setupButton();

  setupTimer();
}

void loop() {
  buttonLoop();
  
  if (currentState == RUNNING_25 || currentState == RUNNING_5) {
    if (true || displayOn) {
      displayTimeRemaining();
    }

    if (realMillis() + getOneSecondMs() - timerStart > ((currentState == RUNNING_25) ? interval25 : interval5)) {
      currentState = (currentState == RUNNING_25) ? ALARM_25 : ALARM_5;
      triggerAlarm();
    } else if (realMillis() - lastButtonPress > displayTimeout) {
      oled.off();
      displayOn = false;
    }
  }

}

void resetTimer() {
  oled.clear();
  timerStart = realMillis();
  lastButtonPress = realMillis();
}

void displayTimeRemaining() {
  unsigned long remaining = ((currentState == RUNNING_25) ? interval25 : interval5) - (realMillis() - timerStart);
  unsigned int minutes = remaining / 60000;
  unsigned int seconds = (remaining / 1000) % 60;
  displayTime(minutes, seconds);
}

void displayTime(unsigned int minutes, unsigned int seconds) {
  oled.setCursor(0, 2);
  oled.print(minutes);
  oled.print(":");
  if (seconds < 10) oled.print("0");
  oled.print(seconds);
}

void triggerAlarm() {
  for (int i = 0; i < 3; i++) {
    oled.on();
    displayTime(0, 0);
    digitalWrite(BUZZER_PIN, HIGH);
    realDelay(500);
    oled.off();
    digitalWrite(BUZZER_PIN, LOW);
    realDelay(500);
  }

  if (currentState == ALARM_25) {
    resetTimer();
    currentState = RUNNING_5;
  } else {
    resetTimer();
    currentState = RUNNING_25;
  }
}

