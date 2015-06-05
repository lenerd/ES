#include <Wire.h>

/* pins */
const uint8_t led_pin = 13;
volatile bool led_on = false;

/* messages */
const uint8_t msg_on = 0x1;
const uint8_t msg_off = 0x2;

/* i2c addresses */
const uint8_t adr_slave = 0x02;

volatile bool scomm = false;
volatile bool next = msg_on;


void TC7_Handler (void)
{ 
    uint32_t status;
    status = TC2->TC_CHANNEL[1].TC_SR;

    if (next == msg_on)
        next = msg_off;
    else
        next = msg_on;
    scomm = true;
}


void recv_handler (int num_bites)
{
    uint8_t bite;
    while (num_bites > 0)
    {
        bite = Wire.read();
        if (bite == msg_on)
            led_on = true;
        else if (bite == msg_off)
            led_on = false;
        --num_bites;
    }
}


void requ_handler (void)
{
    if (led_on == HIGH)
        Wire.write(msg_on);
    else
        Wire.write(msg_off);
}


void setup (void) {
    uint32_t cmr7_val = 0;
    pmc_set_writeprotect(false);
    pmc_enable_periph_clk(ID_TC7);
    
    /* configure TC7 */
    cmr7_val |= TC_CMR_WAVE;         /* waveform mode */
    cmr7_val |= TC_CMR_WAVSEL_UP_RC; /* counting up until eq RC */
    cmr7_val |= TC_CMR_TCCLKS_TIMER_CLOCK3; /* MCK/32 */
    TC2->TC_CHANNEL[1].TC_RC = 5250000; /* set RC val */
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
    digitalWrite(led_pin, LOW);


    Serial.begin(9600);
    Serial.println("setup done");
    Wire.begin(adr_slave); /* slave */
    Wire.onReceive(recv_handler);
    Wire.onRequest(requ_handler);
}


void loop (void)
{
    digitalWrite(led_pin, led_on);
}
