/* pins */
const uint8_t vref_pin = A1;
const uint8_t az_pin = 2;
const uint8_t x45out_pin = A2;
const uint8_t z45out_pin = A3;

/* values in mv */
double vref_mv;
double x_mv;
double z_mv;

double x_val = 0;
double z_val = 0;
double x_cal = 0;
double z_cal = 0;

uint8_t az_cnt = 0;

void TC8_Handler (void)
{ 
    uint32_t status;
    status = TC2->TC_CHANNEL[2].TC_SR;

    vref_mv = analogRead(vref_pin) * 5000.0/1024.0;
    x_mv = analogRead(x45out_pin) * 5000.0/1024.0;
    z_mv = analogRead(z45out_pin) * 5000.0/1024.0;

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
    pinMode(x45out_pin, OUTPUT);
    pinMode(z45out_pin, OUTPUT);

    digitalWrite(az_pin, LOW);

    delay(6);
    vref_mv = analogRead(vref_pin) * 5000.0/1024.0;
    x_mv = analogRead(x45out_pin) * 5000.0/1024.0;
    z_mv = analogRead(z45out_pin) * 5000.0/1024.0;
    x_cal = (x_mv - vref_mv) / 9.1;
    z_cal = (z_mv - vref_mv) / 9.1;

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
    x_val = (x_mv - vref_mv) / 9.1 - x_cal;
    z_val = (z_mv - vref_mv) / 9.1 - z_cal;
    Serial.print("vref: ");
    Serial.print(vref_mv);
    Serial.print(' ');
    Serial.print(x_mv);
    Serial.print(' ');
    Serial.print(z_mv);
    Serial.print("  x: ");
    Serial.print(x_val);
    Serial.print("  z: ");
    Serial.print(z_val);
    Serial.print('\n');
}
