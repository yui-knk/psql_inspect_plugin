## How to use

### Build mruby

```shell
$ cd src
$ make build_mruby
```

### Build extension

```shell
$ cd src
$ make
```

### Load extension

```shell
$ psql -d my_db

my_db=# load '/path/to/psql_inspect.so';
LOAD

my_db=# set session "psql_inspect.planner_script" = 'p [PgInspect::PlannedStmt.current_stmt.type, PgInspect::PlannedStmt.current_stmt.command_type]';
SET

my_db=# select * from films;
 code | title | did | date_prod | kind | len
------+-------+-----+-----------+------+-----
(0 rows)
```
