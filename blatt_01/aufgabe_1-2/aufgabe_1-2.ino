const int led_pin = 7;
const int t1_pin = 5;
bool led_state;

void setup() {
  pinMode(led_pin, OUTPUT);
  pinMode(t1_pin, INPUT);
  led_state = LOW;

}

void loop() {
  if (digitalRead(t1_pin) == LOW)
  {
    led_state = !led_state;
    digitalWrite(led_pin, led_state);
    delay(500);
  }
  
}
