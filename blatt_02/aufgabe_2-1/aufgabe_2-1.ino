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
    uint32_t cmr_val = 0;
    pmc_set_writeprotect(false);
    pmc_enable_periph_clk(ID_TC0);

    cmr_val |= TC_CMR_WAVE;         /* waveform mode */
    cmr_val |= TC_CMR_WAVSEL_UP_RC; /* counting up until eq RC */
    /* TODO: clock selection */
    /* TODO: set RC val */
    TC_Configure(TC0, 0, cmr_val);
    TC0->TC_CHANNEL[0].TC_IER = TC_IER_CPCS; /* enable rc cmp interrupt */
    /* TODO: isr */

    NVIC_ClearPendingIRQ(TC0_IRQn);
    NVIC_EnableIRQ(TC0_IRQn);

    TC_Start(TC0, 0);


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
