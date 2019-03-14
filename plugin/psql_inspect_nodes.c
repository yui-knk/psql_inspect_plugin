#include "postgres.h"
#include "nodes/nodes.h"

#include <mruby.h>
#include <mruby/class.h>
#include <mruby/data.h>

#include <psql_inspect_nodes.h>

struct RClass *psql_inspect_node_class = NULL;
static const struct mrb_data_type psql_inspect_node_data_type = { "Node", mrb_free };

static mrb_value
psql_inspect_node_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_node_data_type;

    return self;
}

mrb_value
psql_inspect_mrb_str_from_NodeTag(mrb_state *mrb, NodeTag type)
{

#define T_TYPE(type) \
    T_##type: return mrb_str_new_cstr(mrb, #type);

    switch (type) {
      case T_TYPE(Invalid);
      case T_TYPE(IndexInfo);
      case T_TYPE(ExprContext);
      case T_TYPE(ProjectionInfo);
      case T_TYPE(JunkFilter);
      case T_TYPE(OnConflictSetState);
      case T_TYPE(ResultRelInfo);
      case T_TYPE(EState);
      case T_TYPE(TupleTableSlot);
      case T_TYPE(Plan);
      case T_TYPE(Result);
      case T_TYPE(ProjectSet);
      case T_TYPE(ModifyTable);
      case T_TYPE(Append);
      case T_TYPE(MergeAppend);
      case T_TYPE(RecursiveUnion);
      case T_TYPE(BitmapAnd);
      case T_TYPE(BitmapOr);
      case T_TYPE(Scan);
      case T_TYPE(SeqScan);
      case T_TYPE(SampleScan);
      case T_TYPE(IndexScan);
      case T_TYPE(IndexOnlyScan);
      case T_TYPE(BitmapIndexScan);
      case T_TYPE(BitmapHeapScan);
      case T_TYPE(TidScan);
      case T_TYPE(SubqueryScan);
      case T_TYPE(FunctionScan);
      case T_TYPE(ValuesScan);
      case T_TYPE(TableFuncScan);
      case T_TYPE(CteScan);
      case T_TYPE(NamedTuplestoreScan);
      case T_TYPE(WorkTableScan);
      case T_TYPE(ForeignScan);
      case T_TYPE(CustomScan);
      case T_TYPE(Join);
      case T_TYPE(NestLoop);
      case T_TYPE(MergeJoin);
      case T_TYPE(HashJoin);
      case T_TYPE(Material);
      case T_TYPE(Sort);
      case T_TYPE(Group);
      case T_TYPE(Agg);
      case T_TYPE(WindowAgg);
      case T_TYPE(Unique);
      case T_TYPE(Gather);
      case T_TYPE(GatherMerge);
      case T_TYPE(Hash);
      case T_TYPE(SetOp);
      case T_TYPE(LockRows);
      case T_TYPE(Limit);
      case T_TYPE(NestLoopParam);
      case T_TYPE(PlanRowMark);
      case T_TYPE(PartitionPruneInfo);
      case T_TYPE(PartitionedRelPruneInfo);
      case T_TYPE(PartitionPruneStepOp);
      case T_TYPE(PartitionPruneStepCombine);
      case T_TYPE(PlanInvalItem);
      case T_TYPE(PlanState);
      case T_TYPE(ResultState);
      case T_TYPE(ProjectSetState);
      case T_TYPE(ModifyTableState);
      case T_TYPE(AppendState);
      case T_TYPE(MergeAppendState);
      case T_TYPE(RecursiveUnionState);
      case T_TYPE(BitmapAndState);
      case T_TYPE(BitmapOrState);
      case T_TYPE(ScanState);
      case T_TYPE(SeqScanState);
      case T_TYPE(SampleScanState);
      case T_TYPE(IndexScanState);
      case T_TYPE(IndexOnlyScanState);
      case T_TYPE(BitmapIndexScanState);
      case T_TYPE(BitmapHeapScanState);
      case T_TYPE(TidScanState);
      case T_TYPE(SubqueryScanState);
      case T_TYPE(FunctionScanState);
      case T_TYPE(TableFuncScanState);
      case T_TYPE(ValuesScanState);
      case T_TYPE(CteScanState);
      case T_TYPE(NamedTuplestoreScanState);
      case T_TYPE(WorkTableScanState);
      case T_TYPE(ForeignScanState);
      case T_TYPE(CustomScanState);
      case T_TYPE(JoinState);
      case T_TYPE(NestLoopState);
      case T_TYPE(MergeJoinState);
      case T_TYPE(HashJoinState);
      case T_TYPE(MaterialState);
      case T_TYPE(SortState);
      case T_TYPE(GroupState);
      case T_TYPE(AggState);
      case T_TYPE(WindowAggState);
      case T_TYPE(UniqueState);
      case T_TYPE(GatherState);
      case T_TYPE(GatherMergeState);
      case T_TYPE(HashState);
      case T_TYPE(SetOpState);
      case T_TYPE(LockRowsState);
      case T_TYPE(LimitState);
      case T_TYPE(Alias);
      case T_TYPE(RangeVar);
      case T_TYPE(TableFunc);
      case T_TYPE(Expr);
      case T_TYPE(Var);
      case T_TYPE(Const);
      case T_TYPE(Param);
      case T_TYPE(Aggref);
      case T_TYPE(GroupingFunc);
      case T_TYPE(WindowFunc);
      case T_TYPE(ArrayRef);
      case T_TYPE(FuncExpr);
      case T_TYPE(NamedArgExpr);
      case T_TYPE(OpExpr);
      case T_TYPE(DistinctExpr);
      case T_TYPE(NullIfExpr);
      case T_TYPE(ScalarArrayOpExpr);
      case T_TYPE(BoolExpr);
      case T_TYPE(SubLink);
      case T_TYPE(SubPlan);
      case T_TYPE(AlternativeSubPlan);
      case T_TYPE(FieldSelect);
      case T_TYPE(FieldStore);
      case T_TYPE(RelabelType);
      case T_TYPE(CoerceViaIO);
      case T_TYPE(ArrayCoerceExpr);
      case T_TYPE(ConvertRowtypeExpr);
      case T_TYPE(CollateExpr);
      case T_TYPE(CaseExpr);
      case T_TYPE(CaseWhen);
      case T_TYPE(CaseTestExpr);
      case T_TYPE(ArrayExpr);
      case T_TYPE(RowExpr);
      case T_TYPE(RowCompareExpr);
      case T_TYPE(CoalesceExpr);
      case T_TYPE(MinMaxExpr);
      case T_TYPE(SQLValueFunction);
      case T_TYPE(XmlExpr);
      case T_TYPE(NullTest);
      case T_TYPE(BooleanTest);
      case T_TYPE(CoerceToDomain);
      case T_TYPE(CoerceToDomainValue);
      case T_TYPE(SetToDefault);
      case T_TYPE(CurrentOfExpr);
      case T_TYPE(NextValueExpr);
      case T_TYPE(InferenceElem);
      case T_TYPE(TargetEntry);
      case T_TYPE(RangeTblRef);
      case T_TYPE(JoinExpr);
      case T_TYPE(FromExpr);
      case T_TYPE(OnConflictExpr);
      case T_TYPE(IntoClause);
      case T_TYPE(ExprState);
      case T_TYPE(AggrefExprState);
      case T_TYPE(WindowFuncExprState);
      case T_TYPE(SetExprState);
      case T_TYPE(SubPlanState);
      case T_TYPE(AlternativeSubPlanState);
      case T_TYPE(DomainConstraintState);
      case T_TYPE(PlannerInfo);
      case T_TYPE(PlannerGlobal);
      case T_TYPE(RelOptInfo);
      case T_TYPE(IndexOptInfo);
      case T_TYPE(ForeignKeyOptInfo);
      case T_TYPE(ParamPathInfo);
      case T_TYPE(Path);
      case T_TYPE(IndexPath);
      case T_TYPE(BitmapHeapPath);
      case T_TYPE(BitmapAndPath);
      case T_TYPE(BitmapOrPath);
      case T_TYPE(TidPath);
      case T_TYPE(SubqueryScanPath);
      case T_TYPE(ForeignPath);
      case T_TYPE(CustomPath);
      case T_TYPE(NestPath);
      case T_TYPE(MergePath);
      case T_TYPE(HashPath);
      case T_TYPE(AppendPath);
      case T_TYPE(MergeAppendPath);
      case T_TYPE(ResultPath);
      case T_TYPE(MaterialPath);
      case T_TYPE(UniquePath);
      case T_TYPE(GatherPath);
      case T_TYPE(GatherMergePath);
      case T_TYPE(ProjectionPath);
      case T_TYPE(ProjectSetPath);
      case T_TYPE(SortPath);
      case T_TYPE(GroupPath);
      case T_TYPE(UpperUniquePath);
      case T_TYPE(AggPath);
      case T_TYPE(GroupingSetsPath);
      case T_TYPE(MinMaxAggPath);
      case T_TYPE(WindowAggPath);
      case T_TYPE(SetOpPath);
      case T_TYPE(RecursiveUnionPath);
      case T_TYPE(LockRowsPath);
      case T_TYPE(ModifyTablePath);
      case T_TYPE(LimitPath);
      case T_TYPE(EquivalenceClass);
      case T_TYPE(EquivalenceMember);
      case T_TYPE(PathKey);
      case T_TYPE(PathTarget);
      case T_TYPE(RestrictInfo);
      case T_TYPE(PlaceHolderVar);
      case T_TYPE(SpecialJoinInfo);
      case T_TYPE(AppendRelInfo);
      case T_TYPE(PlaceHolderInfo);
      case T_TYPE(MinMaxAggInfo);
      case T_TYPE(PlannerParamItem);
      case T_TYPE(RollupData);
      case T_TYPE(GroupingSetData);
      case T_TYPE(StatisticExtInfo);
      case T_TYPE(MemoryContext);
      case T_TYPE(AllocSetContext);
      case T_TYPE(SlabContext);
      case T_TYPE(GenerationContext);
      case T_TYPE(Value);
      case T_TYPE(Integer);
      case T_TYPE(Float);
      case T_TYPE(String);
      case T_TYPE(BitString);
      case T_TYPE(Null);
      case T_TYPE(List);
      case T_TYPE(IntList);
      case T_TYPE(OidList);
      case T_TYPE(ExtensibleNode);
      case T_TYPE(RawStmt);
      case T_TYPE(Query);
      case T_TYPE(PlannedStmt);
      case T_TYPE(InsertStmt);
      case T_TYPE(DeleteStmt);
      case T_TYPE(UpdateStmt);
      case T_TYPE(SelectStmt);
      case T_TYPE(AlterTableStmt);
      case T_TYPE(AlterTableCmd);
      case T_TYPE(AlterDomainStmt);
      case T_TYPE(SetOperationStmt);
      case T_TYPE(GrantStmt);
      case T_TYPE(GrantRoleStmt);
      case T_TYPE(AlterDefaultPrivilegesStmt);
      case T_TYPE(ClosePortalStmt);
      case T_TYPE(ClusterStmt);
      case T_TYPE(CopyStmt);
      case T_TYPE(CreateStmt);
      case T_TYPE(DefineStmt);
      case T_TYPE(DropStmt);
      case T_TYPE(TruncateStmt);
      case T_TYPE(CommentStmt);
      case T_TYPE(FetchStmt);
      case T_TYPE(IndexStmt);
      case T_TYPE(CreateFunctionStmt);
      case T_TYPE(AlterFunctionStmt);
      case T_TYPE(DoStmt);
      case T_TYPE(RenameStmt);
      case T_TYPE(RuleStmt);
      case T_TYPE(NotifyStmt);
      case T_TYPE(ListenStmt);
      case T_TYPE(UnlistenStmt);
      case T_TYPE(TransactionStmt);
      case T_TYPE(ViewStmt);
      case T_TYPE(LoadStmt);
      case T_TYPE(CreateDomainStmt);
      case T_TYPE(CreatedbStmt);
      case T_TYPE(DropdbStmt);
      case T_TYPE(VacuumStmt);
      case T_TYPE(ExplainStmt);
      case T_TYPE(CreateTableAsStmt);
      case T_TYPE(CreateSeqStmt);
      case T_TYPE(AlterSeqStmt);
      case T_TYPE(VariableSetStmt);
      case T_TYPE(VariableShowStmt);
      case T_TYPE(DiscardStmt);
      case T_TYPE(CreateTrigStmt);
      case T_TYPE(CreatePLangStmt);
      case T_TYPE(CreateRoleStmt);
      case T_TYPE(AlterRoleStmt);
      case T_TYPE(DropRoleStmt);
      case T_TYPE(LockStmt);
      case T_TYPE(ConstraintsSetStmt);
      case T_TYPE(ReindexStmt);
      case T_TYPE(CheckPointStmt);
      case T_TYPE(CreateSchemaStmt);
      case T_TYPE(AlterDatabaseStmt);
      case T_TYPE(AlterDatabaseSetStmt);
      case T_TYPE(AlterRoleSetStmt);
      case T_TYPE(CreateConversionStmt);
      case T_TYPE(CreateCastStmt);
      case T_TYPE(CreateOpClassStmt);
      case T_TYPE(CreateOpFamilyStmt);
      case T_TYPE(AlterOpFamilyStmt);
      case T_TYPE(PrepareStmt);
      case T_TYPE(ExecuteStmt);
      case T_TYPE(DeallocateStmt);
      case T_TYPE(DeclareCursorStmt);
      case T_TYPE(CreateTableSpaceStmt);
      case T_TYPE(DropTableSpaceStmt);
      case T_TYPE(AlterObjectDependsStmt);
      case T_TYPE(AlterObjectSchemaStmt);
      case T_TYPE(AlterOwnerStmt);
      case T_TYPE(AlterOperatorStmt);
      case T_TYPE(DropOwnedStmt);
      case T_TYPE(ReassignOwnedStmt);
      case T_TYPE(CompositeTypeStmt);
      case T_TYPE(CreateEnumStmt);
      case T_TYPE(CreateRangeStmt);
      case T_TYPE(AlterEnumStmt);
      case T_TYPE(AlterTSDictionaryStmt);
      case T_TYPE(AlterTSConfigurationStmt);
      case T_TYPE(CreateFdwStmt);
      case T_TYPE(AlterFdwStmt);
      case T_TYPE(CreateForeignServerStmt);
      case T_TYPE(AlterForeignServerStmt);
      case T_TYPE(CreateUserMappingStmt);
      case T_TYPE(AlterUserMappingStmt);
      case T_TYPE(DropUserMappingStmt);
      case T_TYPE(AlterTableSpaceOptionsStmt);
      case T_TYPE(AlterTableMoveAllStmt);
      case T_TYPE(SecLabelStmt);
      case T_TYPE(CreateForeignTableStmt);
      case T_TYPE(ImportForeignSchemaStmt);
      case T_TYPE(CreateExtensionStmt);
      case T_TYPE(AlterExtensionStmt);
      case T_TYPE(AlterExtensionContentsStmt);
      case T_TYPE(CreateEventTrigStmt);
      case T_TYPE(AlterEventTrigStmt);
      case T_TYPE(RefreshMatViewStmt);
      case T_TYPE(ReplicaIdentityStmt);
      case T_TYPE(AlterSystemStmt);
      case T_TYPE(CreatePolicyStmt);
      case T_TYPE(AlterPolicyStmt);
      case T_TYPE(CreateTransformStmt);
      case T_TYPE(CreateAmStmt);
      case T_TYPE(CreatePublicationStmt);
      case T_TYPE(AlterPublicationStmt);
      case T_TYPE(CreateSubscriptionStmt);
      case T_TYPE(AlterSubscriptionStmt);
      case T_TYPE(DropSubscriptionStmt);
      case T_TYPE(CreateStatsStmt);
      case T_TYPE(AlterCollationStmt);
      case T_TYPE(CallStmt);
      case T_TYPE(A_Expr);
      case T_TYPE(ColumnRef);
      case T_TYPE(ParamRef);
      case T_TYPE(A_Const);
      case T_TYPE(FuncCall);
      case T_TYPE(A_Star);
      case T_TYPE(A_Indices);
      case T_TYPE(A_Indirection);
      case T_TYPE(A_ArrayExpr);
      case T_TYPE(ResTarget);
      case T_TYPE(MultiAssignRef);
      case T_TYPE(TypeCast);
      case T_TYPE(CollateClause);
      case T_TYPE(SortBy);
      case T_TYPE(WindowDef);
      case T_TYPE(RangeSubselect);
      case T_TYPE(RangeFunction);
      case T_TYPE(RangeTableSample);
      case T_TYPE(RangeTableFunc);
      case T_TYPE(RangeTableFuncCol);
      case T_TYPE(TypeName);
      case T_TYPE(ColumnDef);
      case T_TYPE(IndexElem);
      case T_TYPE(Constraint);
      case T_TYPE(DefElem);
      case T_TYPE(RangeTblEntry);
      case T_TYPE(RangeTblFunction);
      case T_TYPE(TableSampleClause);
      case T_TYPE(WithCheckOption);
      case T_TYPE(SortGroupClause);
      case T_TYPE(GroupingSet);
      case T_TYPE(WindowClause);
      case T_TYPE(ObjectWithArgs);
      case T_TYPE(AccessPriv);
      case T_TYPE(CreateOpClassItem);
      case T_TYPE(TableLikeClause);
      case T_TYPE(FunctionParameter);
      case T_TYPE(LockingClause);
      case T_TYPE(RowMarkClause);
      case T_TYPE(XmlSerialize);
      case T_TYPE(WithClause);
      case T_TYPE(InferClause);
      case T_TYPE(OnConflictClause);
      case T_TYPE(CommonTableExpr);
      case T_TYPE(RoleSpec);
      case T_TYPE(TriggerTransition);
      case T_TYPE(PartitionElem);
      case T_TYPE(PartitionSpec);
      case T_TYPE(PartitionBoundSpec);
      case T_TYPE(PartitionRangeDatum);
      case T_TYPE(PartitionCmd);
      case T_TYPE(VacuumRelation);
      case T_TYPE(IdentifySystemCmd);
      case T_TYPE(BaseBackupCmd);
      case T_TYPE(CreateReplicationSlotCmd);
      case T_TYPE(DropReplicationSlotCmd);
      case T_TYPE(StartReplicationCmd);
      case T_TYPE(TimeLineHistoryCmd);
      case T_TYPE(SQLCmd);
      case T_TYPE(TriggerData);
      case T_TYPE(EventTriggerData);
      case T_TYPE(ReturnSetInfo);
      case T_TYPE(WindowObjectData);
      case T_TYPE(TIDBitmap);
      case T_TYPE(InlineCodeBlock);
      case T_TYPE(FdwRoutine);
      case T_TYPE(IndexAmRoutine);
      case T_TYPE(TsmRoutine);
      case T_TYPE(ForeignKeyCacheInfo);
      case T_TYPE(CallContext);
#undef T_TYPE
      default:
        mrb_raisef(mrb, E_RUNTIME_ERROR, "Unknown node tag number: %S", mrb_fixnum_value(type));
    }
}

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
        mrb_raisef(mrb, E_RUNTIME_ERROR, "Unknown command type number: %S", mrb_fixnum_value(type));
    }
}

