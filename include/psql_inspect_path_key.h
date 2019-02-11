#ifndef PSQL_INSPECT_PATH_KEY_H
#define PSQL_INSPECT_PATH_KEY_H

#include "nodes/relation.h"

#include <mruby.h>

mrb_value psql_inspect_path_key_build_from_path_key(mrb_state *mrb, PathKey *pathkey);
void psql_inspect_path_key_class_init(mrb_state *mrb, struct RClass *class);

#endif  /* PSQL_INSPECT_PATH_KEY_H */
