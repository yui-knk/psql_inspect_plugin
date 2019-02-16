#ifndef PSQL_INSPECT_BITMAPSET_H
#define PSQL_INSPECT_BITMAPSET_H

#include "postgres.h"
#include "nodes/bitmapset.h"

#include <mruby.h>

mrb_value psql_inspect_bitmapset_build_from_bitmapset(mrb_state *mrb, Bitmapset *b);
void psql_inspect_bitmapset_class_init(mrb_state *mrb, struct RClass *class);

#endif  /* PSQL_INSPECT_BITMAPSET_H */
