#include "postgres.h"
#include "executor/execdesc.h"

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/variable.h>

#include <psql_inspect_nodes.h>
#include <psql_inspect_query_desc.h>

static struct RClass *query_desc_class = NULL;

static const struct mrb_data_type psql_inspect_query_desc_data_type = { "QueryDesc", mrb_free };

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
psql_inspect_query_desc_operation(mrb_state *mrb, mrb_value self)
{
    QueryDesc *queryDesc;

    queryDesc = (QueryDesc *)DATA_PTR(self);
    return psql_inspect_mrb_str_from_CmdType(mrb, queryDesc->operation);
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
}
