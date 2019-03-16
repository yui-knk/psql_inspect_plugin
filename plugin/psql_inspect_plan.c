#include "postgres.h"
#include "nodes/relation.h"
#include "nodes/pg_list.h"

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/data.h>

#include <psql_inspect_nodes.h>
#include <psql_inspect_plan.h>
#include <psql_inspect_primnodes.h>

static struct RClass *plan_class = NULL;
static struct RClass *agg_class = NULL;
static struct RClass *sort_class = NULL;
static struct RClass *seq_scan_class = NULL;

static const struct mrb_data_type psql_inspect_plan_data_type = { "Plan", mrb_free };
static const struct mrb_data_type psql_inspect_agg_data_type = { "Agg", mrb_free };
static const struct mrb_data_type psql_inspect_sort_data_type = { "Sort", mrb_free };
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
psql_inspect_agg_aggstrategy(mrb_state *mrb, mrb_value self)
{
    Agg *agg;

    agg = (Agg *)DATA_PTR(self);
    return psql_inspect_mrb_str_from_AggStrategy(mrb, agg->aggstrategy);
}

static mrb_value
psql_inspect_agg_num_cols(mrb_state *mrb, mrb_value self)
{
    Agg *agg;

    agg = (Agg *)DATA_PTR(self);
    return mrb_fixnum_value(agg->numCols);
}

static mrb_value
psql_inspect_agg_grp_col_idx(mrb_state *mrb, mrb_value self)
{
    Agg *agg;
    mrb_value ary;

    agg = (Agg *)DATA_PTR(self);
    ary = mrb_ary_new_capa(mrb, agg->numCols);

    for (int i = 0; i < agg->numCols; i++) {
        mrb_value v;

        v = mrb_fixnum_value(agg->grpColIdx[i]);
        mrb_ary_set(mrb, ary, i, v);
    }

    return ary;
}

static mrb_value
psql_inspect_agg_chain(mrb_state *mrb, mrb_value self)
{
    Agg *agg;
    int array_size;
    int i = 0;
    mrb_value ary;
    ListCell *lc;

    agg = (Agg *)DATA_PTR(self);
    array_size = list_length(agg->chain);
    ary = mrb_ary_new_capa(mrb, array_size);

    foreach(lc, agg->chain) {
        mrb_value v;
        Agg *agg2 = (Agg *) lfirst(lc);

        v = psql_inspect_plan_build_from_plan(mrb, (Plan *)agg2);
        mrb_ary_set(mrb, ary, i, v);
        i++;
    }

    return ary;
}

static mrb_value
psql_inspect_sort_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_sort_data_type;

    return self;
}

static mrb_value
psql_inspect_sort_num_cols(mrb_state *mrb, mrb_value self)
{
    Sort *s;

    s = (Sort *)DATA_PTR(self);
    return mrb_fixnum_value(s->numCols);
}

static mrb_value
psql_inspect_sort_sort_col_idx(mrb_state *mrb, mrb_value self)
{
    Sort *s;
    mrb_value ary;

    s = (Sort *)DATA_PTR(self);
    ary = mrb_ary_new_capa(mrb, s->numCols);

    for (int i = 0; i < s->numCols; i++) {
        mrb_value v;

        v = mrb_fixnum_value(s->sortColIdx[i]);
        mrb_ary_set(mrb, ary, i, v);
    }

    return ary;
}

static mrb_value
psql_inspect_sort_sort_operators(mrb_state *mrb, mrb_value self)
{
    Sort *s;
    mrb_value ary;

    s = (Sort *)DATA_PTR(self);
    ary = mrb_ary_new_capa(mrb, s->numCols);

    for (int i = 0; i < s->numCols; i++) {
        mrb_value v;

        v = mrb_fixnum_value(s->sortOperators[i]);
        mrb_ary_set(mrb, ary, i, v);
    }

    return ary;
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
      case T_Sort:
        val = mrb_class_new_instance(mrb, 0, NULL, sort_class);
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
psql_inspect_plan_targetlist(mrb_state *mrb, mrb_value self)
{
    Plan *plan;
    int array_size;
    int i = 0;
    mrb_value ary;
    ListCell *lc;

    plan = (Plan *)DATA_PTR(self);
    array_size = list_length(plan->targetlist);
    ary = mrb_ary_new_capa(mrb, array_size);

    foreach(lc, plan->targetlist) {
        mrb_value v;
        TargetEntry *tle = (TargetEntry *) lfirst(lc);

        v = psql_inspect_primnode_build_from_expr(mrb, (Expr *)tle);
        mrb_ary_set(mrb, ary, i, v);
        i++;
    }

    return ary;
}

/* input plan tree(s) */
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
    mrb_define_method(mrb, plan_class, "targetlist", psql_inspect_plan_targetlist, MRB_ARGS_NONE());
    mrb_define_method(mrb, plan_class, "lefttree", psql_inspect_plan_lefttree, MRB_ARGS_NONE());
    mrb_define_method(mrb, plan_class, "righttree", psql_inspect_plan_righttree, MRB_ARGS_NONE());

    /* Agg class */
    agg_class = mrb_define_class_under(mrb, class, "Agg", plan_class);
    MRB_SET_INSTANCE_TT(agg_class, MRB_TT_DATA);

    mrb_define_method(mrb, agg_class, "initialize", psql_inspect_agg_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, agg_class, "aggstrategy", psql_inspect_agg_aggstrategy, MRB_ARGS_NONE());
    mrb_define_method(mrb, agg_class, "num_cols", psql_inspect_agg_num_cols, MRB_ARGS_NONE());
    mrb_define_method(mrb, agg_class, "grp_col_idx", psql_inspect_agg_grp_col_idx, MRB_ARGS_NONE());
    mrb_define_method(mrb, agg_class, "chain", psql_inspect_agg_chain, MRB_ARGS_NONE());

    /* Sort class */
    sort_class = mrb_define_class_under(mrb, class, "Sort", plan_class);
    MRB_SET_INSTANCE_TT(sort_class, MRB_TT_DATA);

    mrb_define_method(mrb, sort_class, "initialize", psql_inspect_sort_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, sort_class, "num_cols", psql_inspect_sort_num_cols, MRB_ARGS_NONE());
    mrb_define_method(mrb, sort_class, "sort_col_idx", psql_inspect_sort_sort_col_idx, MRB_ARGS_NONE());
    mrb_define_method(mrb, sort_class, "sort_operators", psql_inspect_sort_sort_operators, MRB_ARGS_NONE());

    /* SeqScan class */
    seq_scan_class = mrb_define_class_under(mrb, class, "SeqScan", plan_class);
    MRB_SET_INSTANCE_TT(seq_scan_class, MRB_TT_DATA);

    mrb_define_method(mrb, seq_scan_class, "initialize", psql_inspect_seq_scan_init, MRB_ARGS_NONE());
}
