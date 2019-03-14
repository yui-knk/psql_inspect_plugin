#ifndef PSQL_INSPECT_NODES_H
#define PSQL_INSPECT_NODES_H

#include "postgres.h"

#include <mruby.h>

extern struct RClass *psql_inspect_node_class;

mrb_value psql_inspect_mrb_str_from_NodeTag(mrb_state *mrb, NodeTag type);
mrb_value psql_inspect_mrb_str_from_CmdType(mrb_state *mrb, CmdType type);
mrb_value psql_inspect_mrb_str_from_AggStrategy(mrb_state *mrb, AggStrategy strategy);

void psql_inspect_node_fini(mrb_state *mrb);
void psql_inspect_node_class_init(mrb_state *mrb, struct RClass *class);

#endif  /* PSQL_INSPECT_NODES_H */
