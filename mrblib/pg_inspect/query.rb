class PgInspect
  class Query
    def _pretty_print(pp)
      pp.add_line "Query: #{command_type}(id: #{queryId})"

      pp.with_indent_inc do
        pp.add_line "rtable (# #{rtable.count})"
        pp.with_indent_inc do
          rtable.each do |r|
            r._pretty_print(pp)
          end
        end
      end

      pp.with_indent_inc do
        pp.add_line "jointree"
        pp.with_indent_inc do
          jointree._pretty_print(pp)
        end
      end

      pp.with_indent_inc do
        pp.add_line "target_list (# #{target_list.count})"
        pp.with_indent_inc do
          target_list.each do |t|
            t._pretty_print(pp)
          end
        end
      end
    end
  end
end
