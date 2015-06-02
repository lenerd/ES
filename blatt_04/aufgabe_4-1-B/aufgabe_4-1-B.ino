/* pins */
const uint8_t led_pin = 13;

/* messages */
const uint8_t msg_start = 0x1;
const uint8_t msg_max = 0x2;
const uint8_t msg_end = 0x4;

/* led blink? */
volatile bool led_fade = false;
volatile bool led_max = false;
volatile bool led_min = false;
volatile bool led_inc = true;
volatile uint8_t led_val = 0;


void TC7_Handler (void)
{ 
    uint32_t status;
    status = TC2->TC_CHANNEL[1].TC_SR;

    if (!led_fade)
        return;

    if (led_inc)
    {
        led_val += 1;
        analogWrite(led_pin, led_val);
        if (led_val == 255)
        {
            led_max = true;
            led_inc = false;
        }
    }
    else
    {
        led_val -= 1;
        analogWrite(led_pin, led_val);
        if (led_val == 0)
        {
            led_min = true;
            led_inc = true;
            led_fade = false;
        }
    }
}


void setup (void) {
    uint32_t cmr7_val = 0;
    pmc_set_writeprotect(false);
    pmc_enable_periph_clk(ID_TC7);
    
    /* configure TC7 */
    cmr7_val |= TC_CMR_WAVE;         /* waveform mode */
    cmr7_val |= TC_CMR_WAVSEL_UP_RC; /* counting up until eq RC */
    cmr7_val |= TC_CMR_TCCLKS_TIMER_CLOCK3; /* MCK/32 */
    TC2->TC_CHANNEL[1].TC_RC = 51470; /* set RC val */
    TC_Configure(TC2, 1, cmr7_val);
    TC2->TC_CHANNEL[1].TC_IER = TC_IER_CPCS; /* enable rc cmp interrupt */
    TC2->TC_CHANNEL[1].TC_IDR = ~TC_IER_CPCS; /* not disable rc cmp interrupt */

    /* enable interrupts */
    NVIC_ClearPendingIRQ(TC7_IRQn);
    NVIC_EnableIRQ(TC7_IRQn);

    /* start timer */
    TC_Start(TC2, 1);

    /* configure i/o pins */
    pinMode(led_pin, OUTPUT);
    analogWrite(led_pin, 0);


    Serial.begin(9600);
    Serial.println("setup done");
    Serial1.begin(9600);
}


void loop (void)
{
    uint8_t bite;

    Serial.println("waiting for msg_start");
    while ((bite = Serial1.read()) != msg_start);
    Serial.println("received msg_start");
    led_fade = true;

    while (!led_max);
    Serial1.write(msg_max);
    Serial.println("sent msg_max");
    led_max = false;
    while (!led_min);
    Serial1.write(msg_end);
    Serial.println("sent msg_end");
    led_min = false;
}
