#include "postgres.h"
#include "access/stratnum.h"
#include "nodes/relation.h"
#include "nodes/pg_list.h"

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/data.h>

#include <psql_inspect_bitmapset.h>
#include <psql_inspect_primnodes.h>
#include <psql_inspect_nodes.h>
#include <psql_inspect_path_key.h>

static struct RClass *path_key_class = NULL;
static struct RClass *equivalence_class_class = NULL;
static struct RClass *equivalence_member_class = NULL;

static const struct mrb_data_type psql_inspect_path_key_data_type = { "PathKey", mrb_free };
static const struct mrb_data_type psql_inspect_equivalence_class_data_type = { "EquivalenceClass", mrb_free };
static const struct mrb_data_type psql_inspect_equivalence_member_data_type = { "EquivalenceMember", mrb_free };

static mrb_value psql_inspect_equivalence_class_build_from_equivalence_class(mrb_state *mrb, EquivalenceClass *ec);
static mrb_value psql_inspect_equivalence_member_build_from_equivalence_member(mrb_state *mrb, EquivalenceMember *em);


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

/*
 * Note: pk_strategy is either BTLessStrategyNumber (for ASC) or
 * BTGreaterStrategyNumber (for DESC).
 * See: src/include/nodes/relation.h
 */
static mrb_value
psql_inspect_path_key_get_strategy_str(mrb_state *mrb, int pk_strategy)
{
    switch (pk_strategy) {
      case BTLessStrategyNumber:
        return mrb_str_new_cstr(mrb, "BTLess");
      case BTLessEqualStrategyNumber:
        return mrb_str_new_cstr(mrb, "BTLessEqual");
      case BTEqualStrategyNumber:
        return mrb_str_new_cstr(mrb, "BTEqual");
      case BTGreaterEqualStrategyNumber:
        return mrb_str_new_cstr(mrb, "BTGreaterEqual");
      case BTGreaterStrategyNumber:
        return mrb_str_new_cstr(mrb, "BTGreater");
      default:
        mrb_raisef(mrb, E_RUNTIME_ERROR, "Unknown pk_strategy number: %d", pk_strategy);
    }
}

static mrb_value
psql_inspect_path_key_pk_eclass(mrb_state *mrb, mrb_value self)
{
    PathKey *path_key;

    path_key = (PathKey *)DATA_PTR(self);
    return psql_inspect_equivalence_class_build_from_equivalence_class(mrb, path_key->pk_eclass);
}

static mrb_value
psql_inspect_path_key_pk_strategy(mrb_state *mrb, mrb_value self)
{
    PathKey *path_key;

    path_key = (PathKey *)DATA_PTR(self);
    return psql_inspect_path_key_get_strategy_str(mrb, path_key->pk_strategy);
}


static mrb_value
psql_inspect_equivalence_class_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_equivalence_class_data_type;

    return self;
}

static void
psql_inspect_equivalence_class_set_equivalence_class(mrb_state *mrb, mrb_value self, EquivalenceClass *ec)
{
    DATA_PTR(self) = ec;
}

static mrb_value
psql_inspect_equivalence_class_build_from_equivalence_class(mrb_state *mrb, EquivalenceClass *ec)
{
    mrb_value val = mrb_class_new_instance(mrb, 0, NULL, equivalence_class_class);
    psql_inspect_equivalence_class_set_equivalence_class(mrb, val, ec);

    return val;
}

static mrb_value
psql_inspect_equivalence_class_type(mrb_state *mrb, mrb_value self)
{
    EquivalenceClass *ec;

    ec = (EquivalenceClass *)DATA_PTR(self);
    return psql_inspect_mrb_str_from_NodeTag(mrb, ec->type);
}

static mrb_value
psql_inspect_equivalence_class_ec_members(mrb_state *mrb, mrb_value self)
{
    EquivalenceClass *ec;
    int array_size;
    int i = 0;
    mrb_value ary;
    ListCell   *lc;

    ec = (EquivalenceClass *)DATA_PTR(self);
    array_size = list_length(ec->ec_members);
    ary = mrb_ary_new_capa(mrb, array_size);

    foreach(lc, ec->ec_members) {
        mrb_value v;
        EquivalenceMember *em = (EquivalenceMember *) lfirst(lc);

        v = psql_inspect_equivalence_member_build_from_equivalence_member(mrb, em);
        mrb_ary_set(mrb, ary, i, v);
        i++;
    }

    return ary;
}

