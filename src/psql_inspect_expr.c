#include "postgres.h"
#include "nodes/nodes.h"
#include "nodes/pg_list.h"
#include "nodes/primnodes.h"

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/data.h>

#include <psql_inspect_expr.h>
#include <psql_inspect_nodes.h>

static struct RClass *expr_class = NULL;
static struct RClass *relabel_type_class = NULL;
static struct RClass *var_class = NULL;
static struct RClass *aggref_class = NULL;
static struct RClass *const_class = NULL;
static struct RClass *target_entry_class = NULL;

static const struct mrb_data_type psql_inspect_expr_data_type = { "Expr", mrb_free };
static const struct mrb_data_type psql_inspect_relabel_type_data_type = { "RelabelType", mrb_free };
static const struct mrb_data_type psql_inspect_var_data_type = { "Var", mrb_free };
static const struct mrb_data_type psql_inspect_aggref_data_type = { "Aggref", mrb_free };
static const struct mrb_data_type psql_inspect_const_data_type = { "Const", mrb_free };
static const struct mrb_data_type psql_inspect_target_entry_data_type = { "TargetEntry", mrb_free };

mrb_value psql_inspect_expr_build_from_expr(mrb_state *mrb, Expr *expr);


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

static mrb_value
psql_inspect_relabel_type_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_relabel_type_data_type;

    return self;
}

static mrb_value
psql_inspect_var_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_var_data_type;

    return self;
}

static mrb_value
psql_inspect_target_entry_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_target_entry_data_type;

    return self;
}

static mrb_value
psql_inspect_aggref_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_aggref_data_type;

    return self;
}

static mrb_value
psql_inspect_aggref_args(mrb_state *mrb, mrb_value self)
{
    Aggref *aggref;
    int array_size;
    int i = 0;
    mrb_value ary;
    ListCell *lc;

    aggref = (Aggref *)DATA_PTR(self);
    array_size = list_length(aggref->args);
    ary = mrb_ary_new_capa(mrb, array_size);

    foreach(lc, aggref->args) {
        mrb_value v;
        TargetEntry *tle = (TargetEntry *) lfirst(lc);

        v = psql_inspect_expr_build_from_expr(mrb, (Expr *)tle);
        mrb_ary_set(mrb, ary, i, v);
        i++;
    }

    return ary;
}

static mrb_value
psql_inspect_const_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_const_data_type;

    return self;
}

static mrb_value
psql_inspect_const_consttype(mrb_state *mrb, mrb_value self)
{
    Const *c;
    c = (Const *)DATA_PTR(self);

    return mrb_fixnum_value(c->consttype);
}

static mrb_value
psql_inspect_const_constlen(mrb_state *mrb, mrb_value self)
{
    Const *c;
    c = (Const *)DATA_PTR(self);

    return mrb_fixnum_value(c->constlen);
}

static mrb_value
psql_inspect_const_constvalue(mrb_state *mrb, mrb_value self)
{
    Const *c;
    c = (Const *)DATA_PTR(self);

    return mrb_fixnum_value(c->constvalue);
}

static mrb_value
psql_inspect_target_entry_expr(mrb_state *mrb, mrb_value self)
{
    TargetEntry *tle;

    tle = (TargetEntry *)DATA_PTR(self);
    return psql_inspect_expr_build_from_expr(mrb, tle->expr);
 }

static mrb_value
psql_inspect_target_entry_resno(mrb_state *mrb, mrb_value self)
{
    TargetEntry *tle;

    tle = (TargetEntry *)DATA_PTR(self);
    return mrb_fixnum_value(tle->resno);
}

static mrb_value
psql_inspect_target_entry_resname(mrb_state *mrb, mrb_value self)
{
    TargetEntry *tle;

    tle = (TargetEntry *)DATA_PTR(self);
    return mrb_str_new_cstr(mrb, tle->resname);
}

static mrb_value
psql_inspect_target_entry_resjunk(mrb_state *mrb, mrb_value self)
{
    TargetEntry *tle;

    tle = (TargetEntry *)DATA_PTR(self);
    return mrb_bool_value(tle->resjunk);
}

