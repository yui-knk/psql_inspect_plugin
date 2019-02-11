#include "postgres.h"
#include "nodes/relation.h"

#include <mruby.h>
#include <mruby/class.h>
#include <mruby/data.h>

#include <psql_inspect_nodes.h>
#include <psql_inspect_path_key.h>

static struct RClass *path_key_class = NULL;
static const struct mrb_data_type psql_inspect_path_key_data_type = { "PathKey", mrb_free };

static void
psql_inspect_path_key_set_path_key(mrb_state *mrb, mrb_value self, PathKey *path_key)
{
    DATA_PTR(self) = path_key;
}

static mrb_value
psql_inspect_path_key_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_path_key_data_type;

    return self;
}

mrb_value
psql_inspect_path_key_build_from_path_key(mrb_state *mrb, PathKey *path_key)
{
    mrb_value val = mrb_class_new_instance(mrb, 0, NULL, path_key_class);
    psql_inspect_path_key_set_path_key(mrb, val, path_key);

    return val;
}

static mrb_value
psql_inspect_path_key_type(mrb_state *mrb, mrb_value self)
{
    PathKey *path_key;

    path_key = (PathKey *)DATA_PTR(self);
    return psql_inspect_mrb_str_from_NodeTag(mrb, path_key->type);
}

void
psql_inspect_path_key_class_init(mrb_state *mrb, struct RClass *class)
{
    path_key_class = mrb_define_class_under(mrb, class, "PathKey", mrb->object_class);
    MRB_SET_INSTANCE_TT(path_key_class, MRB_TT_DATA);

    mrb_define_method(mrb, path_key_class, "initialize", psql_inspect_path_key_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, path_key_class, "type", psql_inspect_path_key_type, MRB_ARGS_NONE());
}
