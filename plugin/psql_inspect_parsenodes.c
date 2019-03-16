#include "postgres.h"
#include "nodes/nodes.h"
#include "nodes/pg_list.h"
#include "nodes/parsenodes.h"

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/data.h>

#include <psql_inspect_nodes.h>
#include <psql_inspect_parsenodes.h>

static struct RClass *raw_stmt_class = NULL;
static struct RClass *select_stmt_class = NULL;
static struct RClass *a_expr_class = NULL;
static struct RClass *a_const_class = NULL;
static struct RClass *column_ref_class = NULL;
static struct RClass *res_target_class = NULL;

static const struct mrb_data_type psql_inspect_raw_stmt_data_type = { "RawStmt", mrb_free };
static const struct mrb_data_type psql_inspect_select_stmt_data_type = { "SelectStmt", mrb_free };
static const struct mrb_data_type psql_inspect_a_expr_data_type = { "A_Expr", mrb_free };
static const struct mrb_data_type psql_inspect_a_const_data_type = { "A_Const", mrb_free };
static const struct mrb_data_type psql_inspect_column_ref_data_type = { "ColumnRef", mrb_free };
static const struct mrb_data_type psql_inspect_res_target_data_type = { "ResTarget", mrb_free };


static mrb_value
psql_inspect_raw_stmt_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_raw_stmt_data_type;

    return self;
}

static mrb_value
psql_inspect_select_stmt_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_select_stmt_data_type;

    return self;
}

static mrb_value
psql_inspect_a_expr_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_a_expr_data_type;

    return self;
}

static mrb_value
psql_inspect_a_const_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_a_const_data_type;

    return self;
}

static mrb_value
psql_inspect_column_ref_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_column_ref_data_type;

    return self;
}

static mrb_value
psql_inspect_res_target_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_res_target_data_type;

    return self;
}

mrb_value
psql_inspect_parsenode_build_from_node(mrb_state *mrb, Node *node)
{
    mrb_value val;

    switch (node->type) {
      case T_RawStmt:
        val = mrb_class_new_instance(mrb, 0, NULL, raw_stmt_class);
        break;
      case T_SelectStmt:
        val = mrb_class_new_instance(mrb, 0, NULL, select_stmt_class);
        break;
      case T_A_Expr:
        val = mrb_class_new_instance(mrb, 0, NULL, a_expr_class);
        break;
      case T_A_Const:
        val = mrb_class_new_instance(mrb, 0, NULL, a_const_class);
        break;
      case T_ColumnRef:
        val = mrb_class_new_instance(mrb, 0, NULL, column_ref_class);
        break;
      case T_ResTarget:
        val = mrb_class_new_instance(mrb, 0, NULL, res_target_class);
        break;
      default:
        val = mrb_class_new_instance(mrb, 0, NULL, psql_inspect_node_class);
    }

    DATA_PTR(val) = node;

    return val;
}

static mrb_value
psql_inspect_raw_stmt_stmt(mrb_state *mrb, mrb_value self)
{
    RawStmt *stmt;

    stmt = (RawStmt *)DATA_PTR(self);
    return psql_inspect_node_build_from_node(mrb, stmt->stmt);
}

static mrb_value
psql_inspect_select_stmt_targetList(mrb_state *mrb, mrb_value self)
{
    SelectStmt *stmt;
    int array_size;
    int i = 0;
    mrb_value ary;
    ListCell *lc;

    stmt = (SelectStmt *)DATA_PTR(self);
    array_size = list_length(stmt->targetList);
    ary = mrb_ary_new_capa(mrb, array_size);

    foreach(lc, stmt->targetList) {
        mrb_value v;
        Node *n = lfirst(lc);

        v = psql_inspect_node_build_from_node(mrb, n);
        mrb_ary_set(mrb, ary, i, v);
        i++;
    }

    return ary;
}