mrb_value
psql_inspect_expr_build_from_expr(mrb_state *mrb, Expr *expr)
{
    mrb_value val;

    switch (expr->type) {
      case T_RelabelType:
        val = mrb_class_new_instance(mrb, 0, NULL, relabel_type_class);
        break;
      case T_Var:
        val = mrb_class_new_instance(mrb, 0, NULL, var_class);
        break;
      case T_Const:
        val = mrb_class_new_instance(mrb, 0, NULL, const_class);
        break;
      case T_Aggref:
        val = mrb_class_new_instance(mrb, 0, NULL, aggref_class);
        break;
      case T_TargetEntry:
        val = mrb_class_new_instance(mrb, 0, NULL, target_entry_class);
        break;
      default:
        val = mrb_class_new_instance(mrb, 0, NULL, expr_class);
    }

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

static mrb_value
psql_inspect_relabel_type_arg(mrb_state *mrb, mrb_value self)
{
    RelabelType *relt;

    relt = (RelabelType *)DATA_PTR(self);
    return psql_inspect_expr_build_from_expr(mrb, relt->arg);
}

static mrb_value
psql_inspect_var_varno(mrb_state *mrb, mrb_value self)
{
    Var *var;

    var = (Var *)DATA_PTR(self);
    return mrb_fixnum_value(var->varno);
}

static mrb_value
psql_inspect_var_varattno(mrb_state *mrb, mrb_value self)
{
    Var *var;

    var = (Var *)DATA_PTR(self);
    return mrb_fixnum_value(var->varattno);
}

void
psql_inspect_expr_class_init(mrb_state *mrb, struct RClass *class)
{
    /* Expr is base class */
    expr_class = mrb_define_class_under(mrb, class, "Expr", mrb->object_class);
    MRB_SET_INSTANCE_TT(expr_class, MRB_TT_DATA);

    mrb_define_method(mrb, expr_class, "initialize", psql_inspect_expr_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, expr_class, "type", psql_inspect_expr_type, MRB_ARGS_NONE());

    /* RelabelType class */
    relabel_type_class = mrb_define_class_under(mrb, class, "RelabelType", expr_class);
    MRB_SET_INSTANCE_TT(relabel_type_class, MRB_TT_DATA);

    mrb_define_method(mrb, relabel_type_class, "initialize", psql_inspect_relabel_type_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, relabel_type_class, "arg", psql_inspect_relabel_type_arg, MRB_ARGS_NONE());

    /* Var class */
    var_class = mrb_define_class_under(mrb, class, "Var", expr_class);
    MRB_SET_INSTANCE_TT(var_class, MRB_TT_DATA);

    mrb_define_method(mrb, var_class, "initialize", psql_inspect_var_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, var_class, "varno", psql_inspect_var_varno, MRB_ARGS_NONE());
    mrb_define_method(mrb, var_class, "varattno", psql_inspect_var_varattno, MRB_ARGS_NONE());

    /* Aggref class */
    aggref_class = mrb_define_class_under(mrb, class, "Aggref", expr_class);
    MRB_SET_INSTANCE_TT(aggref_class, MRB_TT_DATA);

    mrb_define_method(mrb, aggref_class, "initialize", psql_inspect_aggref_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, aggref_class, "args", psql_inspect_aggref_args, MRB_ARGS_NONE());

    /* Const class */
    const_class = mrb_define_class_under(mrb, class, "Const", expr_class);
    MRB_SET_INSTANCE_TT(const_class, MRB_TT_DATA);

    mrb_define_method(mrb, const_class, "initialize", psql_inspect_const_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, const_class, "consttype", psql_inspect_const_consttype, MRB_ARGS_NONE());
    mrb_define_method(mrb, const_class, "constlen", psql_inspect_const_constlen, MRB_ARGS_NONE());
    mrb_define_method(mrb, const_class, "constvalue", psql_inspect_const_constvalue, MRB_ARGS_NONE());

    /* TargetEntry class */
    target_entry_class = mrb_define_class_under(mrb, class, "TargetEntry", expr_class);
    MRB_SET_INSTANCE_TT(target_entry_class, MRB_TT_DATA);

    mrb_define_method(mrb, target_entry_class, "initialize", psql_inspect_target_entry_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, target_entry_class, "expr", psql_inspect_target_entry_expr, MRB_ARGS_NONE());
    mrb_define_method(mrb, target_entry_class, "resno", psql_inspect_target_entry_resno, MRB_ARGS_NONE());
    mrb_define_method(mrb, target_entry_class, "resname", psql_inspect_target_entry_resname, MRB_ARGS_NONE());
    mrb_define_method(mrb, target_entry_class, "resjunk", psql_inspect_target_entry_resjunk, MRB_ARGS_NONE());
}
