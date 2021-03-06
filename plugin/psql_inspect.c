#include "postgres.h"
#include "optimizer/paths.h"
#include "optimizer/planner.h"
#include "parser/analyze.h"
#include "utils/guc.h"
#include "utils/elog.h"
#include "executor/executor.h"

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/compile.h>
#include <mruby/string.h>

#include <psql_inspect_bitmapset.h>
#include <psql_inspect_nodes.h>
#include <psql_inspect_parse_state.h>
#include <psql_inspect_parsenodes.h>
#include <psql_inspect_path.h>
#include <psql_inspect_path_key.h>
#include <psql_inspect_plan.h>
#include <psql_inspect_planned_stmt.h>
#include <psql_inspect_planner_info.h>
#include <psql_inspect_primnodes.h>
#include <psql_inspect_query.h>
#include <psql_inspect_query_desc.h>
#include <psql_inspect_rel_opt_info.h>

PG_MODULE_MAGIC;

void _PG_init(void);
void _PG_fini(void);

/* This is called the end of parse analysis */
static post_parse_analyze_hook_type prev_post_parse_analyze_hook = NULL;
static planner_hook_type prev_planner_hook = NULL;
static set_rel_pathlist_hook_type prev_set_rel_pathlist_hook = NULL;
static ExecutorRun_hook_type prev_ExecutorRun_hook = NULL;

static mrb_state *mrb_s = NULL;

static const char *post_parse_analyze_script_guc_name = "psql_inspect.post_parse_analyze_script";
static const char *planner_script_guc_name = "psql_inspect.planner_script";
static const char *set_rel_pathlist_script_guc_name = "psql_inspect.set_rel_pathlist_script";
static const char *ExecutorRun_script_guc_name = "psql_inspect.ExecutorRun_script";

// static const char *script = "p [PgInspect::PlannedStmt.current_stmt.type, PgInspect::PlannedStmt.current_stmt.command_type]";

static char *
psql_inspect_get_script(const char *script_guc_name)
{
    return GetConfigOptionByName(script_guc_name, NULL, true);
}

static void
psql_inspect_mruby_log_backtrace(mrb_state *mrb, mrb_value obj)
{
    if (mrb_type(obj) == MRB_TT_EXCEPTION) {
        const mrb_value *p;
        const mrb_value *e;

        mrb_value bt = mrb_funcall(mrb, obj, "backtrace", 0);
        if (mrb_type(bt) != MRB_TT_ARRAY) {
            elog(WARNING, "backtrace must be Array. mrb_type=%d", mrb_type(bt));
            return;
        }

        p = RARRAY_PTR(bt);
        e = p + RARRAY_LEN(bt);

        while (p < e) {
          elog(WARNING, "%*s", RSTRING_LEN(*p), RSTRING_PTR(*p));
          p++;
        }
    }
}

static void
psql_inspect_mruby_error_handling(mrb_state *mrb)
{
    if (mrb->exc != NULL) {
        mrb_value exc, msg;

        exc = mrb_obj_value(mrb->exc);
        msg = mrb_funcall(mrb, exc, "inspect", 0, NULL);

        if (mrb_string_p(msg) && mrb_type(msg) == MRB_TT_STRING) {
            elog(WARNING, "Error is raised in mruby \"%s\"", mrb_string_value_ptr(mrb, msg));
            psql_inspect_mruby_log_backtrace(mrb, exc);
        } else {
            elog(WARNING, "Some error is raised in mruby but can not detect it...");
        }

        mrb->exc = NULL;
    }
}

static void
psql_inspect_mrb_load_string(mrb_state *mrb, const char *script)
{
    mrbc_context *ctx;

    ctx = mrbc_context_new(mrb);
    mrbc_filename(mrb, ctx, "psql_inspect");

    /* TODO: Is using mrb_rescue better? We can not touch original error in mrb_rescue */
    mrb_load_string_cxt(mrb, script, ctx);
    psql_inspect_mruby_error_handling(mrb);
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
    psql_inspect_mrb_load_string(mrb_s, script);
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
    psql_inspect_mrb_load_string(mrb_s, script);
    psql_inspect_planned_stmt_mruby_env_tear_down(mrb_s);

    return stmt;
}