static mrb_value
psql_inspect_select_stmt_fromClause(mrb_state *mrb, mrb_value self)
{
    SelectStmt *stmt;
    int array_size;
    int i = 0;
    mrb_value ary;
    ListCell *lc;

    stmt = (SelectStmt *)DATA_PTR(self);
    array_size = list_length(stmt->fromClause);
    ary = mrb_ary_new_capa(mrb, array_size);

    foreach(lc, stmt->fromClause) {
        mrb_value v;
        Node *n = lfirst(lc);

        v = psql_inspect_node_build_from_node(mrb, n);
        mrb_ary_set(mrb, ary, i, v);
        i++;
    }

    return ary;
}

static mrb_value
psql_inspect_select_stmt_whereClause(mrb_state *mrb, mrb_value self)
{
    SelectStmt *stmt;

    stmt = (SelectStmt *)DATA_PTR(self);

    if (stmt->whereClause == NULL) {
        return mrb_nil_value();
    }

    return psql_inspect_node_build_from_node(mrb, stmt->whereClause);
}

static mrb_value
psql_inspect_mrb_str_from_A_Expr_Kind(mrb_state *mrb, A_Expr_Kind kind)
{

#define AEXPR(kind) \
    AEXPR_##kind: return mrb_str_new_cstr(mrb, #kind);

    switch (kind) {
      case AEXPR(OP);
      case AEXPR(OP_ANY);
      case AEXPR(OP_ALL);
      case AEXPR(DISTINCT);
      case AEXPR(NOT_DISTINCT);
      case AEXPR(NULLIF);
      case AEXPR(OF);
      case AEXPR(IN);
      case AEXPR(LIKE);
      case AEXPR(ILIKE);
      case AEXPR(SIMILAR);
      case AEXPR(BETWEEN);
      case AEXPR(NOT_BETWEEN);
      case AEXPR(BETWEEN_SYM);
      case AEXPR(NOT_BETWEEN_SYM);
      case AEXPR(PAREN);
#undef AEXPR
      default:
        mrb_raisef(mrb, E_RUNTIME_ERROR, "Unknown command kind number: %S", mrb_fixnum_value(kind));
    }
}

static mrb_value
psql_inspect_a_expr_kind(mrb_state *mrb, mrb_value self)
{
    A_Expr *expr;

    expr = (A_Expr *)DATA_PTR(self);
    return psql_inspect_mrb_str_from_A_Expr_Kind(mrb, expr->kind);
}

/* See: "nodes/value.h" and "nodes/value.c" */
static mrb_value
psql_inspect_value_to_mrb_value(mrb_state *mrb, Value *val)
{
    switch (val->type) {
      case T_Integer:
        return mrb_fixnum_value(intVal(val));
      case T_Float:
        return mrb_float_value(mrb, floatVal(val));
      case T_String: /* fallthrough */
      case T_BitString:
        return mrb_str_new_cstr(mrb, strVal(val));
      default:
        mrb_raisef(mrb, E_RUNTIME_ERROR, "Unknown Value node tag number: %S", mrb_fixnum_value(val->type));
    }
}

static mrb_value
psql_inspect_a_expr_name(mrb_state *mrb, mrb_value self)
{
    A_Expr *expr;
    int array_size;
    int i = 0;
    mrb_value ary;
    ListCell *lc;

    expr = (A_Expr *)DATA_PTR(self);
    array_size = list_length(expr->name);
    ary = mrb_ary_new_capa(mrb, array_size);

    foreach(lc, expr->name) {
        mrb_value v;
        Value *val = (Value *) lfirst(lc);

        v = psql_inspect_value_to_mrb_value(mrb, val);
        mrb_ary_set(mrb, ary, i, v);
        i++;
    }

    return ary;
}

static mrb_value
psql_inspect_a_expr_lexpr(mrb_state *mrb, mrb_value self)
{
    A_Expr *expr;

    expr = (A_Expr *)DATA_PTR(self);

    if (expr->lexpr == NULL) {
        return mrb_nil_value();
    }

    return psql_inspect_node_build_from_node(mrb, expr->lexpr);
}

