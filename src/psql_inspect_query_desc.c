#include "postgres.h"
#include "executor/execdesc.h"
#include "nodes/execnodes.h"

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/variable.h>

#include <psql_inspect_nodes.h>
#include <psql_inspect_query_desc.h>

static struct RClass *query_desc_class = NULL;

static struct RClass *plan_state_class = NULL;
static struct RClass *agg_state_class = NULL;


static const struct mrb_data_type psql_inspect_query_desc_data_type = { "QueryDesc", mrb_free };

static const struct mrb_data_type psql_inspect_plan_state_data_type = { "PlanState", mrb_free };
static const struct mrb_data_type psql_inspect_agg_state_data_type = { "AggState", mrb_free };

static void
psql_inspect_planstate_set_planstate(mrb_state *mrb, mrb_value self, PlanState *planstate)
{
    DATA_PTR(self) = planstate;
}

static mrb_value
psql_inspect_planstate_build_from_planstate(mrb_state *mrb, PlanState *planstate)
{
    mrb_value val;

    switch (planstate->type) {
      case T_AggState:
        val = mrb_class_new_instance(mrb, 0, NULL, agg_state_class);
        break;
      default:
        val = mrb_class_new_instance(mrb, 0, NULL, plan_state_class);
    }

    psql_inspect_planstate_set_planstate(mrb, val, planstate);

    return val;
}

static void
psql_inspect_query_desc_set_query_desc(mrb_state *mrb, mrb_value self, QueryDesc *queryDesc)
{
    DATA_PTR(self) = queryDesc;
}

static mrb_value
psql_inspect_c_current_query_desc(mrb_state *mrb, mrb_value klass)
{
    return mrb_cv_get(mrb, klass, mrb_intern_lit(mrb, "query_desc"));
}

static mrb_value
psql_inspect_query_desc_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_query_desc_data_type;

    return self;
}

static mrb_value
psql_inspect_plan_state_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_plan_state_data_type;

    return self;
}

static mrb_value
psql_inspect_agg_state_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_agg_state_data_type;

    return self;
}

static mrb_value
psql_inspect_query_desc_operation(mrb_state *mrb, mrb_value self)
{
    QueryDesc *queryDesc;

    queryDesc = (QueryDesc *)DATA_PTR(self);
    return psql_inspect_mrb_str_from_CmdType(mrb, queryDesc->operation);
}

static mrb_value
psql_inspect_query_desc_planstate(mrb_state *mrb, mrb_value self)
{
    QueryDesc *queryDesc;

    queryDesc = (QueryDesc *)DATA_PTR(self);
    return psql_inspect_planstate_build_from_planstate(mrb, queryDesc->planstate);
}

static mrb_value
psql_inspect_plan_state_type(mrb_state *mrb, mrb_value self)
{
    Plan *plan;

    plan = (Plan *)DATA_PTR(self);
    return psql_inspect_mrb_str_from_NodeTag(mrb, plan->type);
}

static mrb_value
psql_inspect_agg_state_numaggs(mrb_state *mrb, mrb_value self)
{
    AggState *aggstate;

    aggstate = (AggState *)DATA_PTR(self);
    return mrb_fixnum_value(aggstate->numaggs);
}

static mrb_value
psql_inspect_agg_state_numtrans(mrb_state *mrb, mrb_value self)
{
    AggState *aggstate;

    aggstate = (AggState *)DATA_PTR(self);
    return mrb_fixnum_value(aggstate->numtrans);
}

static mrb_value
psql_inspect_agg_state_numphases(mrb_state *mrb, mrb_value self)
{
    AggState *aggstate;

    aggstate = (AggState *)DATA_PTR(self);
    return mrb_fixnum_value(aggstate->numphases);
}

static mrb_value
psql_inspect_agg_state_current_phase(mrb_state *mrb, mrb_value self)
{
    AggState *aggstate;

    aggstate = (AggState *)DATA_PTR(self);
    return mrb_fixnum_value(aggstate->current_phase);
}

static mrb_value
psql_inspect_agg_state_current_set(mrb_state *mrb, mrb_value self)
{
    AggState *aggstate;

    aggstate = (AggState *)DATA_PTR(self);
    return mrb_fixnum_value(aggstate->current_set);
}

static mrb_value
psql_inspect_agg_state_num_hashes(mrb_state *mrb, mrb_value self)
{
    AggState *aggstate;

    aggstate = (AggState *)DATA_PTR(self);
    return mrb_fixnum_value(aggstate->num_hashes);
}

void
psql_inspect_query_desc_mruby_env_setup(mrb_state *mrb, QueryDesc *queryDesc)
{
    /* TODO: Ensure mrb and query_desc_class are set. */
    mrb_value obj = mrb_obj_new(mrb, query_desc_class, 0, NULL);
    psql_inspect_query_desc_set_query_desc(mrb, obj, queryDesc);
    mrb_mod_cv_set(mrb, query_desc_class, mrb_intern_lit(mrb, "query_desc"), obj);
}

void
psql_inspect_query_desc_mruby_env_tear_down(mrb_state *mrb)
{
    mrb_mod_cv_set(mrb, query_desc_class, mrb_intern_lit(mrb, "query_desc"), mrb_nil_value());
}

void
psql_inspect_query_desc_fini(mrb_state *mrb)
{
    query_desc_class = NULL;
}

void
psql_inspect_query_desc_class_init(mrb_state *mrb, struct RClass *class)
{
    /* QueryDesc class */
    query_desc_class = mrb_define_class_under(mrb, class, "QueryDesc", mrb->object_class);
    MRB_SET_INSTANCE_TT(query_desc_class, MRB_TT_DATA);

    mrb_define_class_method(mrb, query_desc_class, "current_query_desc", psql_inspect_c_current_query_desc, MRB_ARGS_NONE());
    mrb_define_method(mrb, query_desc_class, "initialize", psql_inspect_query_desc_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, query_desc_class, "operation", psql_inspect_query_desc_operation, MRB_ARGS_NONE());
    mrb_define_method(mrb, query_desc_class, "planstate", psql_inspect_query_desc_planstate, MRB_ARGS_NONE());

    /* PlanState is base class */
    plan_state_class = mrb_define_class_under(mrb, class, "PlanState", mrb->object_class);
    MRB_SET_INSTANCE_TT(plan_state_class, MRB_TT_DATA);

    mrb_define_method(mrb, plan_state_class, "initialize", psql_inspect_plan_state_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, plan_state_class, "type", psql_inspect_plan_state_type, MRB_ARGS_NONE());

    /* AggState */
    agg_state_class = mrb_define_class_under(mrb, class, "AggState", mrb->object_class);
    MRB_SET_INSTANCE_TT(agg_state_class, MRB_TT_DATA);

    mrb_define_method(mrb, agg_state_class, "initialize", psql_inspect_agg_state_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, agg_state_class, "numaggs", psql_inspect_agg_state_numaggs, MRB_ARGS_NONE());
    mrb_define_method(mrb, agg_state_class, "numtrans", psql_inspect_agg_state_numtrans, MRB_ARGS_NONE());
    mrb_define_method(mrb, agg_state_class, "numphases", psql_inspect_agg_state_numphases, MRB_ARGS_NONE());
    mrb_define_method(mrb, agg_state_class, "current_phase", psql_inspect_agg_state_current_phase, MRB_ARGS_NONE());
    mrb_define_method(mrb, agg_state_class, "current_set", psql_inspect_agg_state_current_set, MRB_ARGS_NONE());
    mrb_define_method(mrb, agg_state_class, "num_hashes", psql_inspect_agg_state_num_hashes, MRB_ARGS_NONE());
}
