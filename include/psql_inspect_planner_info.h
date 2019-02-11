#ifndef PSQL_INSPECT_PLANNER_INFO_H
#define PSQL_INSPECT_PLANNER_INFO_H

#include "postgres.h"
#include "nodes/relation.h"

#include <mruby.h>

void psql_inspect_planner_info_mruby_env_setup(mrb_state *mrb, PlannerInfo *root);
void psql_inspect_planner_info_mruby_env_tear_down(mrb_state *mrb);
void psql_inspect_planner_info_fini(mrb_state *mrb);
void psql_inspect_planner_info_class_init(mrb_state *mrb, struct RClass *class);

#endif  /* PSQL_INSPECT_PLANNER_INFO_H*/
