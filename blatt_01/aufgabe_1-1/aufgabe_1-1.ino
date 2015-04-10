
const int led_pin = 7;

void setup() {
  pinMode(led_pin, OUTPUT);

}

void loop() {
  digitalWrite(led_pin, HIGH);
  delay(2000);
  digitalWrite(led_pin, LOW);
  delay(500);
}
