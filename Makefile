P_INSPECT_ROOT=$(shell pwd)
MRUBY_ROOT=$(P_INSPECT_ROOT)/mruby
SRC_DIR=$(P_INSPECT_ROOT)/plugin
INCLUDE_DIR=$(P_INSPECT_ROOT)/include
VPATH=$(SRC_DIR)
BUILD_DIR=$(P_INSPECT_ROOT)/build

RAKE=rake

MRUBY_INCLUDE=$(MRUBY_ROOT)/include
MRUBY_LIB=$(MRUBY_ROOT)/build/host/lib

# pg specific
MODULE_big=psql_inspect
OBJS=$(BUILD_DIR)/psql_inspect.o \
     $(BUILD_DIR)/psql_inspect_bitmapset.o \
     $(BUILD_DIR)/psql_inspect_expr.o \
     $(BUILD_DIR)/psql_inspect_nodes.o \
     $(BUILD_DIR)/psql_inspect_parse_state.o \
     $(BUILD_DIR)/psql_inspect_path.o \
     $(BUILD_DIR)/psql_inspect_path_key.o \
     $(BUILD_DIR)/psql_inspect_plan.o \
     $(BUILD_DIR)/psql_inspect_planned_stmt.o \
     $(BUILD_DIR)/psql_inspect_planner_info.o \
     $(BUILD_DIR)/psql_inspect_primnodes.o \
     $(BUILD_DIR)/psql_inspect_query.o \
     $(BUILD_DIR)/psql_inspect_query_desc.o \
     $(BUILD_DIR)/psql_inspect_rel_opt_info.o

PG_CONFIG=pg_config
SHLIB_LINK=-L$(MRUBY_LIB) -lmruby
PGXS := $(shell $(PG_CONFIG) --pgxs)

$(BUILD_DIR)/%.o : $(SRC_DIR)/%.c
	@if test ! -d $(BUILD_DIR); then mkdir -p $(BUILD_DIR); fi
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

include $(PGXS)

override CPPFLAGS := -I$(MRUBY_INCLUDE) -I$(INCLUDE_DIR) $(CPPFLAGS)

.PHONY: build_mruby

build_mruby:
	cd $(MRUBY_ROOT) && $(RAKE) MRUBY_CONFIG=$(P_INSPECT_ROOT)/build_config.rb
