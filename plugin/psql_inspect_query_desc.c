#include "postgres.h"
#include "executor/execdesc.h"
#include "executor/execExpr.h"
#include "executor/nodeAgg.h"
#include "nodes/execnodes.h"

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/variable.h>

#include <psql_inspect_nodes.h>
#include <psql_inspect_query_desc.h>

static struct RClass *query_desc_class = NULL;

static struct RClass *agg_state_per_hash_class = NULL;
static struct RClass *agg_state_per_phase_class = NULL;

static struct RClass *expr_eval_step_class = NULL;

static struct RClass *expr_state_class = NULL;
static struct RClass *plan_state_class = NULL;
static struct RClass *scan_state_class = NULL;
static struct RClass *seq_scan_state_class = NULL;
static struct RClass *agg_state_class = NULL;

static struct RClass *tuple_table_slot_class = NULL;
static struct RClass *tuple_desc_class = NULL;

static const struct mrb_data_type psql_inspect_query_desc_data_type = { "QueryDesc", mrb_free };

static const struct mrb_data_type psql_inspect_agg_state_per_hash_data_type = { "AggStatePerHash", mrb_free };
static const struct mrb_data_type psql_inspect_expr_state_data_type = { "ExprState", mrb_free };

static const struct mrb_data_type psql_inspect_expr_eval_step_data_type = { "ExprEvalStep", mrb_free };

static const struct mrb_data_type psql_inspect_agg_state_per_phase_data_type = { "AggStatePerPhase", mrb_free };
static const struct mrb_data_type psql_inspect_plan_state_data_type = { "PlanState", mrb_free };
static const struct mrb_data_type psql_inspect_scan_state_data_type = { "ScanState", mrb_free };
static const struct mrb_data_type psql_inspect_seq_scan_state_data_type = { "SeqScanState", mrb_free };
static const struct mrb_data_type psql_inspect_agg_state_data_type = { "AggState", mrb_free };

static const struct mrb_data_type psql_inspect_tuple_table_slot_data_type = { "TupleTableSlot", mrb_free };
static const struct mrb_data_type psql_inspect_tuple_desc_data_type = { "TupleDesc", mrb_free };


typedef struct ExprEvalStepData {
    ExprState *exprState; /* Needed for ExecEvalStepOp function */
    ExprEvalStep *step;
} ExprEvalStepData_t;

static void
psql_inspect_planstate_set_planstate(mrb_state *mrb, mrb_value self, PlanState *planstate)
{
    DATA_PTR(self) = planstate;
}

static mrb_value
psql_inspect_planstate_build_from_planstate(mrb_state *mrb, PlanState *planstate)
{
    mrb_value val;

    switch (planstate->type) {
      case T_AggState:
        val = mrb_class_new_instance(mrb, 0, NULL, agg_state_class);
        break;
      case T_SeqScanState:
        val = mrb_class_new_instance(mrb, 0, NULL, seq_scan_state_class);
        break;
      default:
        val = mrb_class_new_instance(mrb, 0, NULL, plan_state_class);
    }

    psql_inspect_planstate_set_planstate(mrb, val, planstate);

    return val;
}

static mrb_value
psql_inspect_agg_state_per_phase_build_from_agg_state_per_phase(mrb_state *mrb, AggStatePerPhase phase)
{
    mrb_value val;

    val = mrb_class_new_instance(mrb, 0, NULL, agg_state_per_phase_class);
    DATA_PTR(val) = phase;

    return val;
}

static mrb_value
psql_inspect_TupleDesc_build_from_TupleDesc(mrb_state *mrb, TupleDesc tupdesc)
{
    mrb_value val;

    val = mrb_class_new_instance(mrb, 0, NULL, tuple_desc_class);
    DATA_PTR(val) = tupdesc;

    return val;
}

static mrb_value
psql_inspect_ScanTupleSlot_build_from_ScanTupleSlot(mrb_state *mrb, TupleTableSlot *tuple)
{
    mrb_value val;

    val = mrb_class_new_instance(mrb, 0, NULL, tuple_table_slot_class);
    DATA_PTR(val) = tuple;

    return val;
}

