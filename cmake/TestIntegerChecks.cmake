function(run mode function_name parameter)
    execute_process(COMMAND "${PROGRAM}" "${mode}" "${INPUT}" --function "${function_name}"
        --param "${parameter}" --up 0 --down 0 ${ARGN} RESULT_VARIABLE code OUTPUT_VARIABLE output ERROR_VARIABLE error)
    if(NOT code EQUAL 0)
        message(FATAL_ERROR "Integer risk warnings must not fail execution: ${code}: ${error}")
    endif()
    set(actual "${output}" PARENT_SCOPE)
endfunction()
run(trace narrow64 0 --report-level 4)
if(NOT actual MATCHES "reason=SignedTruncationLoss feasibility=unverified" OR
   NOT actual MATCHES "reason=UnsignedTruncationLoss feasibility=unverified")
    message(FATAL_ERROR "Trace did not retain unverified truncation candidates: ${actual}")
endif()
run(verify narrow64 0 --report-level 5)
if(NOT actual MATCHES "Operand operand0 i64" OR NOT actual MATCHES "modular-bit-pattern=[0-9]+ i32 ir-value=poison" OR
   NOT actual MATCHES "reason=UnsignedTruncationLoss feasibility=boundary-triggerable")
    message(FATAL_ERROR "Truncation witness widths, flags or status missing: ${actual}")
endif()
run(verify logical32 1)
if(NOT actual MATCHES "reason=InvalidShiftAmount feasibility=boundary-triggerable" OR
   NOT actual MATCHES "no meaningful result bit pattern" OR actual MATCHES "modular-bit-pattern=")
    message(FATAL_ERROR "Invalid shift reported a fictitious result: ${actual}")
endif()
run(verify unknown_narrow 0 --show-status none)
if(actual MATCHES "Warning: ArithmeticRisk" OR NOT actual MATCHES "status=unknown total=2 shown=0 hidden=2" OR
   NOT actual MATCHES "Verification boundary")
    message(FATAL_ERROR "Hidden new risks lost unknown boundaries: ${actual}")
endif()
