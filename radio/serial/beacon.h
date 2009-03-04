// $Id: beacon.h,v 1.4 2009-03-04 07:45:13 nick Exp $

#define VLOC_DIM (3)
#define MAXNEIGH (20)
#define MAXSTRAT (5)
#define STRAT_INF (255)
#define BEACON_TIMEOUT (3)

typedef u_int16_t nodeid_t;
typedef int32_t loc_t;
typedef u_int8_t strat_t;

typedef loc_t vloc_t[VLOC_DIM];

struct beacon_s {
    nodeid_t id;
    strat_t stratum;
    vloc_t vloc;
} __attribute__((__packed__));
typedef struct beacon_s beacon_t;

struct neighbour_s {
    int state;
    nodeid_t id;
    strat_t stratum;
    vloc_t vloc;
    time_t stamp;
};
typedef struct neighbour_s neighbour_t;

void beacon_init(nodeid_t identifier);   

void beacon_recv(unsigned char *buffer, int length);

int beacon_prepare(unsigned char *buffer, int length);
