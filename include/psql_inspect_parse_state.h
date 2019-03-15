#ifndef PSQL_INSPECT_PARSE_STATE_H
#define PSQL_INSPECT_PARSE_STATE_H

#include "parser/parse_node.h"

#include <mruby.h>

mrb_value psql_inspect_parse_state_build_from_parse_state(mrb_state *mrb, ParseState *state);

void psql_inspect_parse_state_mruby_env_setup(mrb_state *mrb, ParseState *state);
void psql_inspect_parse_state_mruby_env_tear_down(mrb_state *mrb);

void psql_inspect_parse_state_fini(mrb_state *mrb);
void psql_inspect_parse_state_class_init(mrb_state *mrb, struct RClass *class);

#endif  /* PSQL_INSPECT_PARSE_STATE_H */
