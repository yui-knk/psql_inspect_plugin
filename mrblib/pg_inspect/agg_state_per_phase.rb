class PgInspect
  class AggStatePerPhase
    def _pretty_print(pp)
      pp.add_line "AggStatePerPhase:"

      pp.with_indent_inc do
        evaltrans._pretty_print(pp)
      end
    end
  end
end