static mrb_value
psql_inspect_a_expr_rexpr(mrb_state *mrb, mrb_value self)
{
    A_Expr *expr;

    expr = (A_Expr *)DATA_PTR(self);

    if (expr->rexpr == NULL) {
        return mrb_nil_value();
    }

    return psql_inspect_node_build_from_node(mrb, expr->rexpr);
}

static mrb_value
psql_inspect_a_expr_location(mrb_state *mrb, mrb_value self)
{
    A_Expr *expr;

    expr = (A_Expr *)DATA_PTR(self);
    return mrb_fixnum_value(expr->location);
}

static mrb_value
psql_inspect_a_const_val(mrb_state *mrb, mrb_value self)
{
    A_Const *con;

    con = (A_Const *)DATA_PTR(self);
    return psql_inspect_value_to_mrb_value(mrb, &con->val);
}

static mrb_value
psql_inspect_a_const_location(mrb_state *mrb, mrb_value self)
{
    A_Const *con;

    con = (A_Const *)DATA_PTR(self);
    return mrb_fixnum_value(con->location);
}

static mrb_value
psql_inspect_column_ref_fields(mrb_state *mrb, mrb_value self)
{
    ColumnRef *cref;
    int array_size;
    int i = 0;
    mrb_value ary;
    ListCell *lc;

    cref = (ColumnRef *)DATA_PTR(self);
    array_size = list_length(cref->fields);
    ary = mrb_ary_new_capa(mrb, array_size);

    foreach(lc, cref->fields) {
        mrb_value v;
        Node *node = (Node *) lfirst(lc);

        if (node->type == T_A_Star) {
            v = mrb_str_new_lit(mrb, "*");
        }
        else {
            v = psql_inspect_value_to_mrb_value(mrb, (Value *)node);
        }

        mrb_ary_set(mrb, ary, i, v);
        i++;
    }

    return ary;
}

static mrb_value
psql_inspect_column_ref_location(mrb_state *mrb, mrb_value self)
{
    ColumnRef *cref;

    cref = (ColumnRef *)DATA_PTR(self);
    return mrb_fixnum_value(cref->location);
}

static mrb_value
psql_inspect_res_target_name(mrb_state *mrb, mrb_value self)
{
    ResTarget *res;

    res = (ResTarget *)DATA_PTR(self);

    if (res->name == NULL) {
        return mrb_nil_value();
    }

    return mrb_str_new_cstr(mrb, res->name);
}

static mrb_value
psql_inspect_res_target_indirection(mrb_state *mrb, mrb_value self)
{
    ResTarget *res;
    int array_size;
    int i = 0;
    mrb_value ary;
    ListCell *lc;

    res = (ResTarget *)DATA_PTR(self);
    array_size = list_length(res->indirection);
    ary = mrb_ary_new_capa(mrb, array_size);

    foreach(lc, res->indirection) {
        mrb_value v;
        Node *node = (Node *) lfirst(lc);

        v = psql_inspect_node_build_from_node(mrb, node);
        mrb_ary_set(mrb, ary, i, v);
        i++;
    }

    return ary;
}

static mrb_value
psql_inspect_res_target_val(mrb_state *mrb, mrb_value self)
{
    ResTarget *res;

    res = (ResTarget *)DATA_PTR(self);

    if (res->val == NULL) {
        return mrb_nil_value();
    }

    return psql_inspect_node_build_from_node(mrb, res->val);
}

static mrb_value
psql_inspect_res_target_location(mrb_state *mrb, mrb_value self)
{
    ResTarget *res;

    res = (ResTarget *)DATA_PTR(self);
    return mrb_fixnum_value(res->location);
}


void
psql_inspect_parsenodes_fini(mrb_state *mrb)
{
    raw_stmt_class = NULL;
    select_stmt_class = NULL;
    a_expr_class = NULL;
    a_const_class = NULL;
}

