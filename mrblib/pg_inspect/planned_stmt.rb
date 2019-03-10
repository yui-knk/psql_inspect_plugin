class PgInspect
  class PlannedStmt
    def _pretty_print(pp)
      pp.add_line "PlannedStmt: command_type: #{command_type}"

      pp.with_indent_inc do
        plan_tree._pretty_print(pp)
      end
    end
  end
end
