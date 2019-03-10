class PgInspect
  class ExprState
    def _pretty_print(pp)
      ss = steps.map(&:opcode).join(", ")

      pp.add_line "ExprSate: #{ss}"
    end
  end
end
