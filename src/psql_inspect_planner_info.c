#include "postgres.h"
#include "nodes/relation.h"

#include <mruby.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/variable.h>

#include <psql_inspect_nodes.h>
#include <psql_inspect_planner_info.h>

static struct RClass *planner_info = NULL;
static const struct mrb_data_type psql_inspect_planner_info_type = { "PlannerInfo", mrb_free };

static void
psql_inspect_planner_info_set_planner_info(mrb_state *mrb, mrb_value self, PlannerInfo *root)
{
    DATA_PTR(self) = root;
}

static mrb_value
psql_inspect_c_current_planner_info(mrb_state *mrb, mrb_value klass)
{
    return mrb_cv_get(mrb, klass, mrb_intern_lit(mrb, "planner_info"));
}

static mrb_value
psql_inspect_planner_info_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_planner_info_type;

    return self;
}

static mrb_value
psql_inspect_planned_type(mrb_state *mrb, mrb_value self)
{
    PlannerInfo *root;

    root = (PlannerInfo *)DATA_PTR(self);
    return psql_inspect_mrb_str_from_NodeTag(mrb, root->type);
}

void
psql_inspect_planner_info_mruby_env_setup(mrb_state *mrb, PlannerInfo *stmt)
{
    /* TODO: Ensure mrb and planner_info are set. */
    mrb_value obj = mrb_obj_new(mrb, planner_info, 0, NULL);
    psql_inspect_planner_info_set_planner_info(mrb, obj, stmt);
    mrb_mod_cv_set(mrb, planner_info, mrb_intern_lit(mrb, "planner_info"), obj);
}

void
psql_inspect_planner_info_mruby_env_tear_down(mrb_state *mrb)
{
    mrb_mod_cv_set(mrb, planner_info, mrb_intern_lit(mrb, "planner_info"), mrb_nil_value());
}

void
psql_inspect_planner_info_fini(mrb_state *mrb)
{
    planner_info = NULL;
}

void
psql_inspect_planner_info_class_init(mrb_state *mrb, struct RClass *class)
{
    planner_info = mrb_define_class_under(mrb, class, "PlannerInfo", mrb->object_class);
    MRB_SET_INSTANCE_TT(planner_info, MRB_TT_DATA);

    mrb_define_class_method(mrb, planner_info, "current_planner_info", psql_inspect_c_current_planner_info, MRB_ARGS_NONE());
    mrb_define_method(mrb, planner_info, "initialize", psql_inspect_planner_info_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, planner_info, "type", psql_inspect_planned_type, MRB_ARGS_NONE());
}
