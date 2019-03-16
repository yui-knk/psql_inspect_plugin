class PgInspect
  class QueryDesc
    def _pretty_print(pp)
      pp.add_line "QueryDesc (#{operation}):"

      pp.with_indent_inc do
        pp.add_line "planstate"
        pp.with_indent_inc do
          planstate._pretty_print(pp)
        end
      end
    end
  end
end
