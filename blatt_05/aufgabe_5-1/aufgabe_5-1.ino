#include <SPI.h>

/* pins */
const uint8_t rst_pin = 6;
const uint8_t dc_pin = 5;
const uint8_t ss_pin = 10;

uint8_t lcd_buf[6][84];

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
            SPI.transfer(ss_pin, lcd_buf[y][x]);
}

void invert_col (uint8_t x)
{
    for (uint8_t y = 0; y < 6; ++y)
        lcd_buf[y][x] = ~lcd_buf[y][x];
}

void setup() {
  pinMode(rst_pin, OUTPUT);
  pinMode(dc_pin, OUTPUT);

  digitalWrite(rst_pin, LOW);
  delay(500);
  digitalWrite(rst_pin, HIGH);

  SPI.begin(ss_pin);
  SPI.setClockDivider(ss_pin, 84);

  /* command mode */
  digitalWrite(dc_pin, LOW);
  SPI.transfer(ss_pin, 0x21);
  SPI.transfer(ss_pin, 0x14);
  SPI.transfer(ss_pin, 0xE0);
  SPI.transfer(ss_pin, 0x20);
  SPI.transfer(ss_pin, 0x0C);


  /* data mode */
  digitalWrite(dc_pin, HIGH);
  set_pixel(30, 30, 1);
  buffer_to_lcd();
}

void loop() {
  // put your main code here, to run repeatedly:

}
