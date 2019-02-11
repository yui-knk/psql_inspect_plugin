P_INSPECT_ROOT=$(shell pwd)/..
MRUBY_ROOT=$(P_INSPECT_ROOT)/mruby
SRC_DIR=$(P_INSPECT_ROOT)/src
INCLUDE_DIR=$(P_INSPECT_ROOT)/include

RAKE=rake

MRUBY_INCLUDE=$(MRUBY_ROOT)/include
MRUBY_LIB=$(MRUBY_ROOT)/build/host/lib

# pg specific
MODULE_big=psql_inspect
OBJS=psql_inspect_nodes.o psql_inspect_planned_stmt.o \
     psql_inspect_rel_opt_info.o psql_inspect_planner_info.o \
     psql_inspect.o psql_inspect_path.o psql_inspect_path_key.o
PG_CONFIG=pg_config
SHLIB_LINK=-L$(MRUBY_LIB) -lmruby
PGXS := $(shell $(PG_CONFIG) --pgxs)

include $(PGXS)

override CPPFLAGS := -I$(MRUBY_INCLUDE) -I$(INCLUDE_DIR) $(CPPFLAGS)

.PHONY: build_mruby

build_mruby:
	cd $(MRUBY_ROOT) && $(RAKE) MRUBY_CONFIG=$(P_INSPECT_ROOT)/build_config.rb
