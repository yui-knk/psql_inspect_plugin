#include "postgres.h"
#include "optimizer/paths.h"
#include "optimizer/planner.h"
#include "utils/guc.h"
#include "utils/elog.h"

#include <mruby.h>
#include <mruby/compile.h>
#include <mruby/string.h>

#include <psql_inspect_bitmapset.h>
#include <psql_inspect_expr.h>
#include <psql_inspect_path.h>
#include <psql_inspect_path_key.h>
#include <psql_inspect_plan.h>
#include <psql_inspect_planned_stmt.h>
#include <psql_inspect_planner_info.h>
#include <psql_inspect_rel_opt_info.h>

PG_MODULE_MAGIC;

void _PG_init(void);
void _PG_fini(void);

static planner_hook_type prev_planner_hook = NULL;
static set_rel_pathlist_hook_type prev_set_rel_pathlist_hook = NULL;

static mrb_state *mrb_s = NULL;

static const char *planner_script_guc_name = "psql_inspect.planner_script";
static const char *set_rel_pathlist_script_guc_name = "psql_inspect.set_rel_pathlist_script";

// static const char *script = "p [PgInspect::PlannedStmt.current_stmt.type, PgInspect::PlannedStmt.current_stmt.command_type]";

static char *
psql_inspect_get_script(const char *script_guc_name)
{
    return GetConfigOptionByName(script_guc_name, NULL, true);
}

static void
psql_inspect_mruby_error_handling(mrb_state *mrb)
{
    if (mrb->exc != NULL) {
        mrb_value msg;
        msg = mrb_funcall(mrb, mrb_obj_value(mrb->exc), "message", 0, NULL);

        if (mrb_string_p(msg)) {
            elog(WARNING, "Error is raised in mruby \"%s\"", mrb_string_value_ptr(mrb, msg));
        } else {
            elog(WARNING, "Some error is raised in mruby but can not detect it...");
        }

        mrb->exc = NULL;
    }
}

static void
psql_inspect_set_rel_pathlist_hook(PlannerInfo *root, RelOptInfo *rel, Index rti, RangeTblEntry *rte)
{
    const char *script;

    /* TODO: use elog debug */
    fprintf(stderr, "psql_inspect_set_rel_pathlist_hook with mruby!\n");

    script = psql_inspect_get_script(set_rel_pathlist_script_guc_name);

    if (script == NULL) {
        elog(LOG, "You should set \"%s\"", set_rel_pathlist_script_guc_name);
        return;
    }

    psql_inspect_planner_info_mruby_env_setup(mrb_s, root);

    /* TODO: Is using mrb_rescue better? We can not touch original error in mrb_rescue */
    mrb_load_string(mrb_s, script);
    psql_inspect_mruby_error_handling(mrb_s);

    psql_inspect_planner_info_mruby_env_tear_down(mrb_s);
}

static PlannedStmt *
psql_inspect_planner_hook(Query *parse, int cursorOptions, ParamListInfo boundParams)
{
    const char *script;
    PlannedStmt *stmt = standard_planner(parse, cursorOptions, boundParams);

    /* TODO: use elog debug */
    fprintf(stderr, "psql_inspect_planner_hook with mruby!\n");

    script = psql_inspect_get_script(planner_script_guc_name);

    if (script == NULL) {
        elog(LOG, "You should set \"%s\"", planner_script_guc_name);
        return stmt;
    }

    psql_inspect_planned_stmt_mruby_env_setup(mrb_s, stmt);

    mrb_load_string(mrb_s, script);
    psql_inspect_mruby_error_handling(mrb_s);

    psql_inspect_planned_stmt_mruby_env_tear_down(mrb_s);

    return stmt;
}

static void
psql_inspect_class_init(mrb_state *mrb)
{
    struct RClass *class;

    class = mrb_define_class(mrb, "PgInspect", mrb->object_class);

    psql_inspect_bitmapset_class_init(mrb, class);
    psql_inspect_planned_stmt_class_init(mrb, class);
    psql_inspect_planner_info_class_init(mrb, class);
    psql_inspect_rel_opt_info_class_init(mrb, class);
    psql_inspect_path_class_init(mrb, class);
    psql_inspect_path_key_class_init(mrb, class);
    psql_inspect_expr_class_init(mrb, class);
    psql_inspect_plan_class_init(mrb, class);
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

    /* planner_hook */
    prev_planner_hook = planner_hook;
    planner_hook = psql_inspect_planner_hook;

    /* set_rel_pathlist_hook */
    prev_set_rel_pathlist_hook = set_rel_pathlist_hook;
    set_rel_pathlist_hook = psql_inspect_set_rel_pathlist_hook;
}

void
_PG_fini(void)
{
    fprintf(stderr, "psql_inspect is unloaded!\n");

    /* set_rel_pathlist_hook */
    if (set_rel_pathlist_hook == psql_inspect_set_rel_pathlist_hook) {
        set_rel_pathlist_hook = prev_set_rel_pathlist_hook;
    }

    /* planner_hook */
    if (planner_hook == psql_inspect_planner_hook) {
        planner_hook = prev_planner_hook;
    }

    if (mrb_s != NULL) {
        psql_inspect_planned_stmt_fini(mrb_s);
        psql_inspect_planner_info_fini(mrb_s);

        mrb_close(mrb_s);
        mrb_s = NULL;
    }
}
