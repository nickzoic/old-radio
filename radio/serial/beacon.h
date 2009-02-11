// $Id: beacon.h,v 1.2 2009-02-11 07:29:34 nick Exp $

#define VLOC_DIM (3)
#define MAXNEIGH (20)
#define MAXSTRAT (5)
#define STRAT_INF (255)

typedef int nodeid_t;
typedef int loc_t;

typedef loc_t vloc_t[VLOC_DIM];

typedef struct {
    nodeid_t id;
    unsigned char stratum;
    vloc_t vloc;
} neighbour_t;

void beacon_init(nodeid_t identifier);   

void beacon_recv(unsigned char *buffer, int length);

int beacon_prepare(unsigned char *buffer, int length);