static mrb_value
psql_inspect_equivalence_class_ec_sortref(mrb_state *mrb, mrb_value self)
{
    EquivalenceClass *ec;

    ec = (EquivalenceClass *)DATA_PTR(self);
    return mrb_fixnum_value(ec->ec_sortref);
}


static mrb_value
psql_inspect_equivalence_member_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_equivalence_member_data_type;

    return self;
}

static void
psql_inspect_equivalence_member_set_equivalence_member(mrb_state *mrb, mrb_value self, EquivalenceMember *em)
{
    DATA_PTR(self) = em;
}

static mrb_value
psql_inspect_equivalence_member_build_from_equivalence_member(mrb_state *mrb, EquivalenceMember *em)
{
    mrb_value val = mrb_class_new_instance(mrb, 0, NULL, equivalence_member_class);
    psql_inspect_equivalence_member_set_equivalence_member(mrb, val, em);

    return val;
}

static mrb_value
psql_inspect_equivalence_member_type(mrb_state *mrb, mrb_value self)
{
    EquivalenceMember *em;

    em = (EquivalenceMember *)DATA_PTR(self);
    return psql_inspect_mrb_str_from_NodeTag(mrb, em->type);
}

static mrb_value
psql_inspect_equivalence_member_em_expr(mrb_state *mrb, mrb_value self)
{
    EquivalenceMember *em;

    em = (EquivalenceMember *)DATA_PTR(self);
    return psql_inspect_primnode_build_from_expr(mrb, em->em_expr);
}

static mrb_value
psql_inspect_equivalence_member_em_relids(mrb_state *mrb, mrb_value self)
{
    EquivalenceMember *em;

    em = (EquivalenceMember *)DATA_PTR(self);
    return psql_inspect_bitmapset_build_from_bitmapset(mrb, em->em_relids);
}

void
psql_inspect_path_key_class_init(mrb_state *mrb, struct RClass *class)
{
    path_key_class = mrb_define_class_under(mrb, class, "PathKey", mrb->object_class);
    MRB_SET_INSTANCE_TT(path_key_class, MRB_TT_DATA);

    mrb_define_method(mrb, path_key_class, "initialize", psql_inspect_path_key_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, path_key_class, "type", psql_inspect_path_key_type, MRB_ARGS_NONE());
    mrb_define_method(mrb, path_key_class, "pk_eclass", psql_inspect_path_key_pk_eclass, MRB_ARGS_NONE());
    mrb_define_method(mrb, path_key_class, "pk_strategy", psql_inspect_path_key_pk_strategy, MRB_ARGS_NONE());


    equivalence_class_class = mrb_define_class_under(mrb, class, "EquivalenceClass", mrb->object_class);
    MRB_SET_INSTANCE_TT(equivalence_class_class, MRB_TT_DATA);
    mrb_define_method(mrb, equivalence_class_class, "initialize", psql_inspect_equivalence_class_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, equivalence_class_class, "type", psql_inspect_equivalence_class_type, MRB_ARGS_NONE());
    mrb_define_method(mrb, equivalence_class_class, "ec_members", psql_inspect_equivalence_class_ec_members, MRB_ARGS_NONE());
    mrb_define_method(mrb, equivalence_class_class, "ec_sortref", psql_inspect_equivalence_class_ec_sortref, MRB_ARGS_NONE());


    equivalence_member_class = mrb_define_class_under(mrb, class, "EquivalenceMember", mrb->object_class);
    MRB_SET_INSTANCE_TT(equivalence_member_class, MRB_TT_DATA);
    mrb_define_method(mrb, equivalence_member_class, "initialize", psql_inspect_equivalence_member_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, equivalence_member_class, "type", psql_inspect_equivalence_member_type, MRB_ARGS_NONE());
    mrb_define_method(mrb, equivalence_member_class, "em_expr", psql_inspect_equivalence_member_em_expr, MRB_ARGS_NONE());
    mrb_define_method(mrb, equivalence_member_class, "em_relids", psql_inspect_equivalence_member_em_relids, MRB_ARGS_NONE());
}
