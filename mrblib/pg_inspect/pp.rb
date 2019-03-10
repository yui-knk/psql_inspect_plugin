class PgInspect
  def self._pp(obj)
    buf = "" 
    pp = PP.new(buf)
    obj._pretty_print(pp)

    buf
  end

  class PP
    def initialize(buffre, indent: 0, indent_str: "  ")
      @buffre = buffre
      @indent = indent
      @indent_str= indent_str
    end

    def add_str(str)
      @buffre << "#{i}#{str}"
    end

    def add_line(str)
      @buffre << "#{i}#{str}\n"
    end

    def with_indent_inc
      @indent += 1
      yield
      @indent -= 1
    end

    private

    def i
      @indent_str * @indent
    end
  end
end
