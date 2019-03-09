#include "postgres.h"
#include "nodes/relation.h"
#include "nodes/pg_list.h"

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/data.h>

#include <psql_inspect_nodes.h>
#include <psql_inspect_path.h>
#include <psql_inspect_rel_opt_info.h>

static struct RClass *rel_opt_info = NULL;
static const struct mrb_data_type psql_inspect_rel_opt_info_data_type = { "RelOptInfo", mrb_free };

static void
psql_inspect_rel_opt_info_set_rel_opt_info(mrb_state *mrb, mrb_value self, RelOptInfo *rel)
{
    DATA_PTR(self) = rel;
}

static mrb_value
psql_inspect_rel_opt_info_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_rel_opt_info_data_type;

    return self;
}

mrb_value
psql_inspect_rel_opt_info_build_from_rel_opt_info(mrb_state *mrb, RelOptInfo *rel)
{
    mrb_value val = mrb_class_new_instance(mrb, 0, NULL, rel_opt_info);
    psql_inspect_rel_opt_info_set_rel_opt_info(mrb, val, rel);

    return val;
}

static mrb_value
psql_inspect_rel_opt_info_type(mrb_state *mrb, mrb_value self)
{
    RelOptInfo *rel;

    rel = (RelOptInfo *)DATA_PTR(self);
    return psql_inspect_mrb_str_from_NodeTag(mrb, rel->type);
}

static mrb_value
psql_inspect_rel_opt_info_pathlist(mrb_state *mrb, mrb_value self)
{
    RelOptInfo *rel;
    int array_size;
    int i = 0;
    mrb_value ary;
    ListCell   *lc;

    rel = (RelOptInfo *)DATA_PTR(self);
    array_size = list_length(rel->pathlist);
    ary = mrb_ary_new_capa(mrb, array_size);

    foreach(lc, rel->pathlist) {
        mrb_value v;
        Path *path = (Path *) lfirst(lc);

        v = psql_inspect_path_build_from_path(mrb, path);
        mrb_ary_set(mrb, ary, i, v);
        i++;
    }

    return ary;
}

void
psql_inspect_rel_opt_info_class_init(mrb_state *mrb, struct RClass *class)
{
    rel_opt_info = mrb_define_class_under(mrb, class, "RelOptInfo", mrb->object_class);
    MRB_SET_INSTANCE_TT(rel_opt_info, MRB_TT_DATA);

    mrb_define_method(mrb, rel_opt_info, "initialize", psql_inspect_rel_opt_info_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, rel_opt_info, "type", psql_inspect_rel_opt_info_type, MRB_ARGS_NONE());
    mrb_define_method(mrb, rel_opt_info, "pathlist", psql_inspect_rel_opt_info_pathlist, MRB_ARGS_NONE());
}
