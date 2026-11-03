execute_process(
    COMMAND "${PROGRAM}" trace "${INPUT}" --function overloaded --up 0 --down 0
    INPUT_FILE "${CHOICES}"
    RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error)
if(NOT result EQUAL 0 OR NOT output MATCHES "Function candidates:" OR
   NOT output MATCHES "Choose function number:" OR NOT output MATCHES "Choose parameter numbers" OR
   NOT output MATCHES "Chosen parameter: count")
    message(FATAL_ERROR "Interactive selection failed: ${result}\n${output}\n${error}")
endif()

# EOF must never implicitly choose the first overload.
set(empty "${CMAKE_CURRENT_BINARY_DIR}/empty-selection-input.txt")
file(WRITE "${empty}" "")
execute_process(
    COMMAND "${PROGRAM}" trace "${INPUT}" --function overloaded --param-name count
    INPUT_FILE "${empty}"
    RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error)
if(result EQUAL 0 OR NOT error MATCHES "selection-required")
    message(FATAL_ERROR "EOF silently chose a candidate: ${result}\n${output}\n${error}")
endif()
