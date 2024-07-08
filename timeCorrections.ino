#define F_CPU 8000000UL
#define TIMER_INTERVAL 2UL
#define ONE_SECOND_MS 1000 / TIMER_INTERVAL

volatile unsigned long timerMillis = 0;

unsigned long realMillis() {
  unsigned long millisCopy;

  // Temporarily disable interrupts to ensure consistent value
  cli();
  millisCopy = timerMillis;
  sei();

  return millisCopy;
}

void realDelay(unsigned long millis) {
  delay(millis);
}

unsigned long getOneSecondMs() {
  return ONE_SECOND_MS;
}

ISR(TIMER0_COMPA_vect) {
  timerMillis += TIMER_INTERVAL;
}

void setupTimer() {
  // Set up Timer0 for CTC mode
  TCCR0A |= (1 << WGM01);  // CTC mode
  TCCR0B |= (1 << CS01) | (1 << CS00);  // Prescaler 64

  // Calculate and set OCR0A for 4 ms interrupt
  OCR0A = (F_CPU / 64 / 1000 * TIMER_INTERVAL) - 1;

  // Enable Timer0 compare interrupt
  TIMSK |= (1 << OCIE0A);

  // Enable global interrupts
  sei();
}