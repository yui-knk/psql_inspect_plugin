#ifndef PSQL_INSPECT_NODES_H
#define PSQL_INSPECT_NODES_H

#include "postgres.h"

#include <mruby.h>

mrb_value psql_inspect_mrb_str_from_NodeTag(mrb_state *mrb, NodeTag type);
mrb_value psql_inspect_mrb_str_from_CmdType(mrb_state *mrb, CmdType type);
mrb_value psql_inspect_mrb_str_from_AggStrategy(mrb_state *mrb, AggStrategy strategy);

#endif  /* PSQL_INSPECT_NODES_H */
