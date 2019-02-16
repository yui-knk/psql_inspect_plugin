#ifndef PSQL_INSPECT_PLAN_H
#define PSQL_INSPECT_PLAN_H

#include "nodes/plannodes.h"

#include <mruby.h>

mrb_value psql_inspect_plan_build_from_plan(mrb_state *mrb, Plan *plan);
void psql_inspect_plan_class_init(mrb_state *mrb, struct RClass *class);

#endif  /* PSQL_INSPECT_PLAN_H */
