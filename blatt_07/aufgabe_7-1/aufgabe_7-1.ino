#include <SD.h>
#include <SPI.h>
#include "parse.h"


/* pins */
const uint8_t rst_pin = 6;
const uint8_t dc_pin = 5;
const uint8_t lcd_ss_pin = 10;
const uint8_t sd_ss_pin = 4;
extern unsigned char font[95][6];
const uint8_t led_pin = 13;


const uint32_t max_filename_len = 255;
char filename_buf[256];
volatile int command;
volatile int count_buf = 0;
volatile int8_t bite;
volatile uint32_t state = 0;
volatile uint32_t fn_index = 0;

char gline[100];
volatile uint8_t gi = 0;

char buf[256];
const char* end_buf = &buf[256];

uint8_t checksum(char* start, char* end)
{
    uint8_t xorx = 0;
    for (char* it = start; it != end; ++it)
        xorx ^= *it;
    return xorx;
}

void print_pos (position_t* pos)
{
    //printf("%" PRIi32 "° %f' %c, %" PRIi32 "° %f' %c, %u\n", pos->lat_deg, pos->lat_min, pos->lat_ns, pos->lon_deg, pos->lon_min, pos->lon_ew, pos->cnt);
    Serial.print(pos->lat_deg);
    Serial.print(" deg ");
    Serial.print(pos->lat_min);
    Serial.print("' ");
    Serial.print((char)pos->lat_ns);
    Serial.print(", ");
    Serial.print(pos->lon_deg);
    Serial.print(" deg ");
    Serial.print(pos->lon_min);
    Serial.print("' ");
    Serial.print((char)pos->lon_ew);
    Serial.print(", ");
    Serial.print(pos->cnt);
    Serial.println();
}

void write_pos (position_t* pos)
{
    File f = SD.open("gps", FILE_WRITE);
    f.print(pos->lat_deg);
    f.print(".");
    f.print(pos->lat_min / 60);
    f.print((char)pos->lat_ns);
    f.print(", ");
    f.print(pos->lon_deg);
    f.print(".");
    f.print(pos->lon_min / 60);
    f.print((char)pos->lon_ew);
    f.println();
    f.close();
    if (!SD.exists("gps"))
        Serial.println("write failed");
}


void parse_gga (void)
{
    position_t pos;
    int c;
    uint8_t xorx = 0x00;
    uint8_t chk = 0x00;
    char* it = buf;
    char* start = NULL;
    char* end = NULL;
    char* lat = NULL;
    char* lon = NULL;
    char* lat_ns = NULL;
    char* lon_ew = NULL;
    char* cnt = NULL;
    char* tok_ptr = NULL;
    char* fix = NULL;
    c = Serial1.read();

    do
    {
        if (c == -1)
            continue;

        *it = c;
        if (c == '$')
            start = it + 1;
        else if (c == '*')
            end = it;
        ++it;
    }
    while ((c = Serial1.read()) != '\n' && it != end_buf);

    *it = '\0';

    if (start == NULL || end == NULL)
        return;

    xorx = checksum(start, end);
    chk = strtoul(end + 1, NULL, 16);
    *end = '*';
    if (xorx != chk)
    {
        return;
    }
    /* check for gga */
    if (strncmp(start, "GPGGA", 5U) != 0)
        return;
    strtok_r(start, ",", &tok_ptr);
    strtok_r(NULL, ",", &tok_ptr);
    lat = strtok_r(NULL, ",", &tok_ptr);
    lat_ns = strtok_r(NULL, ",", &tok_ptr);
    lon = strtok_r(NULL, ",", &tok_ptr);
    lon_ew = strtok_r(NULL, ",", &tok_ptr);
    fix = strtok_r(NULL, ",", &tok_ptr);
    cnt = strtok_r(NULL, ",", &tok_ptr);
    pos.lat_deg = 10 * (lat[0] - '0') + (lat[1] - '0');
    pos.lat_min = strtof(lat + 2, NULL);
    pos.lat_ns = (carddir_t) *lat_ns;
    pos.lon_deg = 100 * (lon[0] - '0') + 10 * (lon[1] - '0') + (lon[2] - '0');
    pos.lon_min = strtof(lon + 3, NULL);
    pos.lon_ew = (carddir_t) *lon_ew;
    pos.cnt = strtoul(cnt, NULL, 10);;

    if (*fix == '0')
    {
        Serial.println("no fix");
        digitalWrite(led_pin, LOW);
    }
    else
    {
        print_pos(&pos);
        write_pos(&pos);
        digitalWrite(led_pin, HIGH);
    }

}


void setup() {


  Serial.begin(9600);
  Serial1.begin(9600);

  pinMode(led_pin, OUTPUT);

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
    // while ((bite = Serial1.read()) != -1)
    // {
    //     Serial.print((char)bite);
    // }
    parse_gga();
}
