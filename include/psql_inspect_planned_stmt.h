#ifndef PSQL_INSPECT_PLANNED_STMT_H
#define PSQL_INSPECT_PLANNED_STMT_H

#include "postgres.h"

#include <mruby.h>

void psql_inspect_planned_stmt_mruby_env_setup(mrb_state *mrb, PlannedStmt *stmt);
void psql_inspect_planned_stmt_mruby_env_tear_down(mrb_state *mrb);
void psql_inspect_planned_stmt_fini(mrb_state *mrb);
void psql_inspect_planned_stmt_class_init(mrb_state *mrb, struct RClass *class);

#endif  /* PSQL_INSPECT_NODPLANNED_STMT*/
