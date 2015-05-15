#include <Servo.h>

/* pins */
const uint8_t led_pin = 13;

bool led_on = false;
uint8_t state = 0;
int8_t bite;


void error_msg (void)
{
    Serial.println("^LED_O(N|FF)$");
}

void setup() {
    /* configure i/o pins */
    pinMode(led_pin, OUTPUT);

    digitalWrite(led_pin, LOW);

    Serial.begin(9600);
    Serial.println("setup done");
}

void loop (void)
{
    while ((bite = Serial.read()) != -1)
    {
        switch(state)
        {
            case 0:
                if (bite == 'L')
                    state = 1;
                else
                {
                    state = 0;
                    error_msg();
                }
                break;

            case 1:
                if (bite == 'E')
                    state = 2;
                else
                {
                    state = 0;
                    error_msg();
                }
                break;

            case 2:
                if (bite == 'D')
                    state = 3;
                else
                {
                    state = 0;
                    error_msg();
                }
                break;

            case 3:
                if (bite == '_')
                    state = 4;
                else
                {
                    state = 0;
                    error_msg();
                }
                break;

            case 4:
                if (bite == 'O')
                    state = 5;
                else
                {
                    state = 0;
                    error_msg();
                }
                break;

            case 5:
                if (bite == 'N')
                {
                    digitalWrite(led_pin, HIGH);
                    state = 0;
                }
                else if (bite == 'F')
                    state = 6;
                else
                {
                    state = 0;
                    error_msg();
                }
                break;

            case 6:
                if (bite == 'F')
                {
                    digitalWrite(led_pin, LOW);
                    state = 0;
                }
                else
                {
                    state = 0;
                    error_msg();
                }
                break;

            default:
                break;
        }
    }
}
