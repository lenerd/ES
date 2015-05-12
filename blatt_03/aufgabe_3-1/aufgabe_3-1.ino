/* pins */
const uint8_t vref_pin = A1;
const uint8_t az_pin = 2;
const uint8_t x45out_pin = A2;
const uint8_t z45out_pin = A3;


uint16_t vref_val = 0;
uint16_t x45out_val = 0;
uint16_t z45out_val = 0;
double x_val = 0;
double z_val = 0;
double x_cal = 0;
double z_cal = 0;


void TC8_Handler (void)
{ 
    uint32_t status;
    status = TC2->TC_CHANNEL[2].TC_SR;

    vref_val = 0x3ff & analogRead(vref_pin);
    x45out_val = 0x3ff & analogRead(x45out_pin);
    z45out_val = 0x3ff & analogRead(z45out_pin);

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

    vref_val = 0x3ff & analogRead(vref_pin);
    x45out_val = 0x3ff & analogRead(x45out_pin);
    z45out_val = 0x3ff & analogRead(z45out_pin);
    delay(3);
    x_cal = (x45out_val - vref_val) / 9.1;
    z_cal = (z45out_val- vref_val) / 9.1;
    delay(3);
    x_cal = (x45out_val - vref_val) / 9.1;
    z_cal = (z45out_val- vref_val) / 9.1;

    Serial.begin(9600);
    Serial.println("setup done");
}

void loop() {
    delay(200);
    digitalWrite(az_pin, HIGH);
    delay(1);
    digitalWrite(az_pin, LOW);
    delay(7);
    x_val = (x45out_val - vref_val) / 9.1 - x_cal;
    z_val = (z45out_val- vref_val) / 9.1 - z_cal;
    Serial.print("vref: ");
    Serial.print(vref_val);
    // Serial.print(' ');
    // Serial.print(x45out_val);
    // Serial.print(' ');
    // Serial.print(z45out_val);
    Serial.print("  x: ");
    Serial.print(x_val);
    Serial.print("  z: ");
    Serial.print(z_val);
    Serial.print('\n');
}