static mrb_value
psql_inspect_expr_eval_step_build_from_expr_eval_step(mrb_state *mrb, ExprEvalStep *step, ExprState *exprState)
{
    mrb_value val;
    ExprEvalStepData_t *data;

    data = (ExprEvalStepData_t *)mrb_malloc(mrb, sizeof(ExprEvalStepData_t));
    data->step = step;
    data->exprState = exprState;

    val = mrb_class_new_instance(mrb, 0, NULL, expr_eval_step_class);
    DATA_PTR(val) = data;

    return val;
}

static void
psql_inspect_query_desc_set_query_desc(mrb_state *mrb, mrb_value self, QueryDesc *queryDesc)
{
    DATA_PTR(self) = queryDesc;
}

static mrb_value
psql_inspect_c_current_query_desc(mrb_state *mrb, mrb_value klass)
{
    return mrb_cv_get(mrb, klass, mrb_intern_lit(mrb, "query_desc"));
}

static mrb_value
psql_inspect_query_desc_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_query_desc_data_type;

    return self;
}

static mrb_value
psql_inspect_tuple_table_slot_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_tuple_table_slot_data_type;

    return self;
}

static mrb_value
psql_inspect_tuple_desc_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_tuple_desc_data_type;

    return self;
}

static mrb_value
psql_inspect_agg_state_per_hash_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_agg_state_per_hash_data_type;

    return self;
}

static mrb_value
psql_inspect_expr_state_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_expr_state_data_type;

    return self;
}

static mrb_value
psql_inspect_expr_eval_step_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_expr_eval_step_data_type;

    return self;
}

static mrb_value
psql_inspect_agg_state_per_phase_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_agg_state_per_phase_data_type;

    return self;
}

static mrb_value
psql_inspect_plan_state_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_plan_state_data_type;

    return self;
}

static mrb_value
psql_inspect_scan_state_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_scan_state_data_type;

    return self;
}

static mrb_value
psql_inspect_seq_scan_state_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_seq_scan_state_data_type;

    return self;
}

static mrb_value
psql_inspect_agg_state_init(mrb_state *mrb, mrb_value self)
{
    DATA_TYPE(self) = &psql_inspect_agg_state_data_type;

    return self;
}

static mrb_value
psql_inspect_query_desc_operation(mrb_state *mrb, mrb_value self)
{
    QueryDesc *queryDesc;

    queryDesc = (QueryDesc *)DATA_PTR(self);
    return psql_inspect_mrb_str_from_CmdType(mrb, queryDesc->operation);
}

static mrb_value
psql_inspect_query_desc_planstate(mrb_state *mrb, mrb_value self)
{
    QueryDesc *queryDesc;

    queryDesc = (QueryDesc *)DATA_PTR(self);
    return psql_inspect_planstate_build_from_planstate(mrb, queryDesc->planstate);
}

static mrb_value
psql_inspect_tuple_table_slot_tts_isempty(mrb_state *mrb, mrb_value self)
{
    TupleTableSlot *tuple;

    tuple = (TupleTableSlot *)DATA_PTR(self);
    return mrb_bool_value(tuple->tts_isempty);
}

static mrb_value
psql_inspect_tuple_table_slot_tts_shouldFree(mrb_state *mrb, mrb_value self)
{
    TupleTableSlot *tuple;

    tuple = (TupleTableSlot *)DATA_PTR(self);
    return mrb_bool_value(tuple->tts_shouldFree);
}

static mrb_value
psql_inspect_tuple_table_slot_tts_shouldFreeMin(mrb_state *mrb, mrb_value self)
{
    TupleTableSlot *tuple;

    tuple = (TupleTableSlot *)DATA_PTR(self);
    return mrb_bool_value(tuple->tts_shouldFreeMin);
}

static mrb_value
psql_inspect_tuple_table_slot_tts_slow(mrb_state *mrb, mrb_value self)
{
    TupleTableSlot *tuple;

    tuple = (TupleTableSlot *)DATA_PTR(self);
    return mrb_bool_value(tuple->tts_slow);
}

static mrb_value
psql_inspect_tuple_table_slot_tts_tupleDescriptor(mrb_state *mrb, mrb_value self)
{
    TupleTableSlot *tuple;

    tuple = (TupleTableSlot *)DATA_PTR(self);
    return psql_inspect_TupleDesc_build_from_TupleDesc(mrb, tuple->tts_tupleDescriptor);
}

static mrb_value
psql_inspect_tuple_table_slot_tts_fixedTupleDescriptor(mrb_state *mrb, mrb_value self)
{
    TupleTableSlot *tuple;

    tuple = (TupleTableSlot *)DATA_PTR(self);
    return mrb_bool_value(tuple->tts_fixedTupleDescriptor);
}

