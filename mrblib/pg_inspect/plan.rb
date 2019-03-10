class PgInspect
  class Plan
    def _pretty_print(pp)
      pp.add_line "#{self.class}:"

      pp.add_line "left"
      pp.with_indent_inc do
        lefttree._pretty_print(pp)
      end

      pp.add_line "right"
      pp.with_indent_inc do
        righttree._pretty_print(pp)
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
