#include "postgres.h"
#include "utils/guc.h"
#include "utils/elog.h"
#include "optimizer/planner.h"

#include <mruby.h>
#include <mruby/class.h>
#include <mruby/compile.h>
#include <mruby/data.h>
#include <mruby/variable.h>

#include <psql_inspect_nodes.h>

PG_MODULE_MAGIC;

void _PG_init(void);
void _PG_fini(void);

static planner_hook_type prev_planner_hook = NULL;

static mrb_state *mrb_s = NULL;
static struct RClass *class_stmt = NULL;

static void psql_inspect_planned_stmt_set_planned_stmt(mrb_state *mrb, mrb_value self, PlannedStmt *stmt);

static const struct mrb_data_type psql_inspect_planned_stmt_type = { "PlannedStmt", mrb_free };
static const char *script_guc_name = "psql_inspect.script";

// static const char *script = "p [PgInspect::PlannedStmt.current_stmt.type, PgInspect::PlannedStmt.current_stmt.command_type]";

/* Setup pg related object. */
static void
psql_inspect_mruby_env_setup(mrb_state *mrb, PlannedStmt *stmt)
{
    /* TODO: Ensure mrb and class_stmt are set. */
    mrb_value obj = mrb_obj_new(mrb, class_stmt, 0, NULL);
    psql_inspect_planned_stmt_set_planned_stmt(mrb, obj, stmt);
    mrb_mod_cv_set(mrb, class_stmt, mrb_intern_lit(mrb, "stmt"), obj);
}

static void
psql_inspect_mruby_env_tear_down(mrb_state *mrb)
{
    mrb_mod_cv_set(mrb, class_stmt, mrb_intern_lit(mrb, "stmt"), mrb_nil_value());
}

static char *
psql_inspect_get_script(void)
{
    return GetConfigOptionByName(script_guc_name, NULL, true);
}

static PlannedStmt *
psql_inspect_planner(Query *parse, int cursorOptions, ParamListInfo boundParams)
{
    const char *script;
    PlannedStmt *stmt = standard_planner(parse, cursorOptions, boundParams);

    fprintf(stderr, "psql_inspect_planner with mruby!\n");

    psql_inspect_mruby_env_setup(mrb_s, stmt);
    script = psql_inspect_get_script();

    if (script == NULL) {
        /* TODO: Warning */
        elog(WARNING, "You should set \"psql_inspect.script\"");
        return stmt;
    }

    mrb_load_string(mrb_s, script);
    psql_inspect_mruby_env_tear_down(mrb_s);
    /* TODO: Handling mruby exception */

    return stmt;
}

static mrb_value
psql_inspect_planned_stmt_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_planned_stmt_type;

    return self;
}

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
psql_inspect_planned_type(mrb_state *mrb, mrb_value self)
{
    PlannedStmt *stmt;

    stmt = (PlannedStmt *)DATA_PTR(self);
    return psql_inspect_mrb_str_from_NodeTag(mrb, stmt->type);
}

static mrb_value
psql_inspect_planned_command_type(mrb_state *mrb, mrb_value self)
{
    PlannedStmt *stmt;

    stmt = (PlannedStmt *)DATA_PTR(self);
    return psql_inspect_mrb_str_from_CmdType(mrb, stmt->commandType);
}

static void
psql_inspect_planned_stmt_class_init(mrb_state *mrb, struct RClass *class)
{
    class_stmt = mrb_define_class_under(mrb, class, "PlannedStmt", mrb->object_class);
    MRB_SET_INSTANCE_TT(class_stmt, MRB_TT_DATA);

    mrb_define_class_method(mrb, class_stmt, "current_stmt", psql_inspect_c_current_stmt, MRB_ARGS_NONE());
    mrb_define_method(mrb, class_stmt, "initialize", psql_inspect_planned_stmt_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, class_stmt, "type", psql_inspect_planned_type, MRB_ARGS_NONE());
    mrb_define_method(mrb, class_stmt, "command_type", psql_inspect_planned_command_type, MRB_ARGS_NONE());
}

static void
psql_inspect_class_init(mrb_state *mrb)
{
    struct RClass *class;

    class = mrb_define_class(mrb, "PgInspect", mrb->object_class);

    psql_inspect_planned_stmt_class_init(mrb, class);
}

/*
 * Module load callback
 */
void
_PG_init(void)
{
    fprintf(stderr, "psql_inspect is loaded!\n");

    mrb_s = mrb_open();
    psql_inspect_class_init(mrb_s);

    prev_planner_hook = planner_hook;
    planner_hook = psql_inspect_planner;
}

void
_PG_fini(void)
{
    fprintf(stderr, "psql_inspect is unloaded!\n");

    if (planner_hook == psql_inspect_planner) {
        mrb_close(mrb_s);
        mrb_s = NULL;
        class_stmt = NULL;
        planner_hook = prev_planner_hook;
    }
}