static mrb_value
psql_inspect_tuple_desc_natts(mrb_state *mrb, mrb_value self)
{
    TupleDesc tupdesc;

    tupdesc = (TupleDesc)DATA_PTR(self);
    return mrb_fixnum_value(tupdesc->natts);
}

static mrb_value
psql_inspect_expr_state_node_tag(mrb_state *mrb, mrb_value self)
{
    ExprState *exprState;

    exprState = (ExprState *)DATA_PTR(self);
    return psql_inspect_mrb_str_from_NodeTag(mrb, exprState->tag.type);
}

static mrb_value
psql_inspect_expr_state_steps(mrb_state *mrb, mrb_value self)
{
    ExprState *exprState;
    mrb_value ary;

    exprState = (ExprState *)DATA_PTR(self);
    ary = mrb_ary_new_capa(mrb, exprState->steps_len);

    for (int i = 0; i < exprState->steps_len; i++) {
        mrb_value v;

        v = psql_inspect_expr_eval_step_build_from_expr_eval_step(mrb, &exprState->steps[i], exprState);
        mrb_ary_set(mrb, ary, i, v);
    }

    return ary;
}

static mrb_value
psql_inspect_plan_state_type(mrb_state *mrb, mrb_value self)
{
    Plan *plan;

    plan = (Plan *)DATA_PTR(self);
    return psql_inspect_mrb_str_from_NodeTag(mrb, plan->type);
}

static mrb_value
psql_inspect_scan_state_ss_ScanTupleSlot(mrb_state *mrb, mrb_value self)
{
    ScanState *ss;

    ss = (ScanState *)DATA_PTR(self);
    return psql_inspect_ScanTupleSlot_build_from_ScanTupleSlot(mrb, ss->ss_ScanTupleSlot);
}

static void
psql_inspect_agg_state_per_hash_set_agg_state_per_hash(mrb_state *mrb, mrb_value self, AggStatePerHash perhash)
{
    DATA_PTR(self) = perhash;
}

static mrb_value
psql_inspect_expr_state_build_from_expr_state(mrb_state *mrb, ExprState *exprState)
{
    mrb_value val;

    val = mrb_class_new_instance(mrb, 0, NULL, expr_state_class);
    DATA_PTR(val) = exprState;

    return val;
}

static mrb_value
psql_inspect_agg_state_per_hash_build_from_agg_state_per_hash(mrb_state *mrb, AggStatePerHash perhash)
{
    mrb_value val;

    val = mrb_class_new_instance(mrb, 0, NULL, agg_state_per_hash_class);
    psql_inspect_agg_state_per_hash_set_agg_state_per_hash(mrb, val, perhash);

    return val;
}

static mrb_value
psql_inspect_agg_state_per_hash_numCols(mrb_state *mrb, mrb_value self)
{
    AggStatePerHash perhash;

    perhash = (AggStatePerHash)DATA_PTR(self);
    return mrb_fixnum_value(perhash->numCols);
}

static mrb_value
psql_inspect_agg_state_per_hash_numhashGrpCols(mrb_state *mrb, mrb_value self)
{
    AggStatePerHash perhash;

    perhash = (AggStatePerHash)DATA_PTR(self);
    return mrb_fixnum_value(perhash->numhashGrpCols);
}

static mrb_value
psql_inspect_agg_state_per_hash_largestGrpColIdx(mrb_state *mrb, mrb_value self)
{
    AggStatePerHash perhash;

    perhash = (AggStatePerHash)DATA_PTR(self);
    return mrb_fixnum_value(perhash->largestGrpColIdx);
}

static mrb_value
psql_inspect_agg_state_per_hash_hashGrpColIdxInput(mrb_state *mrb, mrb_value self)
{
    AggStatePerHash perhash;
    mrb_value ary;

    perhash = (AggStatePerHash)DATA_PTR(self);
    ary = mrb_ary_new_capa(mrb, perhash->numhashGrpCols);

    for (int i = 0; i < perhash->numhashGrpCols; i++) {
        mrb_value v;

        v = mrb_fixnum_value(perhash->hashGrpColIdxInput[i]);
        mrb_ary_set(mrb, ary, i, v);
    }

    return ary;
}

