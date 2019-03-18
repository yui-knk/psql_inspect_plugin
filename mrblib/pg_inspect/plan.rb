class PgInspect
  class Plan
    def _pretty_print(pp)
      pp.add_line "#{type}:"

      _pp_common(pp)
    end

    def _pp_common(pp)
      pp.with_indent_inc do
        pp.add_line "targetlist"
        pp.with_indent_inc do
          targetlist.each do |t|
            t._pretty_print(pp)
          end
        end
      end

      pp.with_indent_inc do
        pp.add_line "qual"
        pp.with_indent_inc do
          qual.each do |q|
            q._pretty_print(pp)
          end
        end
      end

      if lefttree
        pp.with_indent_inc do
          pp.add_line "lefttree"
          pp.with_indent_inc do
            lefttree._pretty_print(pp)
          end
        end
      end

      if righttree
        pp.with_indent_inc do
          pp.add_line "righttree"
          pp.with_indent_inc do
            righttree._pretty_print(pp)
          end
        end
      end
    end
  end

  class Agg < Plan
    def _pretty_print(pp)
      pp.add_line "Agg: aggstrategy: #{aggstrategy}, num_cols: #{num_cols}, grp_col_idx: #{grp_col_idx}"

      _pp_common(pp)

      pp.with_indent_inc do
        pp.add_line "chain"
        pp.with_indent_inc do
          chain.each do |c|
            c._pretty_print(pp)
          end
        end
      end
    end
  end

  class SeqScan < Plan
  end

  class Sort < Plan
    def _pretty_print(pp)
      pp.add_line "Sort: num_cols: #{num_cols}, sort_col_idx: #{sort_col_idx}, sort_operators: #{sort_operators}"

      _pp_common(pp)
    end
  end
end
