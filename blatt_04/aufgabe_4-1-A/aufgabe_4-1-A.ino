/* pins */
const uint8_t t1_pin = 3;
const uint8_t led_pin = 13;

volatile uint8_t t1_cnt = 0;

volatile bool scomm = false;

/* messages */
const uint8_t msg_start = 0x1;
const uint8_t msg_max = 0x2;
const uint8_t msg_end = 0x4;

/* led blink? */
volatile bool led_on = false;
volatile uint8_t led_cnt = 0;


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

    if (digitalRead(t1_pin) == LOW)
    {
        ++t1_cnt;
        if (t1_cnt >= 100)
        {
            scomm = true;
            t1_cnt = 0;
        }
    }
    else
    {
        t1_cnt = 0;
    }
}


void setup (void) {
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
    TC2->TC_CHANNEL[2].TC_RC = 2625; /* set RC val */
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
    pinMode(t1_pin, INPUT);
    pinMode(led_pin, OUTPUT);


    Serial.begin(9600);
    Serial.println("setup done");
    Serial1.begin(9600);
}


void loop (void)
{
    uint8_t bite;

    if (scomm)
    {
        Serial.println("pressed button");
        Serial1.write(msg_start);
        Serial.println("sent msg_start");
        while ((bite = Serial1.read()) != msg_max);
        Serial.println("received msg_max");
        led_cnt = 3;
        while ((bite = Serial1.read()) != msg_end);
        Serial.println("received msg_end");
        led_cnt += 3;
        scomm = false;
        Serial.println("waiting for button");
    }
}
