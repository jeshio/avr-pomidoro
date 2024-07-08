volatile uint32_t debounce;
volatile bool buttonStateChanged = false;
volatile bool buttonState = LOW;
bool longPressHandled = false;

ISR(PCINT0_vect) {
  if (digitalRead(BUTTON_PIN) == LOW || millis() - debounce >= 5) {
    debounce = millis();

    buttonState = digitalRead(BUTTON_PIN);
    buttonStateChanged = true;
  }
}

void buttonLoop() {
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
  if (currentState == OFF) {
      resetTimer();
      currentState = RUNNING_25;
  }
  else if (currentState == RUNNING_25 || currentState == RUNNING_5) {
      displayOn = true;
  }
}

void onButtonLongPress() {
  if (currentState == RUNNING_25 || currentState == RUNNING_5) {
    currentState = OFF;
    oled.off();
  }
}
