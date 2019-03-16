#ifndef PSQL_INSPECT_PRIMNODES_H
#define PSQL_INSPECT_PRIMNODES_H

#include "nodes/primnodes.h"

#include <mruby.h>

mrb_value psql_inspect_primnode_build_from_node(mrb_state *mrb, Node *node);
mrb_value psql_inspect_primnode_build_from_expr(mrb_state *mrb, Expr *expr);
void psql_inspect_primnodes_fini(mrb_state *mrb);
void psql_inspect_primnodes_class_init(mrb_state *mrb, struct RClass *class);

#endif  /* PSQL_INSPECT_PRIMNODES_H */
