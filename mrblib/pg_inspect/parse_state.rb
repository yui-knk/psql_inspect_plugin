class PgInspect
  class ParseState
    def _pretty_print(pp)
      pp.add_line "ParseState:"

      pp.with_indent_inc do
        pp.add_line "sourcetext"
        pp.with_indent_inc do
          pp.add_line sourcetext
        end
      end

      pp.with_indent_inc do
        pp.add_line "parsetree_list"
        pp.with_indent_inc do
          parsetree_list.each do |pt|
            pt._pretty_print(pp)
          end
        end
      end
    end
  end
end
