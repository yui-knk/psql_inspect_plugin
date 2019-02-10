#include "postgres.h"
#include "utils/guc.h"
#include "utils/elog.h"

#include <mruby.h>
#include <mruby/compile.h>

#include <psql_inspect_planned_stmt.h>

PG_MODULE_MAGIC;

void _PG_init(void);
void _PG_fini(void);

static planner_hook_type prev_planner_hook = NULL;

static mrb_state *mrb_s = NULL;

static const char *script_guc_name = "psql_inspect.script";

// static const char *script = "p [PgInspect::PlannedStmt.current_stmt.type, PgInspect::PlannedStmt.current_stmt.command_type]";

/* Setup pg related object. */
static void
psql_inspect_mruby_env_setup(mrb_state *mrb, PlannedStmt *stmt)
{
    psql_inspect_planned_stmt_mruby_env_setup(mrb, stmt);
}

static void
psql_inspect_mruby_env_tear_down(mrb_state *mrb)
{
    psql_inspect_planned_stmt_mruby_env_tear_down(mrb);
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
        elog(WARNING, "You should set \"psql_inspect.script\"");
        return stmt;
    }

    mrb_load_string(mrb_s, script);
    psql_inspect_mruby_env_tear_down(mrb_s);
    /* TODO: Handling mruby exception */

    return stmt;
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
        psql_inspect_planned_stmt_fini(mrb_s);

        mrb_close(mrb_s);
        mrb_s = NULL;
        planner_hook = prev_planner_hook;
    }
}
