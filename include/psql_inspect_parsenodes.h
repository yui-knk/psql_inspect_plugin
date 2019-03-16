#ifndef PSQL_INSPECT_PARSENODES_H
#define PSQL_INSPECT_PARSENODES_H

#include "nodes/parsenodes.h"

#include <mruby.h>

mrb_value psql_inspect_parsenode_build_from_node(mrb_state *mrb, Node *node);
void psql_inspect_parsenodes_fini(mrb_state *mrb);
void psql_inspect_parsenodes_class_init(mrb_state *mrb, struct RClass *class);

#endif  /* PSQL_INSPECT_PARSENODES_H */
