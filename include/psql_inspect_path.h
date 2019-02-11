#ifndef PSQL_INSPECT_PATH_H
#define PSQL_INSPECT_PATH_H

#include "nodes/relation.h"

#include <mruby.h>

mrb_value psql_inspect_path_build_from_path(mrb_state *mrb, Path *path);
void psql_inspect_path_class_init(mrb_state *mrb, struct RClass *class);

#endif  /* PSQL_INSPECT_PATH_H */
