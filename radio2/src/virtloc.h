// $Id: virtloc.h,v 1.3 2009-10-18 13:35:20 nick Exp $

#ifndef _VIRTLOC_H
#define _VIRTLOC_H

#include "vtime.h"
#include "loc.h"
#include "neigh.h"

typedef struct virtloc_s {
    loc_t loc;
} virtloc_t;

void virtloc_init(virtloc_t *virtloc, neigh_id_t id);

void virtloc_recalc(virtloc_t *virtloc, neigh_table_t neigh_table);

#endif
