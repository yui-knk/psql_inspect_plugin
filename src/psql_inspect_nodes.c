#include "postgres.h"
#include "nodes/nodes.h"

#include <mruby.h>

#include <psql_inspect_nodes.h>

mrb_value
psql_inspect_mrb_str_from_CmdType(mrb_state *mrb, CmdType type)
{

#define CMD_TYPE(type) \
    CMD_##type: return mrb_str_new_cstr(mrb, #type);

    switch (type) {
      case CMD_TYPE(UNKNOWN);
      case CMD_TYPE(SELECT);
      case CMD_TYPE(UPDATE);
      case CMD_TYPE(INSERT);
      case CMD_TYPE(DELETE);
      case CMD_TYPE(UTILITY);
      case CMD_TYPE(NOTHING);
#undef CMD_TYPE
      default:
        mrb_raisef(mrb, E_RUNTIME_ERROR, "Unknown command type number: %d", type);
    }
}
