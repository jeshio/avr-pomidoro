volatile uint32_t debounce;
volatile bool buttonStateChanged = false;
volatile bool buttonState = LOW;
bool longPressHandled = false;

ISR(PCINT0_vect) {
  if (digitalRead(BUTTON_PIN) == LOW || realMillis() - debounce >= 10) {
    debounce = realMillis();

    buttonState = digitalRead(BUTTON_PIN);
    buttonStateChanged = true;
  }
}

void setupButton() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Enable pin change interrupt on PB3
  GIMSK |= _BV(PCIE);
  PCMSK |= _BV(PCINT3);
  sei(); // Enable global interrupts
}

void buttonLoop() {
  if (buttonStateChanged) {
    buttonStateChanged = false;

    if (buttonState == LOW) {
      lastButtonPress = realMillis();
      longPressHandled = false;
    } else {
      lastButtonPress = realMillis();
      onButtonShortPress();
    }
  }
  // продолжается удержание
  else if (buttonState == HIGH && !longPressHandled) {
    if (realMillis() - lastButtonPress > 3000) {
      longPressHandled = true;
      onButtonLongPress();
    }
  }
}

void onButtonShortPress() {
  oled.on();
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
