#include "postgres.h"
#include "nodes/parsenodes.h"
#include "nodes/primnodes.h"

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/variable.h>

#include <psql_inspect_expr.h>
#include <psql_inspect_nodes.h>
#include <psql_inspect_query.h>

static struct RClass *query_class = NULL;
static struct RClass *range_tbl_entry_class = NULL;

static struct RClass *from_expr_class = NULL;

static const struct mrb_data_type psql_inspect_query_data_type = { "Query", mrb_free };
static const struct mrb_data_type psql_inspect_range_tbl_entry_data_type = { "RangeTblEntry", mrb_free };
static const struct mrb_data_type psql_inspect_from_expr_data_type = { "FromExpr", mrb_free };

static mrb_value
psql_inspect_range_tbl_entry_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_range_tbl_entry_data_type;

    return self;
}

static mrb_value
psql_inspect_from_expr_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_from_expr_data_type;

    return self;
}

static void
psql_inspect_range_tbl_entry_set_range_tbl_entry(mrb_state *mrb, mrb_value self, RangeTblEntry *rte)
{
    DATA_PTR(self) = rte;
}

static mrb_value
psql_inspect_range_tbl_entry_type(mrb_state *mrb, mrb_value self)
{
    RangeTblEntry *rte;

    rte = (RangeTblEntry *)DATA_PTR(self);
    return psql_inspect_mrb_str_from_NodeTag(mrb, rte->type);
}

static mrb_value
psql_inspect_mrb_str_from_RTEKind(mrb_state *mrb, RTEKind rtekind)
{
    #define RTE_TYPE(rtekind) \
        RTE_##rtekind: return mrb_str_new_cstr(mrb, #rtekind);

        switch (rtekind) {
          case RTE_TYPE(RELATION);
          case RTE_TYPE(SUBQUERY);
          case RTE_TYPE(JOIN);
          case RTE_TYPE(FUNCTION);
          case RTE_TYPE(TABLEFUNC);
          case RTE_TYPE(VALUES);
          case RTE_TYPE(CTE);
          case RTE_TYPE(NAMEDTUPLESTORE);
    #undef RTE_TYPE
          default:
            mrb_raisef(mrb, E_RUNTIME_ERROR, "Unknown RTEKind number: %S", mrb_fixnum_value(rtekind));
        }
}

static mrb_value
psql_inspect_range_tbl_entry_rtekind(mrb_state *mrb, mrb_value self)
{
    RangeTblEntry *rte;

    rte = (RangeTblEntry *)DATA_PTR(self);
    return psql_inspect_mrb_str_from_RTEKind(mrb, rte->rtekind);
}

/* TODO: Extract method which convert Oid to mrb_value into outer file  */
static mrb_value
psql_inspect_range_tbl_entry_relid(mrb_state *mrb, mrb_value self)
{
    RangeTblEntry *rte;

    rte = (RangeTblEntry *)DATA_PTR(self);
    return mrb_fixnum_value(rte->relid);
}

static mrb_value
psql_inspect_range_tbl_entry_relkind(mrb_state *mrb, mrb_value self)
{
    RangeTblEntry *rte;

    rte = (RangeTblEntry *)DATA_PTR(self);
    return mrb_str_new(mrb, &rte->relkind, 1);
}

mrb_value
psql_inspect_range_tbl_entry_build_from_rte(mrb_state *mrb, RangeTblEntry *rte)
{
    mrb_value val;

    val = mrb_class_new_instance(mrb, 0, NULL, range_tbl_entry_class);
    psql_inspect_range_tbl_entry_set_range_tbl_entry(mrb, val, rte);
    return val;
}

static mrb_value
psql_inspect_from_expr_build_from_from_expr(mrb_state *mrb, FromExpr *from)
{
    mrb_value val;

    val = mrb_class_new_instance(mrb, 0, NULL, from_expr_class);
    DATA_PTR(val) = from;

    return val;
}

static void
psql_inspect_query_set_query(mrb_state *mrb, mrb_value self, Query *query)
{
    DATA_PTR(self) = query;
}

static mrb_value
psql_inspect_c_current_query(mrb_state *mrb, mrb_value klass)
{
    return mrb_cv_get(mrb, klass, mrb_intern_lit(mrb, "query_class"));
}