static mrb_value
psql_inspect_agg_state_per_hash_hashGrpColIdxHash(mrb_state *mrb, mrb_value self)
{
    AggStatePerHash perhash;
    mrb_value ary;

    perhash = (AggStatePerHash)DATA_PTR(self);
    ary = mrb_ary_new_capa(mrb, perhash->numCols);

    for (int i = 0; i < perhash->numCols; i++) {
        mrb_value v;

        v = mrb_fixnum_value(perhash->hashGrpColIdxHash[i]);
        mrb_ary_set(mrb, ary, i, v);
    }

    return ary;
}

static mrb_value
psql_inspect_mrb_str_from_ExprEvalOp(mrb_state *mrb, ExprEvalOp op)
{
#define EEOP_TYPE(strategy) \
    EEOP_##strategy: return mrb_str_new_cstr(mrb, #strategy);

    switch (op) {
      /* entire expression has been evaluated completely, return */
      case EEOP_TYPE(DONE);

      /* apply slot_getsomeattrs on corresponding tuple slot */
      case EEOP_TYPE(INNER_FETCHSOME);
      case EEOP_TYPE(OUTER_FETCHSOME);
      case EEOP_TYPE(SCAN_FETCHSOME);

      /* compute non-system Var value */
      case EEOP_TYPE(INNER_VAR);
      case EEOP_TYPE(OUTER_VAR);
      case EEOP_TYPE(SCAN_VAR);

      /* compute system Var value */
      case EEOP_TYPE(INNER_SYSVAR);
      case EEOP_TYPE(OUTER_SYSVAR);
      case EEOP_TYPE(SCAN_SYSVAR);

      /* compute wholerow Var */
      case EEOP_TYPE(WHOLEROW);

      /*
       * Compute non-system Var value, assign it into ExprState's resultslot.
       * These are not used if a CheckVarSlotCompatibility() check would be
       * needed.
       */
      case EEOP_TYPE(ASSIGN_INNER_VAR);
      case EEOP_TYPE(ASSIGN_OUTER_VAR);
      case EEOP_TYPE(ASSIGN_SCAN_VAR);

      /* assign ExprState's resvalue/resnull to a column of its resultslot */
      case EEOP_TYPE(ASSIGN_TMP);
      /* ditto, applying MakeExpandedObjectReadOnly() */
      case EEOP_TYPE(ASSIGN_TMP_MAKE_RO);

      /* evaluate Const value */
      case EEOP_TYPE(CONST);

      /*
       * Evaluate function call (including OpExprs etc).  For speed, we
       * distinguish in the opcode whether the function is strict and/or
       * requires usage stats tracking.
       */
      case EEOP_TYPE(FUNCEXPR);
      case EEOP_TYPE(FUNCEXPR_STRICT);
      case EEOP_TYPE(FUNCEXPR_FUSAGE);
      case EEOP_TYPE(FUNCEXPR_STRICT_FUSAGE);

      /*
       * Evaluate boolean AND expression, one step per subexpression. FIRST/LAST
       * subexpressions are special-cased for performance.  Since AND always has
       * at least two subexpressions, FIRST and LAST never apply to the same
       * subexpression.
       */
      case EEOP_TYPE(BOOL_AND_STEP_FIRST);
      case EEOP_TYPE(BOOL_AND_STEP);
      case EEOP_TYPE(BOOL_AND_STEP_LAST);

      /* similarly for boolean OR expression */
      case EEOP_TYPE(BOOL_OR_STEP_FIRST);
      case EEOP_TYPE(BOOL_OR_STEP);
      case EEOP_TYPE(BOOL_OR_STEP_LAST);

      /* evaluate boolean NOT expression */
      case EEOP_TYPE(BOOL_NOT_STEP);

      /* simplified version of BOOL_AND_STEP for use by ExecQual() */
      case EEOP_TYPE(QUAL);

      /* unconditional jump to another step */
      case EEOP_TYPE(JUMP);

      /* conditional jumps based on current result value */
      case EEOP_TYPE(JUMP_IF_NULL);
      case EEOP_TYPE(JUMP_IF_NOT_NULL);
      case EEOP_TYPE(JUMP_IF_NOT_TRUE);

      /* perform NULL tests for scalar values */
      case EEOP_TYPE(NULLTEST_ISNULL);
      case EEOP_TYPE(NULLTEST_ISNOTNULL);

      /* perform NULL tests for row values */
      case EEOP_TYPE(NULLTEST_ROWISNULL);
      case EEOP_TYPE(NULLTEST_ROWISNOTNULL);

      /* evaluate a BooleanTest expression */
      case EEOP_TYPE(BOOLTEST_IS_TRUE);
      case EEOP_TYPE(BOOLTEST_IS_NOT_TRUE);
      case EEOP_TYPE(BOOLTEST_IS_FALSE);
      case EEOP_TYPE(BOOLTEST_IS_NOT_FALSE);

      /* evaluate PARAM_EXEC/EXTERN parameters */
      case EEOP_TYPE(PARAM_EXEC);
      case EEOP_TYPE(PARAM_EXTERN);
      case EEOP_TYPE(PARAM_CALLBACK);

      /* return CaseTestExpr value */
      case EEOP_TYPE(CASE_TESTVAL);

      /* apply MakeExpandedObjectReadOnly() to target value */
      case EEOP_TYPE(MAKE_READONLY);

      /* evaluate assorted special-purpose expression types */
      case EEOP_TYPE(IOCOERCE);
      case EEOP_TYPE(DISTINCT);
      case EEOP_TYPE(NOT_DISTINCT);
      case EEOP_TYPE(NULLIF);
      case EEOP_TYPE(SQLVALUEFUNCTION);
      case EEOP_TYPE(CURRENTOFEXPR);
      case EEOP_TYPE(NEXTVALUEEXPR);
      case EEOP_TYPE(ARRAYEXPR);
      case EEOP_TYPE(ARRAYCOERCE);
      case EEOP_TYPE(ROW);

      /*
       * Compare two individual elements of each of two compared ROW()
       * expressions.  Skip to ROWCOMPARE_FINAL if elements are not equal.
       */
      case EEOP_TYPE(ROWCOMPARE_STEP);

      /* evaluate boolean value based on previous ROWCOMPARE_STEP operations */
      case EEOP_TYPE(ROWCOMPARE_FINAL);

      /* evaluate GREATEST() or LEAST() */
      case EEOP_TYPE(MINMAX);

      /* evaluate FieldSelect expression */
      case EEOP_TYPE(FIELDSELECT);

      /*
       * Deform tuple before evaluating new values for individual fields in a
       * FieldStore expression.
       */
      case EEOP_TYPE(FIELDSTORE_DEFORM);

      /*
       * Form the new tuple for a FieldStore expression.  Individual fields will
       * have been evaluated into columns of the tuple deformed by the preceding
       * DEFORM step.
       */
      case EEOP_TYPE(FIELDSTORE_FORM);

      /* Process an array subscript; short-circuit expression to NULL if NULL */
      case EEOP_TYPE(ARRAYREF_SUBSCRIPT);

      /*
       * Compute old array element/slice when an ArrayRef assignment expression
       * contains ArrayRef/FieldStore subexpressions.  Value is accessed using
       * the CaseTest mechanism.
       */
      case EEOP_TYPE(ARRAYREF_OLD);

      /* compute new value for ArrayRef assignment expression */
      case EEOP_TYPE(ARRAYREF_ASSIGN);

      /* compute element/slice for ArrayRef fetch expression */
      case EEOP_TYPE(ARRAYREF_FETCH);

      /* evaluate value for CoerceToDomainValue */
      case EEOP_TYPE(DOMAIN_TESTVAL);

      /* evaluate a domain's NOT NULL constraint */
      case EEOP_TYPE(DOMAIN_NOTNULL);

      /* evaluate a single domain CHECK constraint */
      case EEOP_TYPE(DOMAIN_CHECK);

      /* evaluate assorted special-purpose expression types */
      case EEOP_TYPE(CONVERT_ROWTYPE);
      case EEOP_TYPE(SCALARARRAYOP);
      case EEOP_TYPE(XMLEXPR);
      case EEOP_TYPE(AGGREF);
      case EEOP_TYPE(GROUPING_FUNC);
      case EEOP_TYPE(WINDOW_FUNC);
      case EEOP_TYPE(SUBPLAN);
      case EEOP_TYPE(ALTERNATIVE_SUBPLAN);

      /* aggregation related nodes */
      case EEOP_TYPE(AGG_STRICT_DESERIALIZE);
      case EEOP_TYPE(AGG_DESERIALIZE);
      case EEOP_TYPE(AGG_STRICT_INPUT_CHECK);
      case EEOP_TYPE(AGG_INIT_TRANS);
      case EEOP_TYPE(AGG_STRICT_TRANS_CHECK);
      case EEOP_TYPE(AGG_PLAIN_TRANS_BYVAL);
      case EEOP_TYPE(AGG_PLAIN_TRANS);
      case EEOP_TYPE(AGG_ORDERED_TRANS_DATUM);
      case EEOP_TYPE(AGG_ORDERED_TRANS_TUPLE);

      /* non-existent operation, used e.g. to check array lengths */
      case EEOP_TYPE(LAST);
#undef EEOP_TYPE
      default:
        mrb_raisef(mrb, E_RUNTIME_ERROR, "Unknown ExprEvalOp number: %S", mrb_fixnum_value(op));
    }
}

