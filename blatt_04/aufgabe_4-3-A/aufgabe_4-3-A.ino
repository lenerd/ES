#include <Wire.h>
#include <Servo.h>

/* messages */
const uint8_t msg_on = 0x1;
const uint8_t msg_off = 0x2;

/* i2c addresses */
const uint8_t adr_slave = 0x02;

volatile bool scomm = false;
volatile uint8_t next = msg_on;

Servo servo;
const uint8_t servo_pin = 9;
uint8_t angle = 90;
const uint8_t min_angle = 25;
const uint8_t max_angle = 155;


void TC7_Handler (void)
{ 
    uint32_t status;
    status = TC2->TC_CHANNEL[1].TC_SR;

    if (next == msg_on)
        next = msg_off;
    else
        next = msg_on;
    scomm = true;
}


void setup (void) {

    /* start timer */
    TC_Start(TC2, 1);


    /* configure servo */
    pinMode(servo_pin, OUTPUT);
    servo.attach(servo_pin);
    servo.write(90);


    /* configure i/o pins */

    Serial.begin(9600);
    Serial.println("setup done");
    Wire.begin(); /* master */
    Serial.println("master");
}


void loop (void)
{
    int8_t bite;

    Serial.println(angle);
    Wire.requestFrom(adr_slave, (uint8_t) 1);
    bite = Wire.read();
    angle = angle + bite;
    if (angle < 25)
        angle = 25;
    else if (angle > 155)
        angle = 155;
    servo.write(angle);
}
