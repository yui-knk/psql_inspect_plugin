#include "postgres.h"
#include "nodes/relation.h"

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/variable.h>

#include <psql_inspect_nodes.h>
#include <psql_inspect_planner_info.h>
#include <psql_inspect_rel_opt_info.h>

static struct RClass *planner_info = NULL;
static const struct mrb_data_type psql_inspect_planner_info_data_type = { "PlannerInfo", mrb_free };

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
    DATA_TYPE(self) = &psql_inspect_planner_info_data_type;

    return self;
}

static mrb_value
psql_inspect_planner_info_build_from_planner_info(mrb_state *mrb, PlannerInfo *root)
{
    mrb_value val = mrb_class_new_instance(mrb, 0, NULL, planner_info);
    psql_inspect_planner_info_set_planner_info(mrb, val, root);

    return val;
}

static mrb_value
psql_inspect_planner_info_type(mrb_state *mrb, mrb_value self)
{
    PlannerInfo *root;

    root = (PlannerInfo *)DATA_PTR(self);
    return psql_inspect_mrb_str_from_NodeTag(mrb, root->type);
}

static mrb_value
psql_inspect_planner_info_parent_root(mrb_state *mrb, mrb_value self)
{
    PlannerInfo *root;

    root = (PlannerInfo *)DATA_PTR(self);

    if (root->parent_root == NULL) {
        return mrb_nil_value();
    } else {
        return psql_inspect_planner_info_build_from_planner_info(mrb, root->parent_root);
    }
}

static mrb_value
psql_inspect_planner_info_simple_rel_array(mrb_state *mrb, mrb_value self)
{
    PlannerInfo *root;
    int array_size, i;
    mrb_value ary;

    root = (PlannerInfo *)DATA_PTR(self);
    array_size = root->simple_rel_array_size;

    if (array_size == 0) {
        return mrb_ary_new(mrb);
    }

    ary = mrb_ary_new_capa(mrb, array_size);

    for (i = 0; i < array_size; i++) {
        RelOptInfo *rel = root->simple_rel_array[i];

        /* there may be empty slots corresponding to non-baserel RTEs */
        if (rel == NULL) {
            mrb_ary_set(mrb, ary, i, mrb_nil_value());
        } else {
            mrb_value v;

            v = psql_inspect_rel_opt_info_build_from_rel_opt_info(mrb, rel);
            mrb_ary_set(mrb, ary, i, v);
        }
    }

    return ary;
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
    mrb_define_method(mrb, planner_info, "type", psql_inspect_planner_info_type, MRB_ARGS_NONE());
    mrb_define_method(mrb, planner_info, "parent_root", psql_inspect_planner_info_parent_root, MRB_ARGS_NONE());
    mrb_define_method(mrb, planner_info, "simple_rel_array", psql_inspect_planner_info_simple_rel_array, MRB_ARGS_NONE());
}
