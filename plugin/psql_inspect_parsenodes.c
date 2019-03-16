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

static const struct mrb_data_type psql_inspect_raw_stmt_data_type = { "RawStmt", mrb_free };
static const struct mrb_data_type psql_inspect_select_stmt_data_type = { "SelectStmt", mrb_free };


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


void
psql_inspect_parsenodes_fini(mrb_state *mrb)
{
    raw_stmt_class = NULL;
    select_stmt_class = NULL;
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
    // mrb_define_method(mrb, select_stmt_class, "targetList", psql_inspect_select_stmt_targetList, MRB_ARGS_NONE());
    mrb_define_method(mrb, select_stmt_class, "fromClause", psql_inspect_select_stmt_fromClause, MRB_ARGS_NONE());
    mrb_define_method(mrb, select_stmt_class, "whereClause", psql_inspect_select_stmt_whereClause, MRB_ARGS_NONE());
}
