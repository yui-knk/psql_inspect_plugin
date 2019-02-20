#ifndef PSQL_INSPECT_QUERY_INFO_H
#define PSQL_INSPECT_QUERY_INFO_H

#include "postgres.h"
#include "nodes/parsenodes.h"

#include <mruby.h>

void psql_inspect_query_mruby_env_setup(mrb_state *mrb, Query *query);
void psql_inspect_query_mruby_env_tear_down(mrb_state *mrb);
void psql_inspect_query_fini(mrb_state *mrb);
void psql_inspect_query_class_init(mrb_state *mrb, struct RClass *class);

#endif  /* PSQL_INSPECT_QUERY_INFO_H */