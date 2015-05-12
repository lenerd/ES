#include <Servo.h>

/* pins */
Servo servo;
const uint8_t servo_pin = 9;

const uint8_t min = 25;
const uint8_t max = 155;

volatile uint8_t angle = 90;
volatile uint8_t to_angle = 90;
volatile bool incr = true;
volatile uint8_t a = 0;
volatile bool cw = true;
volatile bool next = false;

void TC8_Handler (void)
{ 
    uint32_t status;
    status = TC2->TC_CHANNEL[2].TC_SR;

    if (angle < to_angle)
    {
        angle += 2;
        servo.write(angle);
        return;
    }
    else if (angle > to_angle)
    {
        angle -= 2;
        servo.write(angle);
        return;
    }

    if (incr)
    {
        if (cw)
        {
            a += 10;
            to_angle = 90 - a;
            // servo.write(angle);
            cw = false;
        }
        else
        {
            to_angle = 90 + a;
            // servo.write(angle);
            cw = true;
            if (a == 90)
            {
                incr = false;
            }
        }
    }
    else
    {
        if (cw)
        {
            a -= 10;
            to_angle = 90 - a;
            // servo.write(angle);
            cw = false;
        }
        else
        {
            to_angle = 90 + a;
            // servo.write(angle);
            cw = true;
            if (a == 90)
            {
                incr = true;
            }
        }
    }
}
void setup() {
    uint32_t cmr8_val = 0;
    pmc_set_writeprotect(false);
    pmc_enable_periph_clk(ID_TC8);

    /* configure TC8 */
    cmr8_val |= TC_CMR_WAVE;         /* waveform mode */
    cmr8_val |= TC_CMR_WAVSEL_UP_RC; /* counting up until eq RC */
    cmr8_val |= TC_CMR_TCCLKS_TIMER_CLOCK3; /* MCK/32 */
    TC2->TC_CHANNEL[2].TC_RC = 262500; /* set RC val */
    TC_Configure(TC2, 2, cmr8_val);
    TC2->TC_CHANNEL[2].TC_IER = TC_IER_CPCS; /* enable rc cmp interrupt */
    TC2->TC_CHANNEL[2].TC_IDR = ~TC_IER_CPCS; /* not disable rc cmp interrupt */

    /* enable interrupts */
    NVIC_ClearPendingIRQ(TC8_IRQn);
    NVIC_EnableIRQ(TC8_IRQn);

    /* start timer */
    TC_Start(TC2, 2);

    /* configure i/o pins */
    pinMode(servo_pin, OUTPUT);
    servo.attach(servo_pin);
    servo.write(90);

    Serial.begin(9600);
    Serial.println("setup done");
}

void loop() {
    delay(100);
    Serial.print(angle);
    Serial.print("  ");
    Serial.print(servo.read());
    Serial.print('\n');
}
