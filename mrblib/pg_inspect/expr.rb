class PgInspect
  class Expr
    def _pretty_print(pp)
      pp.add_line "#{self.class}:"
    end
  end

  class Aggref < Expr
  end

  class Const < Expr
  end

  class RelabelType < Expr
  end

  class TargetEntry < Expr
  end

  class Var < Expr
  end
end
