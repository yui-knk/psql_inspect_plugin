class PgInspect
  class Node
    def _pretty_print(pp)
      pp.add_line "Node: #{type}"
    end
  end

  class FromExpr < Node
    def _pretty_print(pp)
      pp.add_line "FromExpr:"

      if quals
        pp.with_indent_inc do
          pp.add_line "quals"
          pp.with_indent_inc do
            quals._pretty_print(pp)
          end
        end
      end
    end
  end

  class RawStmt < Node
    def _pretty_print(pp)
      pp.add_line "RawStmt:"

      pp.with_indent_inc do
        pp.add_line "stmt"
        pp.with_indent_inc do
          stmt._pretty_print(pp)
        end
      end
    end
  end

  class RangeVar < Node
    def _pretty_print(pp)
      pp.add_line "RangeVar:"

      pp.with_indent_inc do
        pp.add_line "catalogname: #{catalogname}"
      end

      pp.with_indent_inc do
        pp.add_line "schemaname: #{schemaname}"
      end

      pp.with_indent_inc do
        pp.add_line "relname: #{relname}"
      end
    end
  end

  class A_Expr < Node
    def _pretty_print(pp)
      pp.add_line "A_Expr: kind: #{kind}, name: #{name.join(', ')}"

      if lexpr
        pp.with_indent_inc do
          pp.add_line "lexpr"
          pp.with_indent_inc do
            lexpr._pretty_print(pp)
          end
        end
      end

      if rexpr
        pp.with_indent_inc do
          pp.add_line "rexpr"
          pp.with_indent_inc do
            rexpr._pretty_print(pp)
          end
        end
      end
    end
  end

  class A_Const < Node
    def _pretty_print(pp)
      pp.add_line "A_Const: val: #{val}"
    end
  end

  class ColumnRef < Node
    def _pretty_print(pp)
      pp.add_line "ColumnRef: fields: #{fields.join(', ')}"
    end
  end

  class ResTarget < Node
    def _pretty_print(pp)
      pp.add_line "ResTarget: name: #{name}, indirection: #{indirection.join(', ')}"

      if val
        pp.with_indent_inc do
          pp.add_line "val"
          pp.with_indent_inc do
            val._pretty_print(pp)
          end
        end
      end
    end
  end

  class SelectStmt < Node
    def _pretty_print(pp)
      pp.add_line "SelectStmt:"

      pp.with_indent_inc do
        pp.add_line "targetList"
        pp.with_indent_inc do
          targetList.each do |f|
            f._pretty_print(pp)
          end
        end
      end

      pp.with_indent_inc do
        pp.add_line "fromClause"
        pp.with_indent_inc do
          fromClause.each do |f|
            f._pretty_print(pp)
          end
        end
      end

      if whereClause
        pp.with_indent_inc do
          pp.add_line "whereClause"
          pp.with_indent_inc do
            whereClause._pretty_print(pp)
          end
        end
      end
    end
  end

  class PlannedStmt < Node
    def _pretty_print(pp)
      pp.add_line "PlannedStmt: command_type: #{command_type}"

      pp.with_indent_inc do
        plan_tree._pretty_print(pp)
      end
    end
  end
end
