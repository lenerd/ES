#include <Servo.h>

/* pins */
const uint8_t vref_pin = A1;
const uint8_t az_pin = 2;
const uint8_t z45out_pin = A3;
const uint8_t servo_pin = 9;
const uint8_t led_pin = 13;

Servo servo;
uint8_t angle = 90;
const uint8_t min_angle = 25;
const uint8_t max_angle = 155;

/* values in mv */
double vref_mv;
double z_mv;

double z_val = 0;
double z_cal = 0;

uint8_t az_cnt = 0;

const uint8_t buf_len = 1;
double zval_buf[buf_len];
uint8_t buf_i = 0;

bool led_on = false;
uint8_t led_cnt = 0;


// void z_measure (void)
// {
//     vref_mv = analogRead(vref_pin) * 5000.0/1024.0;
//     z_mv = analogRead(z45out_pin) * 5000.0/1024.0;
//     z_val = ((z_mv - vref_mv) / 9.1 - z_cal) / 10;
//     zval_buf[buf_i] = z_val;
//     buf_i = (buf_i + 1) % buf_len;
// }
// 
// double z_current (void)
// {
//     double val = 0.0;
//     for (int i = 0; i < buf_len; ++i)
//         val += zval_buf[i];
//     val /= buf_len;
//     return val;
// }


void TC7_Handler (void)
{ 
    uint32_t status;
    status = TC2->TC_CHANNEL[1].TC_SR;

    if (led_cnt == 0)
        return;

    led_on = !led_on;
    digitalWrite(led_pin, led_on);

    if (!led_on)
        --led_cnt;
}


void TC8_Handler (void)
{ 
    uint32_t status;
    status = TC2->TC_CHANNEL[2].TC_SR;

    // z_measure();
    // z_val = z_current();
    vref_mv = analogRead(vref_pin) * 5000.0/1024.0;
    z_mv = analogRead(z45out_pin) * 5000.0/1024.0;
    z_val = ((z_mv - vref_mv) / 9.1 - z_cal) / 40; /* Faktor ? */

    
    // if (abs(z_val) < 1.5)
    // {
    //     z_val = 0;
    //     return;
    // }
    angle = angle + z_val;
    if (angle < 25)
    {
        led_cnt = 3;
        angle = 25;
    }
    else if (angle > 155)
    {
        led_cnt = 3;
        angle = 155;
    }
    servo.write(angle);
}
void setup() {
    uint32_t cmr7_val = 0;
    uint32_t cmr8_val = 0;
    pmc_set_writeprotect(false);
    pmc_enable_periph_clk(ID_TC7);
    pmc_enable_periph_clk(ID_TC8);
    
    /* configure TC7 */
    cmr7_val |= TC_CMR_WAVE;         /* waveform mode */
    cmr7_val |= TC_CMR_WAVSEL_UP_RC; /* counting up until eq RC */
    cmr7_val |= TC_CMR_TCCLKS_TIMER_CLOCK3; /* MCK/32 */
    TC2->TC_CHANNEL[1].TC_RC = 262500; /* set RC val */
    TC_Configure(TC2, 1, cmr7_val);
    TC2->TC_CHANNEL[1].TC_IER = TC_IER_CPCS; /* enable rc cmp interrupt */
    TC2->TC_CHANNEL[1].TC_IDR = ~TC_IER_CPCS; /* not disable rc cmp interrupt */

    /* configure TC8 */
    cmr8_val |= TC_CMR_WAVE;         /* waveform mode */
    cmr8_val |= TC_CMR_WAVSEL_UP_RC; /* counting up until eq RC */
    cmr8_val |= TC_CMR_TCCLKS_TIMER_CLOCK3; /* MCK/32 */
    TC2->TC_CHANNEL[2].TC_RC = 131250; /* set RC val */
    TC_Configure(TC2, 2, cmr8_val);
    TC2->TC_CHANNEL[2].TC_IER = TC_IER_CPCS; /* enable rc cmp interrupt */
    TC2->TC_CHANNEL[2].TC_IDR = ~TC_IER_CPCS; /* not disable rc cmp interrupt */

    /* enable interrupts */
    NVIC_ClearPendingIRQ(TC7_IRQn);
    NVIC_EnableIRQ(TC7_IRQn);
    NVIC_ClearPendingIRQ(TC8_IRQn);
    NVIC_EnableIRQ(TC8_IRQn);

    /* start timer */
    TC_Start(TC2, 1);
    TC_Start(TC2, 2);

    /* configure i/o pins */
    pinMode(vref_pin, INPUT);
    pinMode(az_pin, OUTPUT);
    pinMode(z45out_pin, OUTPUT);
    pinMode(led_pin, OUTPUT);

    digitalWrite(az_pin, LOW);
    digitalWrite(led_pin, LOW);

    delay(6);
    vref_mv = analogRead(vref_pin) * 5000.0/1024.0;
    z_mv = analogRead(z45out_pin) * 5000.0/1024.0;
    z_cal = (z_mv - vref_mv) / 9.1;

    /* configure servo */
    pinMode(servo_pin, OUTPUT);
    servo.attach(servo_pin);
    servo.write(90);

    /* init ringbuffer */
    // for (uint8_t i = 0; i < buf_len; ++i)
    //     z_measure();

    Serial.begin(9600);
    Serial.println("setup done");
}

void loop() {
    delay(200);
    // if (az_cnt < 10)
    // {
    //     ++az_cnt;
    // }
    // else
    // {
    //     digitalWrite(az_pin, HIGH);
    //     delay(1);
    //     digitalWrite(az_pin, LOW);
    //     delay(7);
    //     az_cnt = 0;
    // }
    Serial.print("vref: ");
    Serial.print(vref_mv);
    Serial.print("  z: ");
    Serial.print(z_val);
    Serial.print("  angle: ");
    Serial.print(angle);
    Serial.print('\n');
}
