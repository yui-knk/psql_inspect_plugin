#ifndef PSQL_INSPECT_REL_OPT_INFO_H
#define PSQL_INSPECT_REL_OPT_INFO_H

#include "nodes/relation.h"

#include <mruby.h>

mrb_value psql_inspect_rel_opt_info_build_from_rel_opt_info(mrb_state *mrb, RelOptInfo *rel);
void psql_inspect_rel_opt_info_class_init(mrb_state *mrb, struct RClass *class);

#endif  /* PSQL_INSPECT_REL_OPT_INFO_H */
