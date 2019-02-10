P_INSPECT_ROOT=$(shell pwd)/..
MRUBY_ROOT=$(P_INSPECT_ROOT)/mruby
SRC_DIR=$(P_INSPECT_ROOT)/src

RAKE=rake

SRCS=$(SRC_DIR)/psql_inspect.c
MRUBY_INCLUDE=$(MRUBY_ROOT)/include
MRUBY_LIB=$(MRUBY_ROOT)/build/host/lib

# pg specific
MODULE_big=psql_inspect
OBJS=psql_inspect.o
PG_CONFIG=pg_config
SHLIB_LINK=-L$(MRUBY_LIB) -lmruby
PGXS := $(shell $(PG_CONFIG) --pgxs)

include $(PGXS)

override CPPFLAGS := -I$(MRUBY_INCLUDE) $(CPPFLAGS)

.PHONY: build_mruby

build_mruby:
	cd $(MRUBY_ROOT) && $(RAKE) MRUBY_CONFIG=$(P_INSPECT_ROOT)/build_config.rb
