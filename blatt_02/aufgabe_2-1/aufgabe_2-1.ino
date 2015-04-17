const int led_pin = 7;
const int t1_pin = 5;
const int t2_pin = 3;

volatile uint8_t t1_cnt = 0;
volatile uint8_t t2_cnt = 0;
volatile uint8_t brightness = 100;
volatile bool next_state = LOW;

void TC6_Handler (void)
{ 
    uint32_t status;
    status = TC2->TC_CHANNEL[0].TC_SR;

    if (digitalRead(t1_pin) == LOW)
    {
        ++t1_cnt;
        if (t1_cnt >= 100)
        {
            brightness += 5;
            t1_cnt = 0;
        }
    }
    else
    {
        t1_cnt = 0;
    }
    if (digitalRead(t2_pin) == LOW)
    {
        ++t2_cnt;
        if (t2_cnt >= 100)
        {
            brightness -= 5;
            t2_cnt = 0;
        }
    }
    else
    {
        t2_cnt = 0;
    }
}

void setup()
{

    uint32_t cmr_val = 0;
    pmc_set_writeprotect(false);
    pmc_enable_periph_clk(ID_TC6);

    cmr_val |= TC_CMR_WAVE;         /* waveform mode */
    cmr_val |= TC_CMR_WAVSEL_UP_RC; /* counting up until eq RC */
    cmr_val |= TC_CMR_TCCLKS_TIMER_CLOCK3; /* MCK/32 */
    TC2->TC_CHANNEL[0].TC_RC = 2625; /* set RC val */
    TC_Configure(TC2, 0, cmr_val);
    TC2->TC_CHANNEL[0].TC_IER = TC_IER_CPCS; /* enable rc cmp interrupt */
    TC2->TC_CHANNEL[0].TC_IDR = ~TC_IER_CPCS; /* not disable rc cmp interrupt */

    NVIC_ClearPendingIRQ(TC6_IRQn);
    NVIC_EnableIRQ(TC6_IRQn);

    pinMode(led_pin, OUTPUT);
    pinMode(t1_pin, INPUT);
    pinMode(t2_pin, INPUT);
    TC_Start(TC2, 0);
    Serial.begin(9600);
}

void loop()
{
    delay(100);
    analogWrite(led_pin, brightness);
    Serial.println(brightness);
}