static mrb_value
psql_inspect_expr_eval_step_opcode(mrb_state *mrb, mrb_value self)
{
    ExprEvalStep *step;
    ExprEvalStepData_t *data;
    ExprState *exprState;
    ExprEvalOp op;

    data = (ExprEvalStepData_t *)DATA_PTR(self);
    step = data->step;
    exprState = data->exprState;

    /*
     * When direct-threading is in use, ExprState->opcode isn't easily
     * decipherable. This function returns the appropriate enum member.
     */
    op = ExecEvalStepOp(exprState, step);

    return psql_inspect_mrb_str_from_ExprEvalOp(mrb, op);
}

static mrb_value
psql_inspect_agg_state_per_phase_evaltrans(mrb_state *mrb, mrb_value self)
{
    AggStatePerPhase phase;

    phase = (AggStatePerPhase)DATA_PTR(self);
    return psql_inspect_expr_state_build_from_expr_state(mrb, phase->evaltrans);
}

static mrb_value
psql_inspect_agg_state_numaggs(mrb_state *mrb, mrb_value self)
{
    AggState *aggstate;

    aggstate = (AggState *)DATA_PTR(self);
    return mrb_fixnum_value(aggstate->numaggs);
}

static mrb_value
psql_inspect_agg_state_numtrans(mrb_state *mrb, mrb_value self)
{
    AggState *aggstate;

    aggstate = (AggState *)DATA_PTR(self);
    return mrb_fixnum_value(aggstate->numtrans);
}

