const int led_pin = 7;
const int t1_pin = 5;
volatile int led_state = LOW;

/**
 * Nachteil: Polling, es wird Zeit verwendet, um den Status abzufragen.
 */

void setup() {
  pinMode(led_pin, OUTPUT);
  // pinMode(t1_pin, INPUT);
  //led_state = LOW;
  // digitalWrite(led_pin, led_state);
  // attachInterrupt(t1_pin, &t1_int, FALLING);
  attachInterrupt(t1_pin, &t1_int, FALLING);
}

void loop() {
    digitalWrite(led_pin, led_state);
}

void t1_int ()
{
    led_state = !led_state;
    // delayMicroseconds(200000);
}