static void
psql_inspect_post_parse_analyze_hook(ParseState *pstate, Query *query)
{
    const char *script;

    /* TODO: use elog debug */
    fprintf(stderr, "psql_inspect_post_parse_analyze_hook with mruby!\n");

    script = psql_inspect_get_script(post_parse_analyze_script_guc_name);

    if (script == NULL) {
        elog(LOG, "You should set \"%s\"", post_parse_analyze_script_guc_name);
        return;
    }

    psql_inspect_query_mruby_env_setup(mrb_s, query);
    psql_inspect_parse_state_mruby_env_setup(mrb_s, pstate);
    psql_inspect_mrb_load_string(mrb_s, script);
    psql_inspect_parse_state_mruby_env_tear_down(mrb_s);
    psql_inspect_query_mruby_env_tear_down(mrb_s);    
}

static void
psql_inspect_ExecutorRun_hook(QueryDesc *queryDesc, ScanDirection direction, uint64 count, bool execute_once)
{
    const char *script;

    /* TODO: use elog debug */
    fprintf(stderr, "psql_inspect_ExecutorRun_hook with mruby!\n");

    script = psql_inspect_get_script(ExecutorRun_script_guc_name);

    if (script == NULL) {
        elog(LOG, "You should set \"%s\"", ExecutorRun_script_guc_name);
        standard_ExecutorRun(queryDesc, direction, count, execute_once);
        return;
    }

    psql_inspect_query_desc_mruby_env_setup(mrb_s, queryDesc);
    psql_inspect_mrb_load_string(mrb_s, script);
    psql_inspect_query_desc_mruby_env_tear_down(mrb_s);

    standard_ExecutorRun(queryDesc, direction, count, execute_once);
}

static void
psql_inspect_class_init(mrb_state *mrb)
{
    struct RClass *class;

    class = mrb_define_class(mrb, "PgInspect", mrb->object_class);

    /* This should be first because other classes depend on psql_inspect_node_class */
    psql_inspect_node_class_init(mrb, class);

    psql_inspect_bitmapset_class_init(mrb, class);
    psql_inspect_planned_stmt_class_init(mrb, class);
    psql_inspect_planner_info_class_init(mrb, class);
    psql_inspect_rel_opt_info_class_init(mrb, class);
    psql_inspect_path_class_init(mrb, class);
    psql_inspect_path_key_class_init(mrb, class);
    psql_inspect_plan_class_init(mrb, class);
    psql_inspect_query_class_init(mrb, class);
    psql_inspect_query_desc_class_init(mrb, class);
    psql_inspect_parse_state_class_init(mrb, class);
    psql_inspect_primnodes_class_init(mrb, class);
    psql_inspect_parsenodes_class_init(mrb, class);
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

    /* parse_analyze_hook */
    prev_post_parse_analyze_hook = post_parse_analyze_hook;
    post_parse_analyze_hook = psql_inspect_post_parse_analyze_hook;

    /* planner_hook */
    prev_planner_hook = planner_hook;
    planner_hook = psql_inspect_planner_hook;

    /* set_rel_pathlist_hook */
    prev_set_rel_pathlist_hook = set_rel_pathlist_hook;
    set_rel_pathlist_hook = psql_inspect_set_rel_pathlist_hook;

    /* ExecutorRun_hook */
    prev_ExecutorRun_hook = ExecutorRun_hook;
    ExecutorRun_hook = psql_inspect_ExecutorRun_hook;
}

void
_PG_fini(void)
{
    fprintf(stderr, "psql_inspect is unloaded!\n");

    /* ExecutorRun_hook */
    if (ExecutorRun_hook == psql_inspect_ExecutorRun_hook) {
        ExecutorRun_hook = prev_ExecutorRun_hook;
    }

    /* set_rel_pathlist_hook */
    if (set_rel_pathlist_hook == psql_inspect_set_rel_pathlist_hook) {
        set_rel_pathlist_hook = prev_set_rel_pathlist_hook;
    }

    /* planner_hook */
    if (planner_hook == psql_inspect_planner_hook) {
        planner_hook = prev_planner_hook;
    }

    /* parse_analyze_hook */
    if (post_parse_analyze_hook == psql_inspect_post_parse_analyze_hook) {
        post_parse_analyze_hook = prev_post_parse_analyze_hook;
    }

    if (mrb_s != NULL) {
        psql_inspect_parsenodes_fini(mrb_s);
        psql_inspect_primnodes_fini(mrb_s);
        psql_inspect_parse_state_fini(mrb_s);
        psql_inspect_planned_stmt_fini(mrb_s);
        psql_inspect_planner_info_fini(mrb_s);
        psql_inspect_query_fini(mrb_s);
        psql_inspect_query_desc_fini(mrb_s);

        /* This should be last, see psql_inspect_class_init */
        psql_inspect_node_fini(mrb_s);

        mrb_close(mrb_s);
        mrb_s = NULL;
    }
}
