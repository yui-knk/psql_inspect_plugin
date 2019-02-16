#include "postgres.h"
#include "nodes/primnodes.h"

#include <mruby.h>
#include <mruby/class.h>
#include <mruby/data.h>

#include <psql_inspect_expr.h>
#include <psql_inspect_nodes.h>

static struct RClass *expr_class = NULL;
static const struct mrb_data_type psql_inspect_expr_data_type = { "Expr", mrb_free };

static void
psql_inspect_expr_set_expr(mrb_state *mrb, mrb_value self, Expr *expr)
{
    DATA_PTR(self) = expr;
}

static mrb_value
psql_inspect_expr_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_expr_data_type;

    return self;
}

mrb_value
psql_inspect_expr_build_from_expr(mrb_state *mrb, Expr *expr)
{
    mrb_value val = mrb_class_new_instance(mrb, 0, NULL, expr_class);
    psql_inspect_expr_set_expr(mrb, val, expr);

    return val;
}

static mrb_value
psql_inspect_expr_type(mrb_state *mrb, mrb_value self)
{
    Expr *expr;

    expr = (Expr *)DATA_PTR(self);
    return psql_inspect_mrb_str_from_NodeTag(mrb, expr->type);
}

void
psql_inspect_expr_class_init(mrb_state *mrb, struct RClass *class)
{
    expr_class = mrb_define_class_under(mrb, class, "Expr", mrb->object_class);
    MRB_SET_INSTANCE_TT(expr_class, MRB_TT_DATA);

    mrb_define_method(mrb, expr_class, "initialize", psql_inspect_expr_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, expr_class, "type", psql_inspect_expr_type, MRB_ARGS_NONE());
}
