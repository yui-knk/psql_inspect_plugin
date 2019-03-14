class PgInspect
  class Query
    def _pretty_print(pp)
      pp.add_line "Query: #{command_type}(#{queryId})"

      pp.with_indent_inc do
        pp.add_line "jointree"
        pp.with_indent_inc do
          jointree._pretty_print(pp)
        end
      end
    end
  end
end
