#include <Wire.h>

/* pins */
const uint8_t vref_pin = A1;
const uint8_t az_pin = 2;
const uint8_t z45out_pin = A3;

/* messages */
const uint8_t msg_on = 0x1;
const uint8_t msg_off = 0x2;

/* i2c addresses */
const uint8_t adr_slave = 0x02;

volatile bool scomm = false;
volatile bool next = msg_on;

/* values in mv */
double vref_mv;
double z_mv;

double z_val = 0;
double z_cal = 0;
int8_t angle_diff = 0;
bool new_val = false;

uint8_t az_cnt = 0;

void TC8_Handler (void)
{ 
    uint32_t status;
    status = TC2->TC_CHANNEL[2].TC_SR;

    vref_mv = analogRead(vref_pin) * 5000.0/1024.0;
    z_mv = analogRead(z45out_pin) * 5000.0/1024.0;
    z_val = ((z_mv - vref_mv) / 9.1 - z_cal) / 40; /* Faktor ? */
    angle_diff = (int8_t) z_val;
    new_val = true;
}


void requ_handler (void)
{
    if (!new_val)
        Wire.write(0);
    else
    {
        Wire.write(angle_diff);
        new_val = false;
    }
}


void setup (void) {
    uint32_t cmr8_val = 0;
    pmc_set_writeprotect(false);
    pmc_enable_periph_clk(ID_TC8);
    
    /* configure TC8 */
    cmr8_val |= TC_CMR_WAVE;         /* waveform mode */
    cmr8_val |= TC_CMR_WAVSEL_UP_RC; /* counting up until eq RC */
    cmr8_val |= TC_CMR_TCCLKS_TIMER_CLOCK3; /* MCK/32 */
    TC2->TC_CHANNEL[2].TC_RC = 131250; /* set RC val */
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
    pinMode(z45out_pin, OUTPUT);

    digitalWrite(az_pin, LOW);

    delay(6);
    vref_mv = analogRead(vref_pin) * 5000.0/1024.0;
    z_mv = analogRead(z45out_pin) * 5000.0/1024.0;
    z_cal = (z_mv - vref_mv) / 9.1;

    Serial.begin(9600);
    Serial.println("setup done");
    Wire.begin(adr_slave); /* slave */
    Wire.onRequest(requ_handler);
}


void loop (void)
{
    delay(250);
    Serial.println(angle_diff);
    Serial.println(z_val);
}
