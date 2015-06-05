#include <Servo.h>

/* pins */
const uint8_t servo_pin = 9;

Servo servo;
uint32_t angle = 90;
const uint8_t min_angle = 25;
const uint8_t max_angle = 155;

int8_t bite;
uint8_t state;


void setup()
{
    /* configure servo */
    pinMode(servo_pin, OUTPUT);
    servo.attach(servo_pin);
    servo.write(90);

    Serial.begin(9600);
    Serial.println("setup done");
    angle = 0;
}

void loop()
{
    while ((bite = Serial.read()) != -1)
    {
        switch(state)
        {
            case 0:
                angle = 0;
                if (bite == 'm')
                    state = 1;
                else
                {
                    state = 0;
                    Serial.println(" 0: ^moveTo(\\d{1,3})$");
                }
                break;

            case 1:
                if (bite == 'o')
                    state = 2;
                else
                {
                    state = 0;
                    Serial.println(" 1: ^moveTo(\\d{1,3})$");
                }
                break;

            case 2:
                if (bite == 'v')
                    state = 3;
                else
                {
                    state = 0;
                    Serial.println(" 2: ^moveTo(\\d{1,3})$");
                }
                break;

            case 3:
                if (bite == 'e')
                    state = 4;
                else
                {
                    state = 0;
                    Serial.println(" 3: ^moveTo(\\d{1,3})$");
                }
                break;

            case 4:
                if (bite == 'T')
                    state = 5;
                else
                {
                    state = 0;
                    Serial.println(" 4: ^moveTo(\\d{1,3})$");
                }
                break;

            case 5:
                if (bite == 'o')
                    state = 6;
                else
                {
                    state = 0;
                    Serial.println(" 5: ^moveTo(\\d{1,3})$");
                }
                break;

            case 6:
                if (bite == '(')
                    state = 7;
                else
                {
                    state = 0;
                    Serial.println(" 6: ^moveTo(\\d{1,3})$");
                }
                break;

            case 7:
                if (bite >= '0' && bite <= '9')
                {
                    angle = bite - '0';
                    state = 8;
                }
                else
                {
                    state = 0;
                    Serial.print(bite);
                    Serial.print(" ");
                    Serial.println(" 7: ^moveTo(\\d{1,3})$");
                }
                break;

            case 8:
                if (bite >= '0' && bite <= '9')
                {
                    angle = angle * 10 + bite - '0';
                    state = 9;
                }
                else if (bite == ')')
                {
                    if (angle >= 25 && angle <= 155)
                    {
                        servo.write(angle);
                        Serial.print(angle);
                        Serial.println(" °");
                    }
                    else
                        Serial.println("use values between 25 and 155");
                    state = 0;
                }
                else
                {
                    state = 0;
                    Serial.println(" 8: ^moveTo(\\d{1,3})$");
                }
                break;

            case 9:
                if (bite >= '0' && bite <= '9')
                {
                    angle = angle * 10 + bite - '0';
                    state = 10;
                }
                else if (bite == ')')
                {
                    if (angle >= 25 && angle <= 155)
                    {
                        servo.write(angle);
                        Serial.print(angle);
                        Serial.println(" °");
                    }
                    else
                        Serial.println("use values between 25 and 155");
                    state = 0;
                }
                else
                {
                    state = 0;
                    Serial.println(" 9: ^moveTo(\\d{1,3})$");
                }
                break;

            case 10:
                if (bite == ')')
                {
                    if (angle >= 25 && angle <= 155)
                    {
                        servo.write(angle);
                        Serial.print(angle);
                        Serial.println(" °");
                    }
                    else
                        Serial.println("use values between 25 and 155");
                    state = 0;
                }
                else
                {
                    state = 0;
                    Serial.println("10: ^moveTo(\\d{1,3})$");
                }
                break;

            default:
                break;
        }
        Serial.println(state);
    }
}
