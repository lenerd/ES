#include <SD.h>
#include <SPI.h>

/* pins */
const uint8_t rst_pin = 6;
const uint8_t dc_pin = 5;
const uint8_t lcd_ss_pin = 10;
const uint8_t sd_ss_pin = 4;
extern unsigned char font[95][6];


const uint32_t max_filename_len = 255;
char filename_buf[256];
volatile int command;
volatile int count_buf = 0;
volatile int8_t bite;
volatile uint32_t state = 0;
volatile uint32_t fn_index = 0;

char gline[100];
volatile uint8_t gi = 0;


void setup() {


  Serial.begin(9600);
  Serial1.begin(9600);

  /* sd card */
  if (SD.begin(sd_ss_pin))
      Serial.println("found sd card");
  else
      Serial.println("sd card not found ");

}

void error (void)
{
    Serial.println(state);
    Serial.println((int)bite);
    Serial.println("bitte geben Sie einen gueltigen Befehl ein");
    while (bite != '\n')
        bite = Serial.read();
    state = 0;

}


void loop() {
    while ((bite = Serial1.read()) != -1)
    {
        Serial.print((char)bite);
    }
}
