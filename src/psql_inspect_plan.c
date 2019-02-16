#include "postgres.h"
#include "nodes/relation.h"

#include <mruby.h>
#include <mruby/class.h>
#include <mruby/data.h>

#include <psql_inspect_nodes.h>
#include <psql_inspect_plan.h>

static struct RClass *plan_class = NULL;
static struct RClass *agg_class = NULL;
static struct RClass *seq_scan_class = NULL;

static const struct mrb_data_type psql_inspect_plan_data_type = { "Plan", mrb_free };
static const struct mrb_data_type psql_inspect_agg_data_type = { "Agg", mrb_free };
static const struct mrb_data_type psql_inspect_seq_scan_data_type = { "SeqScan", mrb_free };

static void
psql_inspect_plan_set_plan(mrb_state *mrb, mrb_value self, Plan *plan)
{
    DATA_PTR(self) = plan;
}

static mrb_value
psql_inspect_plan_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_plan_data_type;

    return self;
}

static mrb_value
psql_inspect_agg_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_agg_data_type;

    return self;
}

static mrb_value
psql_inspect_seq_scan_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_seq_scan_data_type;

    return self;
}

mrb_value
psql_inspect_plan_build_from_plan(mrb_state *mrb, Plan *plan)
{
    mrb_value val;

    switch (plan->type) {
      case T_Agg:
        val = mrb_class_new_instance(mrb, 0, NULL, agg_class);
        break;
      case T_SeqScan:
        val = mrb_class_new_instance(mrb, 0, NULL, seq_scan_class);
        break;
      default:
        val = mrb_class_new_instance(mrb, 0, NULL, plan_class);
    }

    psql_inspect_plan_set_plan(mrb, val, plan);

    return val;
}

static mrb_value
psql_inspect_plan_type(mrb_state *mrb, mrb_value self)
{
    Plan *plan;

    plan = (Plan *)DATA_PTR(self);
    return psql_inspect_mrb_str_from_NodeTag(mrb, plan->type);
}

static mrb_value
psql_inspect_plan_lefttree(mrb_state *mrb, mrb_value self)
{
    Plan *plan;

    plan = (Plan *)DATA_PTR(self);

    /* Could this be nil? */
    if (plan->lefttree == NULL) {
        return mrb_nil_value();
    }

    return psql_inspect_plan_build_from_plan(mrb, plan->lefttree);
}

static mrb_value
psql_inspect_plan_righttree(mrb_state *mrb, mrb_value self)
{
    Plan *plan;

    plan = (Plan *)DATA_PTR(self);

    if (plan->righttree == NULL) {
        return mrb_nil_value();
    }

    return psql_inspect_plan_build_from_plan(mrb, plan->righttree);
}

void
psql_inspect_plan_class_init(mrb_state *mrb, struct RClass *class)
{
    /* Plan is base class */
    plan_class = mrb_define_class_under(mrb, class, "Plan", mrb->object_class);
    MRB_SET_INSTANCE_TT(plan_class, MRB_TT_DATA);

    mrb_define_method(mrb, plan_class, "initialize", psql_inspect_plan_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, plan_class, "type", psql_inspect_plan_type, MRB_ARGS_NONE());
    mrb_define_method(mrb, plan_class, "lefttree", psql_inspect_plan_lefttree, MRB_ARGS_NONE());
    mrb_define_method(mrb, plan_class, "righttree", psql_inspect_plan_righttree, MRB_ARGS_NONE());

    /* Agg class */
    agg_class = mrb_define_class_under(mrb, class, "Agg", plan_class);
    MRB_SET_INSTANCE_TT(agg_class, MRB_TT_DATA);

    mrb_define_method(mrb, agg_class, "initialize", psql_inspect_agg_init, MRB_ARGS_NONE());

    /* SeqScan class */
    seq_scan_class = mrb_define_class_under(mrb, class, "SeqScan", plan_class);
    MRB_SET_INSTANCE_TT(seq_scan_class, MRB_TT_DATA);

    mrb_define_method(mrb, seq_scan_class, "initialize", psql_inspect_seq_scan_init, MRB_ARGS_NONE());
}
