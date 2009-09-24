// $Id: loc.h,v 1.1 2009-09-24 00:21:15 nick Exp $

typedef int16_t locx_t;
typedef uint32_t locd_t;

typedef struct loc_s {
    locx_t x, y, z;
} loc_t;

locd_t loc_dist2(loc_t *a, loc_t *b);
locd_t loc_dist(loc_t *a, loc_t *b);
void loc_zero(loc_t *a);
void loc_perturb(loc_t *a, locd_t d);
void loc_fprint(loc_t *a, FILE *fp);