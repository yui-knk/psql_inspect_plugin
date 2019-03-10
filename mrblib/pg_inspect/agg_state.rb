class PgInspect
  class AggState
    def _pretty_print(pp)
      pp.add_line "AggState:"

      pp.with_indent_inc do
        phases.each do |phase|
          phase._pretty_print(pp)
        end
      end
    end
  end
end