static mrb_value
psql_inspect_agg_state_numphases(mrb_state *mrb, mrb_value self)
{
    AggState *aggstate;

    aggstate = (AggState *)DATA_PTR(self);
    return mrb_fixnum_value(aggstate->numphases);
}

static mrb_value
psql_inspect_agg_state_current_phase(mrb_state *mrb, mrb_value self)
{
    AggState *aggstate;

    aggstate = (AggState *)DATA_PTR(self);
    return mrb_fixnum_value(aggstate->current_phase);
}

static mrb_value
psql_inspect_agg_state_current_set(mrb_state *mrb, mrb_value self)
{
    AggState *aggstate;

    aggstate = (AggState *)DATA_PTR(self);
    return mrb_fixnum_value(aggstate->current_set);
}

static mrb_value
psql_inspect_agg_state_phases(mrb_state *mrb, mrb_value self)
{
    AggState *aggstate;
    mrb_value ary;

    aggstate = (AggState *)DATA_PTR(self);
    ary = mrb_ary_new_capa(mrb, aggstate->numphases);

    for (int i = 0; i < aggstate->numphases; i++) {
        mrb_value v;

        v = psql_inspect_agg_state_per_phase_build_from_agg_state_per_phase(mrb, &aggstate->phases[i]);
        mrb_ary_set(mrb, ary, i, v);
    }

    return ary;
}

static mrb_value
psql_inspect_agg_state_num_hashes(mrb_state *mrb, mrb_value self)
{
    AggState *aggstate;

    aggstate = (AggState *)DATA_PTR(self);
    return mrb_fixnum_value(aggstate->num_hashes);
}

static mrb_value
psql_inspect_agg_state_perhash(mrb_state *mrb, mrb_value self)
{
    AggState *aggstate;

    aggstate = (AggState *)DATA_PTR(self);
    return psql_inspect_agg_state_per_hash_build_from_agg_state_per_hash(mrb, aggstate->perhash);
}

