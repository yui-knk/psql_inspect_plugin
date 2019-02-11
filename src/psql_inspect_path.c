#include "postgres.h"
#include "nodes/relation.h"

#include <mruby.h>
#include <mruby/class.h>
#include <mruby/data.h>

#include <psql_inspect_nodes.h>
#include <psql_inspect_path.h>

static struct RClass *path_class = NULL;
static const struct mrb_data_type psql_inspect_path_data_type = { "Path", mrb_free };

static void
psql_inspect_path_set_path(mrb_state *mrb, mrb_value self, Path *path)
{
    DATA_PTR(self) = path;
}

static mrb_value
psql_inspect_path_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_path_data_type;

    return self;
}

mrb_value
psql_inspect_path_build_from_path(mrb_state *mrb, Path *path)
{
    mrb_value val = mrb_class_new_instance(mrb, 0, NULL, path_class);
    psql_inspect_path_set_path(mrb, val, path);

    return val;
}

static mrb_value
psql_inspect_path_type(mrb_state *mrb, mrb_value self)
{
    Path *path;

    path = (Path *)DATA_PTR(self);
    return psql_inspect_mrb_str_from_NodeTag(mrb, path->type);
}

static mrb_value
psql_inspect_path_pathtype(mrb_state *mrb, mrb_value self)
{
    Path *path;

    path = (Path *)DATA_PTR(self);
    return psql_inspect_mrb_str_from_NodeTag(mrb, path->pathtype);
}

void
psql_inspect_path_class_init(mrb_state *mrb, struct RClass *class)
{
    path_class = mrb_define_class_under(mrb, class, "Path", mrb->object_class);
    MRB_SET_INSTANCE_TT(path_class, MRB_TT_DATA);

    mrb_define_method(mrb, path_class, "initialize", psql_inspect_path_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, path_class, "type", psql_inspect_path_type, MRB_ARGS_NONE());
    mrb_define_method(mrb, path_class, "pathtype", psql_inspect_path_pathtype, MRB_ARGS_NONE());
}
