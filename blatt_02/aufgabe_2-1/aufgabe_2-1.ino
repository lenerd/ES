const int led_pin = 7;
const int t1_pin = 5;
const int t2_pin = 3;

volatile uint8_t t_cnt = 0;
volatile uint8_t t_id = 0;
volatile uint8_t brightness = 100;
volatile bool next_state = LOW;

void TC6_Handler (void)
{ 
    uint32_t status;
    status = TC2->TC_CHANNEL[0].TC_SR;

    // ++t_cnt;
    // if (t_cnt > 1)
    // {
    //     if (next_state == HIGH)
    //     {
    //         brightness = 255;
    //         next_state = LOW;
    //         t_cnt = 0;
    //     }
    //     else
    //     {
    //         brightness = 0;
    //         next_state = HIGH;
    //         t_cnt = 0;
    //     }
    // }
    // Serial.println("foo");
    // return;

    if (digitalRead(t1_pin) == LOW)
    {
        if (t_id != 1)
        {
            t_id = 1;
            t_cnt = 0;
        }
        ++t_cnt;
        if (t_cnt >= 32)
        {
            brightness += 5;
            t_cnt = 0;
        }
    }
    else if (digitalRead(t2_pin) == LOW)
    {
        if (t_id != 2)
        {
            t_id = 2;
            t_cnt = 0;
        }
        ++t_cnt;
        if (t_cnt >= 32)
        {
            brightness -= 5;
            t_cnt = 0;
        }
    }
    else
    {
        t_cnt = 0;
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
    Serial.println("blubb\n");
    // Serial.println("blubb\n");
}

void loop()
{
    delay(100);
    analogWrite(led_pin, brightness);
    Serial.println(brightness);
}
