#include <SPI.h>

extern unsigned char font[95][6];

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

int print_char(int x, int y, char value)
{
    if(!(x >= 0 && x <= 78 && y >= 0 && y < 40))
    {
        return -1;
    }
    for(int i= 0; i < 6; ++i)
    {
      char index = value - ' ';
      int start = y / 8;
      int shift = y % 8;
      uint16_t tmp = (lcd_buf[start+1][x+i] << 8) | lcd_buf[start][x+i];
      uint16_t f = font[index][i];
      tmp &= ~(0xff << shift);
      tmp |= f << shift;  
      uint8_t* foo = (uint8_t*) &tmp;
      lcd_buf[start][x+i] = foo[0];
      lcd_buf[start+1][x+i] = foo[1];
    }
    return 0;
}
void test (uint8_t i)
{
    memset(lcd_buf, 0x00, 6 * 84);
    if (i == 0)
    {
        print_char( 3, 12, 'L');
        print_char( 9, 12, 'e');
        print_char(15, 12, 'n');
        print_char(21, 12, 'n');
        print_char(27, 12, 'a');
        print_char(33, 12, 'r');
        print_char(39, 12, 't');
        print_char(45, 12, ' ');
        print_char(51, 12, 'B');
        print_char(57, 12, 'r');
        print_char(63, 12, 'a');
        print_char(69, 12, 'u');
        print_char(75, 12, 'n');

        print_char(21, 32, '6');
        print_char(27, 32, '5');
        print_char(33, 32, '2');
        print_char(39, 32, '3');
        print_char(45, 32, '7');
        print_char(51, 32, '4');
        print_char(57, 32, '2');
    }
    else if (i == 1)
    {
        print_char( 6, 16, 'M');
        print_char(12, 16, 'a');
        print_char(18, 16, 'x');
        print_char(24, 16, ' ');
        print_char(30, 16, '3');
        print_char(36, 16, 'k');
        print_char(42, 16, 'n');
        print_char(48, 16, 'a');
        print_char(54, 16, 'p');
        print_char(60, 16, 'p');
        print_char(68, 16, 'e');
        print_char(74, 16, 'r');

        print_char(21, 32, '6');
        print_char(27, 32, '5');
        print_char(33, 32, '3');
        print_char(39, 32, '5');
        print_char(45, 32, '0');
        print_char(51, 32, '9');
        print_char(57, 32, '0');
    }
    else if (i == 2)
    {
        print_char( 3, 16, 'M');
        print_char( 9, 16, 'a');
        print_char(15, 16, 'r');
        print_char(21, 16, 'c');
        print_char(27, 16, 'o');
        print_char(33, 16, ' ');
        print_char(39, 16, 'P');
        print_char(45, 16, 'f');
        print_char(51, 16, 'o');
        print_char(57, 16, 'm');
        print_char(63, 16, 'a');
        print_char(69, 16, 'n');
        print_char(75, 16, 'n');

        print_char(21, 32, '6');
        print_char(27, 32, '3');
        print_char(33, 32, '4');
        print_char(39, 32, '5');
        print_char(45, 32, '2');
        print_char(51, 32, '3');
        print_char(57, 32, '3');
    }

    buffer_to_lcd();
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

  // set ram address
  SPI.transfer(ss_pin, 0x80);
  SPI.transfer(ss_pin, 0x40);


  /* data mode */
  digitalWrite(dc_pin, HIGH);
}

void loop() {
    for (uint8_t i = 0; i < 3; ++i)
    {
        test(i);
        delay(5000);
    }
}
