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
```

### Set script

```shell
set session "psql_inspect.ExecutorRun_script" = 'p PgInspect::ANSWER';
```

```shell
my_db=# set session "psql_inspect.post_parse_analyze_script" = 'cq = PgInspect::Query.current_query; p cq.target_list';
SET

my_db=# select * from films;
 code | title | did | date_prod | kind | len
------+-------+-----+-----------+------+-----
(0 rows)
```


```shell
my_db=# set session "psql_inspect.set_rel_pathlist_script" = 'cp = PgInspect::PlannerInfo.current_planner_info; p cp.simple_rel_array.compact.map(&:pathlist)';
SET

my_db=# select * from films;
 code | title | did | date_prod | kind | len
------+-------+-----+-----------+------+-----
(0 rows)
```

```shell
my_db=# set session "psql_inspect.planner_script" = 'p [PgInspect::PlannedStmt.current_stmt.type, PgInspect::PlannedStmt.current_stmt.command_type]';
SET

my_db=# select * from films;
 code | title | did | date_prod | kind | len
------+-------+-----+-----------+------+-----
(0 rows)
```

```shell
my_db=# set session "psql_inspect.ExecutorRun_script" = 'qd = PgInspect::QueryDesc.current_query_desc; p qd.operation';
SET

my_db=# select * from films;
 code | title | did | date_prod | kind | len
------+-------+-----+-----------+------+-----
(0 rows)
```
