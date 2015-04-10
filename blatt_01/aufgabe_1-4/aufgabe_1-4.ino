const int led_pin = 7;
uint8_t brightness = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(led_pin, OUTPUT);
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(led_pin, brightness);
  Serial.println(brightness);
  brightness = brightness + 5;
  delay(100);

}
