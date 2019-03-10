class PgInspect
  class Plan
    def _pretty_print(pp)
      pp.add_line "#{self.class}:"

      pp.with_indent_inc do
        pp.add_line "targetlist"
        pp.with_indent_inc do
          targetlist.each do |t|
            t._pretty_print(pp)
          end
        end
      end

      if lefttree
        pp.with_indent_inc do
          pp.add_line "left"
          pp.with_indent_inc do
            lefttree._pretty_print(pp)
          end
        end
      end

      if righttree
        pp.with_indent_inc do
          pp.add_line "right"
          pp.with_indent_inc do
            righttree._pretty_print(pp)
          end
        end
      end
    end
  end

  class Agg < Plan
  end

  class SeqScan < Plan
  end

  class Sort < Plan
  end
end
