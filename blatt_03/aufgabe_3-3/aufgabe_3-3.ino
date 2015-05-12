#include <Servo.h>

/* pins */
const uint8_t vref_pin = A1;
const uint8_t az_pin = 2;
const uint8_t z45out_pin = A3;
const uint8_t servo_pin = 9;

Servo servo;
uint8_t angle = 90;
const uint8_t min_angle = 25;
const uint8_t max_angle = 155;

uint16_t vref_val = 0;
uint16_t z45out_val = 0;
double z_val = 0;
double z_cal = 0;

double zval_buf[10];
uint8_t buf_i = 0;


void z_measure (void)
{
    double vref_val, z45out_val, z_val;
    vref_val = 0x3ff & analogRead(vref_pin);
    z45out_val = 0x3ff & analogRead(z45out_pin);
    z_val = (z45out_val - vref_val) / 9.1 - z_cal;
    zval_buf[buf_i] = z_val;
    buf_i = (buf_i + 1) % 10;
}

double z_current (void)
{
    double val = 0.0;
    for (int i = 0; i < 10; ++i)
        val += zval_buf[i];
    val /= 10;
    return val;
}


void TC8_Handler (void)
{ 
    uint32_t status;
    status = TC2->TC_CHANNEL[2].TC_SR;

    z_measure();
    z_val = z_current();
    
    if (abs(z_val) < 1.5)
    {
        z_val = 0;
        return;
    }
    angle = angle + z_val;
    if (angle < 25)
    {
        // TODO: blink
        angle = 25;
    }
    else if (angle > 155)
    {
        // TODO: blink
        angle = 155;
    }
    servo.write(angle);
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
    pinMode(vref_pin, INPUT);
    pinMode(az_pin, OUTPUT);
    pinMode(z45out_pin, OUTPUT);

    digitalWrite(az_pin, LOW);

    vref_val = 0x3ff & analogRead(vref_pin);
    z45out_val = 0x3ff & analogRead(z45out_pin);
    delay(3);
    z_cal = (z45out_val - vref_val) / 9.1;

    /* configure servo */
    pinMode(servo_pin, OUTPUT);
    servo.attach(servo_pin);
    servo.write(90);

    /* init ringbuffer */
    for (uint8_t i = 0; i < 10; ++i)
        z_measure();

    Serial.begin(9600);
    Serial.println("setup done");
}

void loop() {
    delay(200);
    digitalWrite(az_pin, HIGH);
    delay(1);
    digitalWrite(az_pin, LOW);
    delay(7);
    Serial.print("vref: ");
    Serial.print(vref_val);
    Serial.print("  z: ");
    Serial.print(z_val);
    Serial.print("  angle: ");
    Serial.print(angle);
    Serial.print('\n');
}
