#include "postgres.h"
#include "optimizer/planner.h"

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/variable.h>

#include <psql_inspect_nodes.h>
#include <psql_inspect_plan.h>
#include <psql_inspect_planned_stmt.h>
#include <psql_inspect_query.h>

static struct RClass *class_stmt = NULL;
static const struct mrb_data_type psql_inspect_planned_stmt_data_type = { "PlannedStmt", mrb_free };

static void
psql_inspect_planned_stmt_set_planned_stmt(mrb_state *mrb, mrb_value self, PlannedStmt *stmt)
{
    DATA_PTR(self) = stmt;
}


static mrb_value
psql_inspect_c_current_stmt(mrb_state *mrb, mrb_value klass)
{
    return mrb_cv_get(mrb, klass, mrb_intern_lit(mrb, "stmt"));
}

static mrb_value
psql_inspect_planned_stmt_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_planned_stmt_data_type;

    return self;
}

static mrb_value
psql_inspect_planned_stmt_command_type(mrb_state *mrb, mrb_value self)
{
    PlannedStmt *stmt;

    stmt = (PlannedStmt *)DATA_PTR(self);
    return psql_inspect_mrb_str_from_CmdType(mrb, stmt->commandType);
}

static mrb_value
psql_inspect_planned_stmt_plan_tree(mrb_state *mrb, mrb_value self)
{
    PlannedStmt *stmt;

    stmt = (PlannedStmt *)DATA_PTR(self);
    return psql_inspect_plan_build_from_plan(mrb, stmt->planTree);
}

static mrb_value
psql_inspect_planned_stmt_rtable(mrb_state *mrb, mrb_value self)
{
    PlannedStmt *stmt;

    int array_size;
    int i = 0;
    mrb_value ary;
    ListCell *lc;

    stmt = (PlannedStmt *)DATA_PTR(self);
    array_size = list_length(stmt->rtable);
    ary = mrb_ary_new_capa(mrb, array_size);

    foreach(lc, stmt->rtable) {
        mrb_value v;
        RangeTblEntry *rte = (RangeTblEntry *) lfirst(lc);

        v = psql_inspect_range_tbl_entry_build_from_rte(mrb, rte);
        mrb_ary_set(mrb, ary, i, v);
        i++;
    }

    return ary;
}

void
psql_inspect_planned_stmt_mruby_env_setup(mrb_state *mrb, PlannedStmt *stmt)
{
    /* TODO: Ensure mrb and class_stmt are set. */
    mrb_value obj = mrb_obj_new(mrb, class_stmt, 0, NULL);
    psql_inspect_planned_stmt_set_planned_stmt(mrb, obj, stmt);
    mrb_mod_cv_set(mrb, class_stmt, mrb_intern_lit(mrb, "stmt"), obj);
}

void
psql_inspect_planned_stmt_mruby_env_tear_down(mrb_state *mrb)
{
    mrb_mod_cv_set(mrb, class_stmt, mrb_intern_lit(mrb, "stmt"), mrb_nil_value());
}

void
psql_inspect_planned_stmt_fini(mrb_state *mrb)
{
    class_stmt = NULL;
}

void
psql_inspect_planned_stmt_class_init(mrb_state *mrb, struct RClass *class)
{
    class_stmt = mrb_define_class_under(mrb, class, "PlannedStmt", psql_inspect_node_class);
    MRB_SET_INSTANCE_TT(class_stmt, MRB_TT_DATA);

    mrb_define_class_method(mrb, class_stmt, "current_stmt", psql_inspect_c_current_stmt, MRB_ARGS_NONE());
    mrb_define_method(mrb, class_stmt, "initialize", psql_inspect_planned_stmt_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, class_stmt, "command_type", psql_inspect_planned_stmt_command_type, MRB_ARGS_NONE());
    mrb_define_method(mrb, class_stmt, "plan_tree", psql_inspect_planned_stmt_plan_tree, MRB_ARGS_NONE());
    mrb_define_method(mrb, class_stmt, "rtable", psql_inspect_planned_stmt_rtable, MRB_ARGS_NONE());
}