static mrb_value
psql_inspect_query_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_query_data_type;

    return self;
}

static mrb_value
psql_inspect_query_type(mrb_state *mrb, mrb_value self)
{
    Query *query;

    query = (Query *)DATA_PTR(self);
    return psql_inspect_mrb_str_from_NodeTag(mrb, query->type);
}

static mrb_value
psql_inspect_query_command_type(mrb_state *mrb, mrb_value self)
{
    Query *query;

    query = (Query *)DATA_PTR(self);
    return psql_inspect_mrb_str_from_CmdType(mrb, query->commandType);
}

static mrb_value
psql_inspect_query_queryId(mrb_state *mrb, mrb_value self)
{
    Query *query;

    query = (Query *)DATA_PTR(self);
    return mrb_fixnum_value(query->queryId);
}

static mrb_value
psql_inspect_query_has_aggs(mrb_state *mrb, mrb_value self)
{
    Query *query;

    query = (Query *)DATA_PTR(self);
    return mrb_bool_value(query->hasAggs);
}

static mrb_value
psql_inspect_query_rtable(mrb_state *mrb, mrb_value self)
{
    Query *query;

    int array_size;
    int i = 0;
    mrb_value ary;
    ListCell *lc;

    query = (Query *)DATA_PTR(self);
    array_size = list_length(query->rtable);
    ary = mrb_ary_new_capa(mrb, array_size);

    foreach(lc, query->rtable) {
        mrb_value v;
        RangeTblEntry *rte = (RangeTblEntry *) lfirst(lc);

        v = psql_inspect_range_tbl_entry_build_from_rte(mrb, rte);
        mrb_ary_set(mrb, ary, i, v);
        i++;
    }

    return ary;
}

/* table join tree (FROM and WHERE clauses) */
static mrb_value
psql_inspect_query_jointree(mrb_state *mrb, mrb_value self)
{
    Query *query;

    query = (Query *)DATA_PTR(self);
    return psql_inspect_from_expr_build_from_from_expr(mrb, query->jointree);
}

static mrb_value
psql_inspect_query_target_list(mrb_state *mrb, mrb_value self)
{
    Query *query;
    int array_size;
    int i = 0;
    mrb_value ary;
    ListCell *lc;

    query = (Query *)DATA_PTR(self);
    array_size = list_length(query->targetList);
    ary = mrb_ary_new_capa(mrb, array_size);

    foreach(lc, query->targetList) {
        mrb_value v;
        TargetEntry *tle = (TargetEntry *) lfirst(lc);

        v = psql_inspect_expr_build_from_expr(mrb, (Expr *)tle);
        mrb_ary_set(mrb, ary, i, v);
        i++;
    }

    return ary;
}

static mrb_value
psql_inspect_from_expr_quals(mrb_state *mrb, mrb_value self)
{
    FromExpr *from;

    from = (FromExpr *)DATA_PTR(self);

    if (from->quals == NULL)
        return mrb_nil_value();
    return psql_inspect_expr_build_from_expr(mrb, (Expr *)from->quals);
}

void
psql_inspect_query_mruby_env_setup(mrb_state *mrb, Query *query)
{
    /* TODO: Ensure mrb and query_class are set. */
    mrb_value obj = mrb_obj_new(mrb, query_class, 0, NULL);
    psql_inspect_query_set_query(mrb, obj, query);
    mrb_mod_cv_set(mrb, query_class, mrb_intern_lit(mrb, "query_class"), obj);
}

void
psql_inspect_query_mruby_env_tear_down(mrb_state *mrb)
{
    mrb_mod_cv_set(mrb, query_class, mrb_intern_lit(mrb, "query_class"), mrb_nil_value());
}

void
psql_inspect_query_fini(mrb_state *mrb)
{
    query_class = NULL;
}

