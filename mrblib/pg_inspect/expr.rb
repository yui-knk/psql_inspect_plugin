class PgInspect
  class Expr
    def _pretty_print(pp)
      pp.add_line "#{self.class} (#{type}):"
    end
  end

  class Aggref < Expr
    def _pretty_print(pp)
      pp.add_line "Aggref:"

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
      pp.add_line "Const: consttype: #{consttype}, constlen: #{constlen}, constvalue: #{constvalue}"
    end
  end

  class OpExpr < Expr
    def _pretty_print(pp)
      pp.add_line "OpExpr: opno: #{opno}, opfuncid: #{opfuncid}"

      pp.with_indent_inc do
        args.each do |arg|
          arg._pretty_print(pp)
        end
      end
    end
  end

  class RelabelType < Expr
    def _pretty_print(pp)
      pp.add_line "RelabelType:"

      pp.with_indent_inc do
        arg._pretty_print(pp)
      end
    end
  end

  class TargetEntry < Expr
    def _pretty_print(pp)
      pp.add_line "TargetEntry: resno: #{resno}, resname: #{resname}, resjunk: #{resjunk}"

      pp.with_indent_inc do
        expr._pretty_print(pp)
      end
    end
  end

  class Var < Expr
    def _pretty_print(pp)
      pp.add_line "Var: varno: #{varno}, varattno: #{varattno}"
    end
  end
end
