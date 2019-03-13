class PgInspect
  class Expr
    def _pretty_print(pp)
      pp.add_line "#{self.class} (#{type}):"
    end
  end

  class Aggref < Expr
    def _pretty_print(pp)
      super(pp)

      pp.with_indent_inc do
        pp.add_line "aggfnoid: #{aggfnoid}"

        args.each do |arg|
          arg._pretty_print(pp)
        end
      end
    end
  end

  class Const < Expr
    def _pretty_print(pp)
      pp.add_line "#{self.class}: type: #{consttype}, len: #{constlen}, value: #{constvalue}"
    end
  end

  class RelabelType < Expr
    def _pretty_print(pp)
      super(pp)

      pp.with_indent_inc do
        arg._pretty_print(pp)
      end
    end
  end

  class TargetEntry < Expr
    def _pretty_print(pp)
      pp.add_line "#{self.class}: resno: #{resno}, resname: #{resname}, resjunk: #{resjunk}"

      pp.with_indent_inc do
        expr._pretty_print(pp)
      end
    end
  end

  class Var < Expr
    def _pretty_print(pp)
      pp.add_line "#{self.class}: varno: #{varno}, varattno: #{varattno}"
    end
  end
end