void
psql_inspect_query_desc_mruby_env_setup(mrb_state *mrb, QueryDesc *queryDesc)
{
    /* TODO: Ensure mrb and query_desc_class are set. */
    mrb_value obj = mrb_obj_new(mrb, query_desc_class, 0, NULL);
    psql_inspect_query_desc_set_query_desc(mrb, obj, queryDesc);
    mrb_mod_cv_set(mrb, query_desc_class, mrb_intern_lit(mrb, "query_desc"), obj);
}

void
psql_inspect_query_desc_mruby_env_tear_down(mrb_state *mrb)
{
    mrb_mod_cv_set(mrb, query_desc_class, mrb_intern_lit(mrb, "query_desc"), mrb_nil_value());
}

void
psql_inspect_query_desc_fini(mrb_state *mrb)
{
    query_desc_class = NULL;
}

void
psql_inspect_query_desc_class_init(mrb_state *mrb, struct RClass *class)
{
    /* QueryDesc class */
    query_desc_class = mrb_define_class_under(mrb, class, "QueryDesc", mrb->object_class);
    MRB_SET_INSTANCE_TT(query_desc_class, MRB_TT_DATA);

    mrb_define_class_method(mrb, query_desc_class, "current_query_desc", psql_inspect_c_current_query_desc, MRB_ARGS_NONE());
    mrb_define_method(mrb, query_desc_class, "initialize", psql_inspect_query_desc_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, query_desc_class, "operation", psql_inspect_query_desc_operation, MRB_ARGS_NONE());
    mrb_define_method(mrb, query_desc_class, "planstate", psql_inspect_query_desc_planstate, MRB_ARGS_NONE());

    /* TODO: Move this class to another file */
    /* TupleTableSlot */
    tuple_table_slot_class = mrb_define_class_under(mrb, class, "TupleTableSlot", mrb->object_class);
    mrb_define_method(mrb, tuple_table_slot_class, "initialize", psql_inspect_tuple_table_slot_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, tuple_table_slot_class, "tts_isempty", psql_inspect_tuple_table_slot_tts_isempty, MRB_ARGS_NONE());
    mrb_define_method(mrb, tuple_table_slot_class, "tts_shouldFree", psql_inspect_tuple_table_slot_tts_shouldFree, MRB_ARGS_NONE());
    mrb_define_method(mrb, tuple_table_slot_class, "tts_shouldFreeMin", psql_inspect_tuple_table_slot_tts_shouldFreeMin, MRB_ARGS_NONE());
    mrb_define_method(mrb, tuple_table_slot_class, "tts_slow", psql_inspect_tuple_table_slot_tts_slow, MRB_ARGS_NONE());
    mrb_define_method(mrb, tuple_table_slot_class, "tts_tupleDescriptor", psql_inspect_tuple_table_slot_tts_tupleDescriptor, MRB_ARGS_NONE());
    mrb_define_method(mrb, tuple_table_slot_class, "tts_fixedTupleDescriptor", psql_inspect_tuple_table_slot_tts_fixedTupleDescriptor, MRB_ARGS_NONE());

    /* TODO: Move this class to another file */
    /* TupleDesc */
    tuple_desc_class = mrb_define_class_under(mrb, class, "TupleDesc", mrb->object_class);
    mrb_define_method(mrb, tuple_desc_class, "initialize", psql_inspect_tuple_desc_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, tuple_desc_class, "natts", psql_inspect_tuple_desc_natts, MRB_ARGS_NONE());

    /* ExprState */
    expr_state_class = mrb_define_class_under(mrb, class, "ExprState", mrb->object_class);
    MRB_SET_INSTANCE_TT(expr_state_class, MRB_TT_DATA);

    mrb_define_method(mrb, expr_state_class, "initialize", psql_inspect_expr_state_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, expr_state_class, "node_tag", psql_inspect_expr_state_node_tag, MRB_ARGS_NONE());
    mrb_define_method(mrb, expr_state_class, "steps", psql_inspect_expr_state_steps, MRB_ARGS_NONE());

    /* PlanState is base class */
    plan_state_class = mrb_define_class_under(mrb, class, "PlanState", mrb->object_class);
    MRB_SET_INSTANCE_TT(plan_state_class, MRB_TT_DATA);

    mrb_define_method(mrb, plan_state_class, "initialize", psql_inspect_plan_state_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, plan_state_class, "type", psql_inspect_plan_state_type, MRB_ARGS_NONE());

    /* ScanState */
    scan_state_class = mrb_define_class_under(mrb, class, "ScanState", plan_state_class);
    MRB_SET_INSTANCE_TT(scan_state_class, MRB_TT_DATA);

    mrb_define_method(mrb, scan_state_class, "initialize", psql_inspect_scan_state_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, scan_state_class, "ss_ScanTupleSlot", psql_inspect_scan_state_ss_ScanTupleSlot, MRB_ARGS_NONE());

    /* SeqScanState */
    seq_scan_state_class = mrb_define_class_under(mrb, class, "SeqScanState", scan_state_class);
    MRB_SET_INSTANCE_TT(seq_scan_state_class, MRB_TT_DATA);

    mrb_define_method(mrb, seq_scan_state_class, "initialize", psql_inspect_seq_scan_state_init, MRB_ARGS_NONE());

    /* AggStatePerHash */
    agg_state_per_hash_class = mrb_define_class_under(mrb, class, "AggStatePerHash", mrb->object_class);
    MRB_SET_INSTANCE_TT(agg_state_per_hash_class, MRB_TT_DATA);

    mrb_define_method(mrb, agg_state_per_hash_class, "initialize", psql_inspect_agg_state_per_hash_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, agg_state_per_hash_class, "numCols", psql_inspect_agg_state_per_hash_numCols, MRB_ARGS_NONE());
    mrb_define_method(mrb, agg_state_per_hash_class, "numhashGrpCols", psql_inspect_agg_state_per_hash_numhashGrpCols, MRB_ARGS_NONE());
    mrb_define_method(mrb, agg_state_per_hash_class, "largestGrpColIdx", psql_inspect_agg_state_per_hash_largestGrpColIdx, MRB_ARGS_NONE());
    mrb_define_method(mrb, agg_state_per_hash_class, "hashGrpColIdxInput", psql_inspect_agg_state_per_hash_hashGrpColIdxInput, MRB_ARGS_NONE());
    mrb_define_method(mrb, agg_state_per_hash_class, "hashGrpColIdxHash", psql_inspect_agg_state_per_hash_hashGrpColIdxHash, MRB_ARGS_NONE());

    /* ExprEvalStep */
    expr_eval_step_class = mrb_define_class_under(mrb, class, "ExprEvalStep", mrb->object_class);
    MRB_SET_INSTANCE_TT(expr_eval_step_class, MRB_TT_DATA);

    mrb_define_method(mrb, expr_eval_step_class, "initialize", psql_inspect_expr_eval_step_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, expr_eval_step_class, "opcode", psql_inspect_expr_eval_step_opcode, MRB_ARGS_NONE());

    /* AggStatePerPhase */
    agg_state_per_phase_class = mrb_define_class_under(mrb, class, "AggStatePerPhase", mrb->object_class);
    MRB_SET_INSTANCE_TT(agg_state_per_phase_class, MRB_TT_DATA);

    mrb_define_method(mrb, agg_state_per_phase_class, "initialize", psql_inspect_agg_state_per_phase_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, agg_state_per_phase_class, "evaltrans", psql_inspect_agg_state_per_phase_evaltrans, MRB_ARGS_NONE());

    /* AggState */
    agg_state_class = mrb_define_class_under(mrb, class, "AggState", mrb->object_class);
    MRB_SET_INSTANCE_TT(agg_state_class, MRB_TT_DATA);

    mrb_define_method(mrb, agg_state_class, "initialize", psql_inspect_agg_state_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, agg_state_class, "numaggs", psql_inspect_agg_state_numaggs, MRB_ARGS_NONE());
    mrb_define_method(mrb, agg_state_class, "numtrans", psql_inspect_agg_state_numtrans, MRB_ARGS_NONE());
    mrb_define_method(mrb, agg_state_class, "numphases", psql_inspect_agg_state_numphases, MRB_ARGS_NONE());
    mrb_define_method(mrb, agg_state_class, "current_phase", psql_inspect_agg_state_current_phase, MRB_ARGS_NONE());
    mrb_define_method(mrb, agg_state_class, "current_set", psql_inspect_agg_state_current_set, MRB_ARGS_NONE());
    mrb_define_method(mrb, agg_state_class, "phases", psql_inspect_agg_state_phases, MRB_ARGS_NONE());
    mrb_define_method(mrb, agg_state_class, "num_hashes", psql_inspect_agg_state_num_hashes, MRB_ARGS_NONE());
    mrb_define_method(mrb, agg_state_class, "perhash", psql_inspect_agg_state_perhash, MRB_ARGS_NONE());
}