void
psql_inspect_parsenodes_class_init(mrb_state *mrb, struct RClass *class)
{
    /* RawStmt class */
    raw_stmt_class = mrb_define_class_under(mrb, class, "RawStmt", psql_inspect_node_class);
    MRB_SET_INSTANCE_TT(raw_stmt_class, MRB_TT_DATA);
    mrb_define_method(mrb, raw_stmt_class, "initialize", psql_inspect_raw_stmt_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, raw_stmt_class, "stmt", psql_inspect_raw_stmt_stmt, MRB_ARGS_NONE());

    /* SelectStmt class */
    select_stmt_class = mrb_define_class_under(mrb, class, "SelectStmt", psql_inspect_node_class);
    MRB_SET_INSTANCE_TT(select_stmt_class, MRB_TT_DATA);
    mrb_define_method(mrb, select_stmt_class, "initialize", psql_inspect_select_stmt_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, select_stmt_class, "targetList", psql_inspect_select_stmt_targetList, MRB_ARGS_NONE());
    mrb_define_method(mrb, select_stmt_class, "fromClause", psql_inspect_select_stmt_fromClause, MRB_ARGS_NONE());
    mrb_define_method(mrb, select_stmt_class, "whereClause", psql_inspect_select_stmt_whereClause, MRB_ARGS_NONE());

    /* A_Expr class */
    a_expr_class = mrb_define_class_under(mrb, class, "A_Expr", psql_inspect_node_class);
    MRB_SET_INSTANCE_TT(a_expr_class, MRB_TT_DATA);
    mrb_define_method(mrb, a_expr_class, "initialize", psql_inspect_a_expr_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, a_expr_class, "kind", psql_inspect_a_expr_kind, MRB_ARGS_NONE());
    mrb_define_method(mrb, a_expr_class, "name", psql_inspect_a_expr_name, MRB_ARGS_NONE());
    mrb_define_method(mrb, a_expr_class, "lexpr", psql_inspect_a_expr_lexpr, MRB_ARGS_NONE());
    mrb_define_method(mrb, a_expr_class, "rexpr", psql_inspect_a_expr_rexpr, MRB_ARGS_NONE());
    mrb_define_method(mrb, a_expr_class, "location", psql_inspect_a_expr_location, MRB_ARGS_NONE());

    /* A_Const class */
    a_const_class = mrb_define_class_under(mrb, class, "A_Const", psql_inspect_node_class);
    MRB_SET_INSTANCE_TT(a_const_class, MRB_TT_DATA);
    mrb_define_method(mrb, a_const_class, "initialize", psql_inspect_a_const_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, a_const_class, "val", psql_inspect_a_const_val, MRB_ARGS_NONE());
    mrb_define_method(mrb, a_const_class, "location", psql_inspect_a_const_location, MRB_ARGS_NONE());

    /* ColumnRef class */
    column_ref_class = mrb_define_class_under(mrb, class, "ColumnRef", psql_inspect_node_class);
    MRB_SET_INSTANCE_TT(column_ref_class, MRB_TT_DATA);
    mrb_define_method(mrb, column_ref_class, "initialize", psql_inspect_column_ref_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, column_ref_class, "fields", psql_inspect_column_ref_fields, MRB_ARGS_NONE());
    mrb_define_method(mrb, column_ref_class, "location", psql_inspect_column_ref_location, MRB_ARGS_NONE());

    /* ResTarget class */
    res_target_class = mrb_define_class_under(mrb, class, "ResTarget", psql_inspect_node_class);
    MRB_SET_INSTANCE_TT(res_target_class, MRB_TT_DATA);
    mrb_define_method(mrb, res_target_class, "initialize", psql_inspect_res_target_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, res_target_class, "name", psql_inspect_res_target_name, MRB_ARGS_NONE());
    mrb_define_method(mrb, res_target_class, "indirection", psql_inspect_res_target_indirection, MRB_ARGS_NONE());
    mrb_define_method(mrb, res_target_class, "val", psql_inspect_res_target_val, MRB_ARGS_NONE());
    mrb_define_method(mrb, res_target_class, "location", psql_inspect_res_target_location, MRB_ARGS_NONE());
}
