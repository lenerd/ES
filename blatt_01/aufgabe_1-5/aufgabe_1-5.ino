const int led_pin = 7;
const int t1_pin = 5;
const int t2_pin = 3;

uint8_t brightness = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(led_pin, OUTPUT);
  pinMode(t1_pin, INPUT);
  pinMode(t2_pin, INPUT);
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(t1_pin) == LOW)
  {
    brightness = brightness + 5;
  }
  
  if (digitalRead(t2_pin) == LOW)
  {
    brightness = brightness - 5;
  }
  
  analogWrite(led_pin, brightness);
  Serial.println(brightness);
  
  delay(100);

}
