#include <avr/io.h>
#include <avr/interrupt.h>
// #include <TinyWireM.h>
#include <Tiny4kOLED.h>

#define BUTTON_PIN PB3
#define BUZZER_PIN PB1
#define LED_PIN PB4

#define ATTINY_MHZ 8
#define ONE_SECOND_MS 1000 / (16 / ATTINY_MHZ)

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
  pinMode(BUTTON_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // TinyWireM.begin();
  oled.begin(128, 32, sizeof(tiny4koled_init_128x32br), tiny4koled_init_128x32br);
  oled.setFont(FONT8X16);
  oled.clear();
  oled.off();

  // Enable pin change interrupt on PB3
  GIMSK |= _BV(PCIE);
  PCMSK |= _BV(PCINT3);
  sei(); // Enable global interrupts
}

void loop() {
  buttonLoop();
  
  if (currentState == RUNNING_25 || currentState == RUNNING_5) {
    if (true || displayOn) {
      displayTimeRemaining();
    }

    if (millis() + 1000 - timerStart > ((currentState == RUNNING_25) ? interval25 : interval5)) {
      currentState = (currentState == RUNNING_25) ? ALARM_25 : ALARM_5;
      triggerAlarm();
    } else if (millis() - lastButtonPress > displayTimeout) {
      oled.off();
      displayOn = false;
    }
  }

}

void debugLed() {
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);
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
  oled.clear();
  oled.setCursor(0, 2);
  oled.print(minutes);
  oled.print(":");
  if (seconds < 10) oled.print("0");
  oled.print(seconds);
  oled.on();
}

void triggerAlarm() {
  for (int i = 0; i < 3; i++) {
    displayTime(0, 0);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);
    oled.off();
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