mrb_value
psql_inspect_mrb_str_from_AggStrategy(mrb_state *mrb, AggStrategy strategy)
{

#define AGG_TYPE(strategy) \
    AGG_##strategy: return mrb_str_new_cstr(mrb, #strategy);

    switch (strategy) {
      case AGG_TYPE(PLAIN);
      case AGG_TYPE(SORTED);
      case AGG_TYPE(HASHED);
      case AGG_TYPE(MIXED);
#undef AGG_TYPE
      default:
        mrb_raisef(mrb, E_RUNTIME_ERROR, "Unknown command strategy number: %S", mrb_fixnum_value(strategy));
    }
}

static mrb_value
psql_inspect_node_type(mrb_state *mrb, mrb_value self)
{
    Node *node;

    node = (Node *)DATA_PTR(self);
    return psql_inspect_mrb_str_from_NodeTag(mrb, node->type);
}

mrb_value
psql_inspect_node_build_from_node(mrb_state *mrb, Node *node)
{
    mrb_value val;

    val = mrb_class_new_instance(mrb, 0, NULL, psql_inspect_node_class);
    DATA_PTR(val) = node;

    return val;
}


void
psql_inspect_node_fini(mrb_state *mrb)
{
    psql_inspect_node_class = NULL;
}

void
psql_inspect_node_class_init(mrb_state *mrb, struct RClass *class)
{
    /* Node class */
    psql_inspect_node_class = mrb_define_class_under(mrb, class, "Node", mrb->object_class);
    MRB_SET_INSTANCE_TT(psql_inspect_node_class, MRB_TT_DATA);

    mrb_define_method(mrb, psql_inspect_node_class, "initialize", psql_inspect_node_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, psql_inspect_node_class, "type", psql_inspect_node_type, MRB_ARGS_NONE());
}

