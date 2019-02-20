#include "postgres.h"
#include "nodes/parsenodes.h"

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/variable.h>

#include <psql_inspect_nodes.h>
#include <psql_inspect_query.h>

static struct RClass *query_class = NULL;
static const struct mrb_data_type psql_inspect_query_data_type = { "Query", mrb_free };

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
psql_inspect_current_query_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_query_data_type;

    return self;
}

static mrb_value
psql_inspect_current_query_type(mrb_state *mrb, mrb_value self)
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
psql_inspect_query_has_aggs(mrb_state *mrb, mrb_value self)
{
    Query *query;

    query = (Query *)DATA_PTR(self);
    return mrb_bool_value(query->hasAggs);
}

// static mrb_value
// psql_inspect_query_rtable(mrb_state *mrb, mrb_value self)
// {
//     Query *query;

//     query = (Query *)DATA_PTR(self);
//     return mrb_bool_value(query->has_aggs);
// }

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
    query_class = mrb_define_class_under(mrb, class, "Query", mrb->object_class);
    MRB_SET_INSTANCE_TT(query_class, MRB_TT_DATA);

    mrb_define_class_method(mrb, query_class, "current_query", psql_inspect_c_current_query, MRB_ARGS_NONE());
    mrb_define_method(mrb, query_class, "initialize", psql_inspect_current_query_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, query_class, "type", psql_inspect_current_query_type, MRB_ARGS_NONE());
    mrb_define_method(mrb, query_class, "command_type", psql_inspect_query_command_type, MRB_ARGS_NONE());
    mrb_define_method(mrb, query_class, "has_aggs", psql_inspect_query_has_aggs, MRB_ARGS_NONE());
    // mrb_define_method(mrb, query_class, "rtable", psql_inspect_query_rtable, MRB_ARGS_NONE());
    // mrb_define_method(mrb, query_class, "target_list", psql_inspect_query_target_list, MRB_ARGS_NONE());
    // mrb_define_method(mrb, query_class, "group_clause", psql_inspect_query_group_clause, MRB_ARGS_NONE());
    // mrb_define_method(mrb, query_class, "sort_clause", psql_inspect_query_sort_clause, MRB_ARGS_NONE());
}
