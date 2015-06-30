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
volatile int command;
volatile int count_buf = 0;
volatile int8_t bite;
volatile uint32_t state = 0;
volatile uint32_t fn_index = 0;


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
    while ((bite = Serial.read()) != -1)
    {
        switch(state)
        {
            case 0:
                memset(filename_buf, 0x0, 256);
                command = 0;
                count_buf = 0;
                if (bite == 'e')
                    state = 11;
                else if (bite == 's')
                    state = 21;
                else
                    error();
                break;

            case 11:
                if (bite == 'x')
                    state = 12;
                else
                    error();
                break;

            case 12:
                if (bite == 'i')
                    state = 13;
                else
                    error();
                break;

            case 13:
                if (bite == 's')
                    state = 14;
                else
                    error();
                break;

            case 14:
                if (bite == 't')
                    state = 15;
                else
                    error();
                break;

            case 15:
                if (bite == 's')
                    state = 16;
                else
                    error();
                break;

            case 16:
                if (bite == ' ')
                    {
                        state = 3;
                        command = 1;
                    }
                else
                    error();
                break;

            case 21:
                if (bite == 'h')
                    state = 22;
                else
                    error();
                break;

            case 22:
                if (bite == 'o')
                    state = 23;
                else
                    error();
                break;

            case 23:
                if (bite == 'w')
                    state = 24;
                else
                    error();
                break;

            case 24:
                if (bite == ' ')
                    { 
                        state = 3;
                        command = 2;
                    }
                else
                    error();
                break;
            
            case 3:
                if(bite == '\n')
                {
                    Serial.print("filename = ");
                    Serial.println(filename_buf);
                    Serial.println("executing command");
                    if(command == 1)
                        Serial.println("command = exists");
                    else if(command == 2)
                        Serial.println("command = show");
                    else
                        Serial.println("aeaeaeahhhhmmm nein");
                    state = 0;
                }
                else if(count_buf >= 255)
                    error();
                else
                   {
                       filename_buf[count_buf] = bite;
                       count_buf++;
                   }
                break;

            default:
                break;
        }
        // Serial.println(state);
    }
}
