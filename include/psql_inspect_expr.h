#ifndef PSQL_INSPECT_EXPR_H
#define PSQL_INSPECT_EXPR_H

#include "nodes/primnodes.h"

#include <mruby.h>

mrb_value psql_inspect_expr_build_from_expr(mrb_state *mrb, Expr *expr);
void psql_inspect_expr_class_init(mrb_state *mrb, struct RClass *class);

#endif  /* PSQL_INSPECT_EXPR_H */
