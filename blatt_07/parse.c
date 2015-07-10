#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

typedef enum
{
    NORTH = 'N',
    WEST = 'W',
    SOUTH = 'S',
    EAST = 'E',
} carddir_t;

typedef struct
{
    int32_t lat_deg;
    float lat_min;
    carddir_t lat_ns;
    int32_t lon_deg;
    float lon_min;
    carddir_t lon_ew;
    uint32_t cnt;
} position_t;


char buf[256];
char* end_buf = &buf[256];

uint8_t checksum(char* start, char* end)
{
    uint8_t xor = 0;
    for (char* it = start; it != end; ++it)
        xor ^= *it;
    return xor;
}

void print_pos (position_t* pos)
{
    printf("%" PRIi32 "° %f' %c, %" PRIi32 "° %f' %c, %u\n", pos->lat_deg, pos->lat_min, pos->lat_ns, pos->lon_deg, pos->lon_min, pos->lon_ew, pos->cnt);
}

void parse_gga (void)
{
    position_t pos;
    int c;
    uint8_t xor = 0x00;
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
    c = getchar();

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
    while ((c = getchar()) != '\n' && it != end_buf);

    *it = '\0';

    if (start == NULL || end == NULL)
        return;

    xor = checksum(start, end);
    chk = strtoul(end + 1, NULL, 16);
    *end = '*';
    if (xor != chk)
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
    strtok_r(NULL, ",", &tok_ptr);
    cnt = strtok_r(NULL, ",", &tok_ptr);
    pos.lat_deg = 10 * (lat[0] - '0') + (lat[1] - '0');
    pos.lat_min = strtof(lat + 3, NULL);
    pos.lat_ns = *lat_ns;
    pos.lon_deg = 10 * (lon[0] - '0') + (lon[1] - '0');
    pos.lon_min = strtof(lat + 3, NULL);
    pos.lon_ew = *lon_ew;
    pos.cnt = strtoul(cnt, NULL, 10);;

    print_pos(&pos);
    //printf("%s %s; %s %s; %s\n", lat, lat_ns, lon, lon_ew, cnt);

}

int main (void)
{
    while (1)
        parse_gga();
}
