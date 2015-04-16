const int led_pin = 7;
const int t1_pin = 5;
const int t2_pin = 3;

uint8_t brightness = 0;

void t1_int ()
{
  brightness += 5;
}

void t2_int ()
{
  brightness -= 5;
}

void setup()
{
  pinMode(led_pin, OUTPUT);
  attachInterrupt(t1_pin, &t1_int, FALLING);
  attachInterrupt(t2_pin, &t2_int, FALLING);
  Serial.begin(9600);

}

void loop()
{
  analogWrite(led_pin, brightness);
  Serial.println(brightness);
  
  delay(100);

}
