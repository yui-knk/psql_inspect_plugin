#include "postgres.h"
#include "parser/parse_node.h"

#include <mruby.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/variable.h>

#include <psql_inspect_parse_state.h>

static struct RClass *parse_state_class = NULL;

static const struct mrb_data_type psql_inspect_parse_state_data_type = { "ParseState", mrb_free };

mrb_value
psql_inspect_parse_state_build_from_parse_state(mrb_state *mrb, ParseState *state)
{
    mrb_value val;

    val = mrb_class_new_instance(mrb, 0, NULL, parse_state_class);
    DATA_PTR(val) = state;

    return val;
}

static mrb_value
psql_inspect_parse_state_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_parse_state_data_type;

    return self;
}

static mrb_value
psql_inspect_parse_state_sourcetext(mrb_state *mrb, mrb_value self)
{
    ParseState *state;

    state = DATA_PTR(self);
    return mrb_str_new_cstr(mrb, state->p_sourcetext);
}

static mrb_value
psql_inspect_c_current_parse_state(mrb_state *mrb, mrb_value klass)
{
    return mrb_cv_get(mrb, klass, mrb_intern_lit(mrb, "current_parse_state"));
}

void
psql_inspect_parse_state_mruby_env_setup(mrb_state *mrb, ParseState *state)
{
    /* TODO: Ensure mrb and parse_state_class are set. */
    mrb_value obj = mrb_obj_new(mrb, parse_state_class, 0, NULL);
    DATA_PTR(obj) = state;
    mrb_mod_cv_set(mrb, parse_state_class, mrb_intern_lit(mrb, "current_parse_state"), obj);
}

void
psql_inspect_parse_state_mruby_env_tear_down(mrb_state *mrb)
{
    mrb_mod_cv_set(mrb, parse_state_class, mrb_intern_lit(mrb, "current_parse_state"), mrb_nil_value());
}

void
psql_inspect_parse_state_fini(mrb_state *mrb)
{
    parse_state_class = NULL;
}

void
psql_inspect_parse_state_class_init(mrb_state *mrb, struct RClass *class)
{
    /* ParseState class */
    parse_state_class = mrb_define_class_under(mrb, class, "ParseState", mrb->object_class);
    MRB_SET_INSTANCE_TT(parse_state_class, MRB_TT_DATA);

    mrb_define_class_method(mrb, parse_state_class, "current_parse_state", psql_inspect_c_current_parse_state, MRB_ARGS_NONE());
    mrb_define_method(mrb, parse_state_class, "initialize", psql_inspect_parse_state_init, MRB_ARGS_NONE());
    /*  const char *p_sourcetext; */
    mrb_define_method(mrb, parse_state_class, "sourcetext", psql_inspect_parse_state_sourcetext, MRB_ARGS_NONE());
}
