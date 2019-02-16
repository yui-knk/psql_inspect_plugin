#include "postgres.h"
#include "nodes/bitmapset.h"

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/data.h>

#include <psql_inspect_bitmapset.h>

static struct RClass *bitmapset_class = NULL;
static const struct mrb_data_type psql_inspect_bitmapset_data_type = { "Bitmapset", mrb_free };

static void
psql_inspect_bitmapset_set_bitmapset(mrb_state *mrb, mrb_value self, Bitmapset *b)
{
    DATA_PTR(self) = b;
}

static mrb_value
psql_inspect_bitmapset_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_bitmapset_data_type;

    return self;
}

mrb_value
psql_inspect_bitmapset_build_from_bitmapset(mrb_state *mrb, Bitmapset *b)
{
    mrb_value val = mrb_class_new_instance(mrb, 0, NULL, bitmapset_class);
    psql_inspect_bitmapset_set_bitmapset(mrb, val, b);

    return val;
}

static mrb_value
psql_inspect_bitmapset_bms_num_members(mrb_state *mrb, mrb_value self)
{
    Bitmapset *b;

    b = (Bitmapset *)DATA_PTR(self);
    return mrb_fixnum_value(bms_num_members(b));
}

static mrb_value
psql_inspect_bitmapset_words(mrb_state *mrb, mrb_value self)
{
    Bitmapset *b;
    int array_size;
    mrb_value ary;

    b = (Bitmapset *)DATA_PTR(self);
    array_size = bms_num_members(b);
    ary = mrb_ary_new_capa(mrb, array_size);

    for (int i = 0; i < array_size; i++) {
        mrb_value v;

        v = mrb_fixnum_value(b->words[i]);
        mrb_ary_set(mrb, ary, i, v);
    }

    return ary;
}

void
psql_inspect_bitmapset_class_init(mrb_state *mrb, struct RClass *class)
{
    bitmapset_class = mrb_define_class_under(mrb, class, "Bitmapset", mrb->object_class);
    MRB_SET_INSTANCE_TT(bitmapset_class, MRB_TT_DATA);

    mrb_define_method(mrb, bitmapset_class, "initialize", psql_inspect_bitmapset_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, bitmapset_class, "bms_num_members", psql_inspect_bitmapset_bms_num_members, MRB_ARGS_NONE());
    /* For debug */
    mrb_define_method(mrb, bitmapset_class, "words", psql_inspect_bitmapset_words, MRB_ARGS_NONE());
}
