#include <SPI.h>
#include <SD.h>

/* pins */
const uint8_t rst_pin = 6;
const uint8_t dc_pin = 5;
const uint8_t lcd_ss_pin = 10;
const uint8_t sd_ss_pin = 4;

uint8_t lcd_buf[6][84];

const uint32_t max_filename_len = 255;
char filename_buf[256];
uint32_t state = 0;
uint32_t fn_index = 0;


void set_pixel (uint8_t x, uint8_t y, int value)
{
    if (value)
        lcd_buf[y / 8][x] |= 1 << (y % 8);
    else
        lcd_buf[y / 8][x] &= ~(1 << (y % 8));
}

void buffer_to_lcd (void)
{
    for (uint8_t y = 0; y < 6; ++y)
        for (uint8_t x = 0; x < 84; ++x)
            SPI.transfer(lcd_ss_pin, lcd_buf[y][x]);
}

void invert_col (uint8_t x)
{
    for (uint8_t y = 0; y < 6; ++y)
        lcd_buf[y][x] = ~lcd_buf[y][x];
}

void error (void)
{
    Serial.println("o.0");
}

void setup() {
  pinMode(rst_pin, OUTPUT);
  pinMode(dc_pin, OUTPUT);

  digitalWrite(rst_pin, LOW);
  delay(500);
  digitalWrite(rst_pin, HIGH);

  SPI.begin(lcd_ss_pin);
  SPI.setClockDivider(lcd_ss_pin, 84);

  /* command mode */
  digitalWrite(dc_pin, LOW);
  SPI.transfer(lcd_ss_pin, 0x21);
  SPI.transfer(lcd_ss_pin, 0x14);
  SPI.transfer(lcd_ss_pin, 0xE0);
  SPI.transfer(lcd_ss_pin, 0x20);
  SPI.transfer(lcd_ss_pin, 0x0C);

  /* set ram address */
  SPI.transfer(lcd_ss_pin, 0x80);
  SPI.transfer(lcd_ss_pin, 0x40);

  /* data mode */
  digitalWrite(dc_pin, HIGH);
  buffer_to_lcd();

  /* sd card */
  SD.begin(sd_ss_pin);

  Serial.begin(9600);
}

void loop() {
}
