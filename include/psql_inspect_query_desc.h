#ifndef PSQL_INSPECT_QUERY_DESC_H
#define PSQL_INSPECT_QUERY_DESC_H

#include "postgres.h"
#include "executor/execdesc.h"

#include <mruby.h>

void psql_inspect_query_desc_mruby_env_setup(mrb_state *mrb, QueryDesc *queryDesc);
void psql_inspect_query_desc_mruby_env_tear_down(mrb_state *mrb);
void psql_inspect_query_desc_fini(mrb_state *mrb);
void psql_inspect_query_desc_class_init(mrb_state *mrb, struct RClass *class);

#endif  /* PSQL_INSPECT_QUERY_DESC_H */
