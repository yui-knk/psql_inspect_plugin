#ifndef PSQL_INSPECT_QUERY_H
#define PSQL_INSPECT_QUERY_H

#include "postgres.h"
#include "nodes/parsenodes.h"

#include <mruby.h>

void psql_inspect_query_mruby_env_setup(mrb_state *mrb, Query *query);
void psql_inspect_query_mruby_env_tear_down(mrb_state *mrb);
void psql_inspect_query_fini(mrb_state *mrb);
void psql_inspect_query_class_init(mrb_state *mrb, struct RClass *class);

mrb_value psql_inspect_range_tbl_entry_build_from_rte(mrb_state *mrb, RangeTblEntry *rte);

#endif  /* PSQL_INSPECT_QUERY_H */
