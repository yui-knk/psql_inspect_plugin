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

  class SelectStmt < Node
    def _pretty_print(pp)
      pp.add_line "SelectStmt:"

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
end
