/* pins */
const uint8_t standby_pin = 2;
const uint8_t ain1_pin = 3;
const uint8_t ain2_pin = 4;
const uint8_t apwm_pin = 5;
const uint8_t t1_pin = 8;
const uint8_t t2_pin = 9;

/* states */
volatile uint8_t s_current = 0;
const uint8_t s_stop = 0;
const uint8_t s_cw = 1;
const uint8_t s_ccw = 2;

/* motor cotrol */
volatile uint8_t pwm = 0;
volatile bool incr = true;

/* debouncing */
volatile uint8_t t1_cnt = 0;
volatile uint8_t t2_cnt = 0;


uint8_t next_state(uint8_t current)
{
    switch (current)
    {
        case s_stop:
            return s_cw;
        case s_cw:
            return s_ccw;
        case s_ccw:
            return s_stop;
        default:
            return s_stop;
    }
}


void TC8_Handler (void)
{ 
    uint32_t status;
    uint8_t pwm_tmp;
    status = TC2->TC_CHANNEL[2].TC_SR;

    if (digitalRead(t1_pin) == LOW)
    {
        ++t1_cnt;
        if (t1_cnt >= 100)
        {
            s_current = next_state(s_current);
            switch (s_current)
            {
                case s_stop:
                    digitalWrite(ain1_pin, LOW);
                    digitalWrite(ain2_pin, LOW);
                    break;
                case s_cw:
                    digitalWrite(ain1_pin, HIGH);
                    digitalWrite(ain2_pin, LOW);
                    break;
                case s_ccw:
                    digitalWrite(ain1_pin, LOW);
                    digitalWrite(ain2_pin, HIGH);
                    break;
                default:
                    break;
            }
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
            if (incr)
            {
                pwm_tmp = pwm + 5;
                if (pwm_tmp < pwm)
                {
                    pwm = 255;
                    incr = !incr;
                }
                else
                {
                    pwm = pwm_tmp;
                }
            }
            else
            {
                pwm_tmp = pwm - 5;
                if (pwm_tmp > pwm)
                {
                    pwm = 0;
                    incr = !incr;
                }
                else
                {
                    pwm = pwm_tmp;
                }
            }

            t2_cnt = 0;
        }
    }
    else
    {
        t2_cnt = 0;
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
    TC2->TC_CHANNEL[2].TC_RC = 2625; /* set RC val */
    TC_Configure(TC2, 2, cmr8_val);
    TC2->TC_CHANNEL[2].TC_IER = TC_IER_CPCS; /* enable rc cmp interrupt */
    TC2->TC_CHANNEL[2].TC_IDR = ~TC_IER_CPCS; /* not disable rc cmp interrupt */

    /* enable interrupts */
    NVIC_ClearPendingIRQ(TC8_IRQn);
    NVIC_EnableIRQ(TC8_IRQn);

    /* start timer */
    TC_Start(TC2, 2);

    /* configure i/o pins */
    pinMode(t1_pin, INPUT);
    pinMode(t2_pin, INPUT);
    pinMode(standby_pin, OUTPUT);
    pinMode(ain1_pin, OUTPUT);
    pinMode(ain2_pin, OUTPUT);
    pinMode(apwm_pin, OUTPUT);

    digitalWrite(ain1_pin, LOW);
    digitalWrite(ain2_pin, LOW);
    analogWrite(apwm_pin, 0);
    digitalWrite(standby_pin, HIGH);

    Serial.begin(9600);
    Serial.println("setup done");
}

void loop() {
    delay(100);
    analogWrite(apwm_pin, pwm);
    Serial.println(pwm);
}
