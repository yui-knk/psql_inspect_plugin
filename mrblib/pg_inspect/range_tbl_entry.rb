class PgInspect
  class RangeTblEntry
    def _pretty_print(pp)
      pp.add_line "RangeTblEntry: rtekind: #{rtekind}, relid: #{relid}, relkind: #{relkind}"
    end
  end
end
