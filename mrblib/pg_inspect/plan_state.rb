class PgInspect
  class TupleDesc
    def _pretty_print(pp)
      pp.add_line "TupleDesc: natts: #{natts}"

      pp.with_indent_inc do
        pp.add_line "attrs"
        pp.with_indent_inc do
          attrs.each do |a|
            a._pretty_print(pp)
          end
        end
      end
    end
  end

  class PgAttribute
    def _pretty_print(pp)
      pp.add_line "PgAttribute: attname: #{attname}"
    end
  end

  class TupleTableSlot
    def _pretty_print(pp)
      pp.add_line "TupleTableSlot: tts_isempty: #{tts_isempty}, tts_fixedTupleDescriptor: #{tts_fixedTupleDescriptor}"

      pp.with_indent_inc do
        pp.add_line "tts_tupleDescriptor"
        pp.with_indent_inc do
          tts_tupleDescriptor._pretty_print(pp)
        end
      end
    end
  end

  class PlanState
    def _pretty_print(pp)
      pp.add_line "PlanState: #{type}"
    end
  end

  class ScanState < PlanState
  end

  class SeqScanState < ScanState
    def _pretty_print(pp)
      pp.add_line "SeqScanState:"

      pp.with_indent_inc do
        pp.add_line "qual"
        pp.with_indent_inc do
          qual._pretty_print(pp)
        end
      end

      pp.with_indent_inc do
        pp.add_line "ss_ScanTupleSlot"
        pp.with_indent_inc do
          ss_ScanTupleSlot._pretty_print(pp)
        end
      end
    end
  end
end