void
psql_inspect_query_class_init(mrb_state *mrb, struct RClass *class)
{
    /* Query class */
    query_class = mrb_define_class_under(mrb, class, "Query", mrb->object_class);
    MRB_SET_INSTANCE_TT(query_class, MRB_TT_DATA);

    mrb_define_class_method(mrb, query_class, "current_query", psql_inspect_c_current_query, MRB_ARGS_NONE());
    mrb_define_method(mrb, query_class, "initialize", psql_inspect_query_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, query_class, "type", psql_inspect_query_type, MRB_ARGS_NONE());
    mrb_define_method(mrb, query_class, "command_type", psql_inspect_query_command_type, MRB_ARGS_NONE());
    mrb_define_method(mrb, query_class, "queryId", psql_inspect_query_queryId, MRB_ARGS_NONE());
    // mrb_define_method(mrb, query_class, "resultRelation", psql_inspect_query_resultRelation, MRB_ARGS_NONE());
    mrb_define_method(mrb, query_class, "has_aggs", psql_inspect_query_has_aggs, MRB_ARGS_NONE());
    // mrb_define_method(mrb, query_class, "hasWindowFuncs", psql_inspect_query_hasWindowFuncs, MRB_ARGS_NONE());
    // mrb_define_method(mrb, query_class, "hasTargetSRFs", psql_inspect_query_hasTargetSRFs, MRB_ARGS_NONE());
    // mrb_define_method(mrb, query_class, "hasSubLinks", psql_inspect_query_hasSubLinks, MRB_ARGS_NONE());
    // mrb_define_method(mrb, query_class, "hasDistinctOn", psql_inspect_query_hasDistinctOn, MRB_ARGS_NONE());
    // mrb_define_method(mrb, query_class, "hasRecursive", psql_inspect_query_hasRecursive, MRB_ARGS_NONE());
    // mrb_define_method(mrb, query_class, "hasModifyingCTE", psql_inspect_query_hasModifyingCTE, MRB_ARGS_NONE());
    // mrb_define_method(mrb, query_class, "hasForUpdate", psql_inspect_query_hasForUpdate, MRB_ARGS_NONE());
    // mrb_define_method(mrb, query_class, "hasRowSecurity", psql_inspect_query_hasRowSecurity, MRB_ARGS_NONE());
    mrb_define_method(mrb, query_class, "rtable", psql_inspect_query_rtable, MRB_ARGS_NONE());
    mrb_define_method(mrb, query_class, "jointree", psql_inspect_query_jointree, MRB_ARGS_NONE());
    mrb_define_method(mrb, query_class, "target_list", psql_inspect_query_target_list, MRB_ARGS_NONE());
    // mrb_define_method(mrb, query_class, "group_clause", psql_inspect_query_group_clause, MRB_ARGS_NONE());
    // mrb_define_method(mrb, query_class, "sort_clause", psql_inspect_query_sort_clause, MRB_ARGS_NONE());

    /* RangeTblEntry class */
    range_tbl_entry_class = mrb_define_class_under(mrb, class, "RangeTblEntry", mrb->object_class);
    MRB_SET_INSTANCE_TT(range_tbl_entry_class, MRB_TT_DATA);

    mrb_define_method(mrb, range_tbl_entry_class, "initialize", psql_inspect_range_tbl_entry_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, range_tbl_entry_class, "type", psql_inspect_range_tbl_entry_type, MRB_ARGS_NONE());
    mrb_define_method(mrb, range_tbl_entry_class, "rtekind", psql_inspect_range_tbl_entry_rtekind, MRB_ARGS_NONE());
    mrb_define_method(mrb, range_tbl_entry_class, "relid", psql_inspect_range_tbl_entry_relid, MRB_ARGS_NONE());
    mrb_define_method(mrb, range_tbl_entry_class, "relkind", psql_inspect_range_tbl_entry_relkind, MRB_ARGS_NONE());

    /* FromExpr class */
    from_expr_class = mrb_define_class_under(mrb, class, "FromExpr", psql_inspect_node_class);
    MRB_SET_INSTANCE_TT(from_expr_class, MRB_TT_DATA);

    mrb_define_method(mrb, from_expr_class, "initialize", psql_inspect_from_expr_init, MRB_ARGS_NONE());
    // mrb_define_method(mrb, from_expr_class, "fromlist", psql_inspect_from_expr_fromlist, MRB_ARGS_NONE());
    mrb_define_method(mrb, from_expr_class, "quals", psql_inspect_from_expr_quals, MRB_ARGS_NONE());
}
