#ifndef PARSE_H
#define PARSE_H

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

#endif /* PARSE_H */
