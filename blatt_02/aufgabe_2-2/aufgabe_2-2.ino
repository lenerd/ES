const int standby_pin = 2;
const int ain1_pin = 3;
const int ain2_pin = 4;
const int apwm_pin = 5;
volatile bool cw = true;
volatile uint8_t pwm = 0;
volatile bool incr = true;
volatile bool next = false;

void TC7_Handler (void) {
    uint32_t status;
    status = TC2->TC_CHANNEL[1].TC_SR;
    if (incr)
    {
        if (next)
        {
            ++pwm;
            next = false;
            if (pwm == 255)
            {
                incr = false;
            }
        }
        else
        {
            next = true;
        }

    }
    else
    {
        --pwm;
        if (pwm == 0)
        {
            incr = true;
            cw = !cw;
            if (cw)
            {

                digitalWrite(ain1_pin, HIGH);
                digitalWrite(ain2_pin, LOW);
            }
            else
            {
                    
                digitalWrite(ain1_pin, LOW);
                digitalWrite(ain2_pin, HIGH);
            }
        }
    }
}
void setup() {
    uint32_t cmr_val = 0;
    pmc_set_writeprotect(false);
    pmc_enable_periph_clk(ID_TC7);

    cmr_val |= TC_CMR_WAVE;         /* waveform mode */
    cmr_val |= TC_CMR_WAVSEL_UP_RC; /* counting up until eq RC */
    cmr_val |= TC_CMR_TCCLKS_TIMER_CLOCK4; /* MCK/128 */
    TC2->TC_CHANNEL[1].TC_RC = 12868; /* set RC val */
    TC_Configure(TC2, 1, cmr_val);
    TC2->TC_CHANNEL[1].TC_IER = TC_IER_CPCS; /* enable rc cmp interrupt */
    TC2->TC_CHANNEL[1].TC_IDR = ~TC_IER_CPCS; /* not disable rc cmp interrupt */

    NVIC_ClearPendingIRQ(TC7_IRQn);
    NVIC_EnableIRQ(TC7_IRQn);
    TC_Start(TC2, 1);

    pinMode(standby_pin, OUTPUT);
    pinMode(ain1_pin, OUTPUT);
    pinMode(ain2_pin, OUTPUT);
    pinMode(apwm_pin, OUTPUT);

    digitalWrite(ain1_pin, HIGH);
    digitalWrite(ain2_pin, LOW);
    analogWrite(apwm_pin, 0);
    digitalWrite(standby_pin, HIGH);

    Serial.begin(9600);
    Serial.println("setup done");
}

void loop() {
  // put your main code here, to run repeatedly:
    delay(100);
    analogWrite(apwm_pin, pwm);
    Serial.println(pwm);
}
