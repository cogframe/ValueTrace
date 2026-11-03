cmake_minimum_required(VERSION 3.24)
project(ValueTrace VERSION 0.4.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(VALUETRACE_WITH_LLVM "Read LLVM IR and bitcode through LLVM" ON)
if(VALUETRACE_WITH_LLVM)
    if(MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded)
        include(cmake/WindowsLlvmDependencies.cmake)
    endif()
    find_package(LLVM CONFIG REQUIRED
        HINTS "${CMAKE_SOURCE_DIR}/third_party/llvm-23.1.4/lib/cmake/llvm")
    if(NOT LLVM_VERSION_MAJOR EQUAL 23)
        message(FATAL_ERROR "ValueTrace currently targets LLVM 23.x")
    endif()
    if(WIN32 AND NOT MSVC AND LLVM_HOST_TRIPLE MATCHES "msvc")
        message(FATAL_ERROR "This LLVM package requires MSVC ABI. Use scripts/build-msvc.cmd.")
    endif()
    # Match the runtime used by the official Windows LLVM development package.
    if(MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded)
    endif()
endif()

add_library(valuetrace_core
    cpp/src/DependencyGraph.cpp
    cpp/src/Slice.cpp
    cpp/src/ReportFilter.cpp
    cpp/src/ArithmeticReport.cpp
)
target_include_directories(valuetrace_core PUBLIC cpp/include)
if(MSVC)
    target_compile_options(valuetrace_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(valuetrace_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(valuetrace cpp/app/main.cpp)
target_link_libraries(valuetrace PRIVATE valuetrace_core)

if(VALUETRACE_WITH_LLVM)
    add_library(valuetrace_llvm cpp/src/LlvmReader.cpp cpp/src/ParameterSelection.cpp
        cpp/src/ProgramInput.cpp cpp/src/ModuleLoader.cpp cpp/src/SourceTypes.cpp)
    target_include_directories(valuetrace_llvm PUBLIC cpp/include)
    target_include_directories(valuetrace_llvm SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS})
    separate_arguments(VT_LLVM_DEFINITIONS NATIVE_COMMAND "${LLVM_DEFINITIONS}")
    target_compile_options(valuetrace_llvm PRIVATE ${VT_LLVM_DEFINITIONS})
    if(NOT LLVM_ENABLE_RTTI)
        if(MSVC)
            target_compile_options(valuetrace_llvm PRIVATE /GR-)
        else()
            target_compile_options(valuetrace_llvm PRIVATE -fno-rtti)
        endif()
    endif()
    llvm_map_components_to_libnames(VT_LLVM_LIBS Core IRReader Support Demangle Linker TransformUtils)
    target_link_libraries(valuetrace_llvm PUBLIC valuetrace_core PRIVATE ${VT_LLVM_LIBS})
    target_link_libraries(valuetrace PRIVATE valuetrace_llvm)
    target_sources(valuetrace PRIVATE cpp/app/SelectionMenu.cpp)
    target_compile_definitions(valuetrace PRIVATE VALUETRACE_WITH_LLVM=1)
    include(cmake/NativeZ3.cmake)
    add_library(valuetrace_solver cpp/src/PathSolver.cpp cpp/src/IntegerExpressions.cpp
        cpp/src/RiskConditions.cpp cpp/src/RiskVerifier.cpp)
    target_include_directories(valuetrace_solver PUBLIC cpp/include)
    target_include_directories(valuetrace_solver SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS})
    target_compile_options(valuetrace_solver PRIVATE ${VT_LLVM_DEFINITIONS})
    if(NOT LLVM_ENABLE_RTTI)
        if(MSVC)
            target_compile_options(valuetrace_solver PRIVATE /GR-)
        else()
            target_compile_options(valuetrace_solver PRIVATE -fno-rtti)
        endif()
    endif()
    target_link_libraries(valuetrace_solver PUBLIC valuetrace_llvm PRIVATE valuetrace_z3 ${VT_LLVM_LIBS})
    target_link_libraries(valuetrace PRIVATE valuetrace_solver)
    target_sources(valuetrace PRIVATE cpp/app/SolveOutput.cpp)
    valuetrace_copy_z3(valuetrace)
endif()

include(CTest)
if(BUILD_TESTING)
    add_executable(valuetrace_core_tests cpp/tests/SliceTests.cpp)
    target_link_libraries(valuetrace_core_tests PRIVATE valuetrace_core)
    add_test(NAME core_slice COMMAND valuetrace_core_tests)
    add_executable(valuetrace_report_tests cpp/tests/ReportFilterTests.cpp)
    target_link_libraries(valuetrace_report_tests PRIVATE valuetrace_core)
    add_test(NAME report_filter COMMAND valuetrace_report_tests)
    add_test(NAME cli_demo COMMAND valuetrace demo --up 1 --down 1)
    add_test(NAME cli_invalid_depth COMMAND valuetrace demo --up -1)
    set_tests_properties(cli_invalid_depth PROPERTIES WILL_FAIL TRUE)
    if(VALUETRACE_WITH_LLVM)
        find_program(VT_CLANGXX NAMES clang++ HINTS "${LLVM_TOOLS_BINARY_DIR}" NO_DEFAULT_PATH REQUIRED)
        find_program(VT_OPT NAMES opt HINTS "${LLVM_TOOLS_BINARY_DIR}" NO_DEFAULT_PATH REQUIRED)
        function(vt_prepared_fixture output source)
            cmake_parse_arguments(P "" "DEFINE;DEBUG;EXTRA" "" ${ARGN})
            add_custom_command(OUTPUT "${output}"
                COMMAND ${CMAKE_COMMAND} "-DVT_CLANG=${VT_CLANGXX}" "-DVT_OPT=${VT_OPT}"
                    "-DVT_SOURCE=${CMAKE_CURRENT_SOURCE_DIR}/${source}" "-DVT_OUTPUT=${output}"
                    "-DVT_DEFINE=${P_DEFINE}" "-DVT_DEBUG=${P_DEBUG}" "-DVT_EXTRA=${P_EXTRA}"
                    -P "${CMAKE_CURRENT_SOURCE_DIR}/scripts/prepare-ir.cmake"
                DEPENDS "${source}" scripts/prepare-ir.cmake VERBATIM)
        endfunction()
        set(VT_SAMPLE_LL "${CMAKE_CURRENT_BINARY_DIR}/fixtures/parameter_chain.ll")
        set(VT_SAMPLE_BC "${CMAKE_CURRENT_BINARY_DIR}/fixtures/parameter_chain.bc")
        set(VT_SAMPLE_NODEBUG "${CMAKE_CURRENT_BINARY_DIR}/fixtures/parameter_chain_nodebug.ll")
        vt_prepared_fixture("${VT_SAMPLE_LL}" examples/parameter_chain.cpp)
        vt_prepared_fixture("${VT_SAMPLE_BC}" examples/parameter_chain.cpp)
        vt_prepared_fixture("${VT_SAMPLE_NODEBUG}" examples/parameter_chain.cpp DEBUG OFF)
        add_custom_target(valuetrace_fixtures DEPENDS "${VT_SAMPLE_LL}" "${VT_SAMPLE_BC}" "${VT_SAMPLE_NODEBUG}")
        add_executable(valuetrace_llvm_tests cpp/tests/LlvmReaderTests.cpp)
        target_link_libraries(valuetrace_llvm_tests PRIVATE valuetrace_llvm)
        add_dependencies(valuetrace_llvm_tests valuetrace_fixtures)
        add_test(NAME llvm_reader COMMAND valuetrace_llvm_tests
            "${VT_SAMPLE_LL}" "${VT_SAMPLE_BC}" "${CMAKE_CURRENT_SOURCE_DIR}/cpp/tests/fixtures" "${VT_SAMPLE_NODEBUG}")
        add_test(NAME cli_trace_ll COMMAND valuetrace trace "${VT_SAMPLE_LL}" --function B --param 0 --up 1 --down 1)
        set_tests_properties(cli_trace_ll PROPERTIES PASS_REGULAR_EXPRESSION "depth=1")
        add_test(NAME cli_trace_bc COMMAND valuetrace trace "${VT_SAMPLE_BC}" --function B --param 0 --up 0 --down 0)
        set_tests_properties(cli_trace_bc PROPERTIES PASS_REGULAR_EXPRESSION "depth boundary:")
        add_test(NAME cli_missing_parameter COMMAND valuetrace trace "${VT_SAMPLE_LL}" --function B --param 99)
        set_tests_properties(cli_missing_parameter PROPERTIES WILL_FAIL TRUE)
        add_test(NAME cli_arithmetic_warning COMMAND valuetrace trace
            "${CMAKE_CURRENT_SOURCE_DIR}/cpp/tests/fixtures/semantics.ll"
            --function risk_cases --param 0 --up 0 --down 0 --report-level 4)
        set_tests_properties(cli_arithmetic_warning PROPERTIES
            PASS_REGULAR_EXPRESSION "Warning: ArithmeticRisk reason=UnsignedUnderflow feasibility=unverified")
        add_test(NAME cli_report_filter COMMAND ${CMAKE_COMMAND}
            "-DPROGRAM=$<TARGET_FILE:valuetrace>"
            "-DINPUT=${CMAKE_CURRENT_SOURCE_DIR}/cpp/tests/fixtures/semantics.ll"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/TestReportFiltering.cmake")

        set(VT_NAMED_LL "${CMAKE_CURRENT_BINARY_DIR}/fixtures/named_parameters.ll")
        vt_prepared_fixture("${VT_NAMED_LL}" examples/named_parameters.cpp)
        add_custom_target(valuetrace_named_fixture DEPENDS "${VT_NAMED_LL}")
        add_executable(valuetrace_selection_tests cpp/tests/ParameterSelectionTests.cpp)
        target_link_libraries(valuetrace_selection_tests PRIVATE valuetrace_llvm)
        add_dependencies(valuetrace_selection_tests valuetrace_fixtures valuetrace_named_fixture)
        add_test(NAME parameter_selection COMMAND valuetrace_selection_tests
            "${VT_SAMPLE_LL}" "${VT_SAMPLE_BC}" "${VT_SAMPLE_NODEBUG}" "${VT_NAMED_LL}"
            "${CMAKE_CURRENT_SOURCE_DIR}/cpp/tests/fixtures/parameter_mappings.ll")
        add_test(NAME cli_parameter_name COMMAND valuetrace trace "${VT_SAMPLE_BC}" --function B --param-name count --up 0 --down 0)
        set_tests_properties(cli_parameter_name PROPERTIES PASS_REGULAR_EXPRESSION "Source parameter 'count' -> LLVM argument 0")
        add_test(NAME cli_inspect_this COMMAND valuetrace inspect "${VT_NAMED_LL}" --function Worker::process)
        set_tests_properties(cli_inspect_this PROPERTIES PASS_REGULAR_EXPRESSION "this.*implicit.*not traceable")
        add_test(NAME cli_explicit_choices COMMAND valuetrace trace "${VT_NAMED_LL}" --function overloaded --function-choice 2 --param-choice 1 --up 0 --down 0)
        set_tests_properties(cli_explicit_choices PROPERTIES PASS_REGULAR_EXPRESSION "Chosen parameter: count")
        add_test(NAME cli_interactive_choices COMMAND ${CMAKE_COMMAND}
            "-DPROGRAM=$<TARGET_FILE:valuetrace>" "-DINPUT=${VT_NAMED_LL}"
            "-DCHOICES=${CMAKE_CURRENT_SOURCE_DIR}/cpp/tests/fixtures/selection-input.txt"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/TestSelectionMenu.cmake")

        set(VT_GUARDED_LL "${CMAKE_CURRENT_BINARY_DIR}/fixtures/path_guarded.ll")
        set(VT_UNGUARDED_LL "${CMAKE_CURRENT_BINARY_DIR}/fixtures/path_unguarded.ll")
        set(VT_GUARDED_BC "${CMAKE_CURRENT_BINARY_DIR}/fixtures/path_guarded.bc")
        vt_prepared_fixture("${VT_GUARDED_LL}" examples/path_chain.cpp DEFINE GUARDED=1)
        vt_prepared_fixture("${VT_UNGUARDED_LL}" examples/path_chain.cpp DEFINE GUARDED=0)
        vt_prepared_fixture("${VT_GUARDED_BC}" examples/path_chain.cpp DEFINE GUARDED=1)
        add_custom_target(valuetrace_path_fixtures DEPENDS "${VT_GUARDED_LL}" "${VT_UNGUARDED_LL}" "${VT_GUARDED_BC}")
        add_executable(valuetrace_solver_tests cpp/tests/PathSolverTests.cpp)
        target_link_libraries(valuetrace_solver_tests PRIVATE valuetrace_solver)
        add_dependencies(valuetrace_solver_tests valuetrace_path_fixtures)
        valuetrace_copy_z3(valuetrace_solver_tests)
        add_executable(valuetrace_witness_replay examples/path_chain.cpp cpp/tests/ReplayMain.cpp)
        target_compile_definitions(valuetrace_witness_replay PRIVATE GUARDED=0)
        add_test(NAME path_solver COMMAND valuetrace_solver_tests "${VT_GUARDED_LL}" "${VT_UNGUARDED_LL}"
            "${VT_GUARDED_BC}" "${CMAKE_CURRENT_SOURCE_DIR}/cpp/tests/fixtures/paths.ll")
        add_test(NAME cli_poison_branch_ub COMMAND valuetrace solve
            "${CMAKE_CURRENT_SOURCE_DIR}/cpp/tests/fixtures/paths.ll"
            --function poison_branch_sink --param 0 --op eq --value 7 --up 1)
        set_tests_properties(cli_poison_branch_ub PROPERTIES
            PASS_REGULAR_EXPRESSION "Undefined behavior candidate: poison used as branch condition")
        add_test(NAME cli_solve_guarded COMMAND valuetrace solve "${VT_GUARDED_LL}" --function C --param-name length --op ugt --value 4096 --up 2)
        set_tests_properties(cli_solve_guarded PROPERTIES PASS_REGULAR_EXPRESSION "UNSAT")
        add_test(NAME cli_solve_unguarded COMMAND valuetrace solve "${VT_UNGUARDED_LL}" --function C --param-name length --op ugt --value 4096 --up 2)
        set_tests_properties(cli_solve_unguarded PROPERTIES PASS_REGULAR_EXPRESSION "Boundary input")
        add_test(NAME path_witness_replay COMMAND ${CMAKE_COMMAND}
            "-DPROGRAM=$<TARGET_FILE:valuetrace>" "-DINPUT=${VT_UNGUARDED_LL}"
            "-DREPLAY=$<TARGET_FILE:valuetrace_witness_replay>" -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/TestPathWitness.cmake")

        set(VT_RISK_SAFE "${CMAKE_CURRENT_BINARY_DIR}/fixtures/risk_safe.ll")
        set(VT_RISK_UNSAFE "${CMAKE_CURRENT_BINARY_DIR}/fixtures/risk_unsafe.ll")
        set(VT_RISK_BC "${CMAKE_CURRENT_BINARY_DIR}/fixtures/risk_safe.bc")
        vt_prepared_fixture("${VT_RISK_SAFE}" examples/risk_chain.cpp DEFINE GUARDED=1)
        vt_prepared_fixture("${VT_RISK_UNSAFE}" examples/risk_chain.cpp DEFINE GUARDED=0)
        vt_prepared_fixture("${VT_RISK_BC}" examples/risk_chain.cpp DEFINE GUARDED=1)
        add_custom_target(valuetrace_risk_fixtures DEPENDS "${VT_RISK_SAFE}" "${VT_RISK_UNSAFE}" "${VT_RISK_BC}")
        add_executable(valuetrace_risk_tests cpp/tests/RiskVerifierTests.cpp examples/risk_chain.cpp)
        target_compile_definitions(valuetrace_risk_tests PRIVATE GUARDED=0)
        target_link_libraries(valuetrace_risk_tests PRIVATE valuetrace_solver)
        add_dependencies(valuetrace_risk_tests valuetrace_risk_fixtures)
        valuetrace_copy_z3(valuetrace_risk_tests)
        add_test(NAME arithmetic_verifier COMMAND valuetrace_risk_tests
            "${CMAKE_CURRENT_SOURCE_DIR}/cpp/tests/fixtures/risks.ll" "${VT_RISK_SAFE}" "${VT_RISK_UNSAFE}" "${VT_RISK_BC}")
        add_test(NAME cli_verify_guarded COMMAND valuetrace verify "${VT_RISK_SAFE}"
            --function RiskB --param-name count --up 1 --down 1 --report-level 5)
        set_tests_properties(cli_verify_guarded PROPERTIES PASS_REGULAR_EXPRESSION "feasibility=scoped-unreachable")
        add_test(NAME cli_verify_unguarded COMMAND valuetrace verify "${VT_RISK_UNSAFE}"
            --function RiskB --param-name count --up 1 --down 1)
        set_tests_properties(cli_verify_unguarded PROPERTIES PASS_REGULAR_EXPRESSION "feasibility=boundary-triggerable")
        add_test(NAME cli_verify_filters COMMAND ${CMAKE_COMMAND}
            "-DPROGRAM=$<TARGET_FILE:valuetrace>"
            "-DINPUT=${CMAKE_CURRENT_SOURCE_DIR}/cpp/tests/fixtures/risks.ll"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/TestVerificationFiltering.cmake")

        set(VT_INTEGER_LL "${CMAKE_CURRENT_BINARY_DIR}/fixtures/integer_checks.ll")
        set(VT_INTEGER_BC "${CMAKE_CURRENT_BINARY_DIR}/fixtures/integer_checks.bc")
        vt_prepared_fixture("${VT_INTEGER_LL}" examples/integer_checks.cpp)
        vt_prepared_fixture("${VT_INTEGER_BC}" examples/integer_checks.cpp)
        add_custom_target(valuetrace_integer_fixtures DEPENDS "${VT_INTEGER_LL}" "${VT_INTEGER_BC}")
        add_executable(valuetrace_integer_tests cpp/tests/IntegerCheckTests.cpp examples/integer_checks.cpp)
        target_link_libraries(valuetrace_integer_tests PRIVATE valuetrace_solver)
        add_dependencies(valuetrace_integer_tests valuetrace_integer_fixtures)
        valuetrace_copy_z3(valuetrace_integer_tests)
        add_test(NAME integer_checks COMMAND valuetrace_integer_tests
            "${CMAKE_CURRENT_SOURCE_DIR}/cpp/tests/fixtures/integer_checks.ll" "${VT_INTEGER_LL}" "${VT_INTEGER_BC}")
        add_test(NAME cli_integer_checks COMMAND ${CMAKE_COMMAND}
            "-DPROGRAM=$<TARGET_FILE:valuetrace>"
            "-DINPUT=${CMAKE_CURRENT_SOURCE_DIR}/cpp/tests/fixtures/integer_checks.ll"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/TestIntegerChecks.cmake")

        set(VT_TYPES_DEBUG "${CMAKE_CURRENT_BINARY_DIR}/fixtures/type_evidence_debug.ll")
        set(VT_TYPES_NODEBUG "${CMAKE_CURRENT_BINARY_DIR}/fixtures/type_evidence_nodebug.ll")
        vt_prepared_fixture("${VT_TYPES_DEBUG}" examples/type_evidence.cpp)
        vt_prepared_fixture("${VT_TYPES_NODEBUG}" examples/type_evidence.cpp DEBUG OFF)
        add_custom_target(valuetrace_type_fixtures DEPENDS "${VT_TYPES_DEBUG}" "${VT_TYPES_NODEBUG}")
        add_executable(valuetrace_type_tests cpp/tests/TypeEvidenceTests.cpp)
        target_link_libraries(valuetrace_type_tests PRIVATE valuetrace_solver)
        add_dependencies(valuetrace_type_tests valuetrace_type_fixtures)
        valuetrace_copy_z3(valuetrace_type_tests)
        add_test(NAME type_evidence COMMAND valuetrace_type_tests "${VT_TYPES_DEBUG}" "${VT_TYPES_NODEBUG}"
            "${CMAKE_CURRENT_SOURCE_DIR}/cpp/tests/fixtures/type_conflict.ll")

        add_executable(valuetrace_poison_tests cpp/tests/PoisonBoundaryTests.cpp)
        target_include_directories(valuetrace_poison_tests PRIVATE cpp/src)
        target_include_directories(valuetrace_poison_tests SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS})
        target_link_libraries(valuetrace_poison_tests PRIVATE valuetrace_solver valuetrace_z3)
        valuetrace_copy_z3(valuetrace_poison_tests)
        add_test(NAME poison_boundaries COMMAND valuetrace_poison_tests
            "${CMAKE_CURRENT_SOURCE_DIR}/cpp/tests/fixtures/poison_boundaries.ll")

        set(VT_LOCAL_LL "${CMAKE_CURRENT_BINARY_DIR}/fixtures/mem2reg_locals.ll")
        set(VT_LOCAL_BC "${CMAKE_CURRENT_BINARY_DIR}/fixtures/mem2reg_locals.bc")
        vt_prepared_fixture("${VT_LOCAL_LL}" examples/mem2reg_locals.cpp)
        vt_prepared_fixture("${VT_LOCAL_BC}" examples/mem2reg_locals.cpp)
        add_custom_target(valuetrace_local_fixtures DEPENDS "${VT_LOCAL_LL}" "${VT_LOCAL_BC}")
        add_dependencies(valuetrace valuetrace_local_fixtures)
        foreach(case_name IN ITEMS LocalSteps LocalBranch LocalNarrow)
            add_test(NAME "mem2reg_${case_name}" COMMAND ${CMAKE_COMMAND}
                "-DPROGRAM=$<TARGET_FILE:valuetrace>" "-DINPUT=${VT_LOCAL_LL}"
                "-DFUNCTION=${case_name}" -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/TestMem2Reg.cmake")
        endforeach()
        add_test(NAME mem2reg_bitcode COMMAND ${CMAKE_COMMAND}
            "-DPROGRAM=$<TARGET_FILE:valuetrace>" "-DINPUT=${VT_LOCAL_BC}"
            "-DFUNCTION=LocalSteps" -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/TestMem2Reg.cmake")

        set(VT_MULTI_DIR "${CMAKE_CURRENT_BINARY_DIR}/fixtures/multifile")
        set(VT_MULTI_OUTPUTS)
        foreach(variant IN ITEMS safe unsafe)
            list(APPEND VT_MULTI_OUTPUTS "${VT_MULTI_DIR}/${variant}/a.ll" "${VT_MULTI_DIR}/${variant}/b.bc"
                "${VT_MULTI_DIR}/${variant}/c.ll" "${VT_MULTI_DIR}/reference_${variant}.ll")
        endforeach()
        add_custom_command(OUTPUT ${VT_MULTI_OUTPUTS}
            COMMAND ${CMAKE_COMMAND} "-DCLANG=${VT_CLANGXX}" "-DOPT=${VT_OPT}"
                "-DPREPARE_SCRIPT=${CMAKE_CURRENT_SOURCE_DIR}/scripts/prepare-ir.cmake"
                "-DSOURCE=${CMAKE_CURRENT_SOURCE_DIR}/examples/multifile"
                "-DOUTPUT=${VT_MULTI_DIR}" -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateMultifileFixtures.cmake"
            DEPENDS examples/multifile/a.cpp examples/multifile/b.cpp examples/multifile/c.cpp
                examples/multifile/reference.cpp examples/multifile/chain.h cmake/GenerateMultifileFixtures.cmake
                scripts/prepare-ir.cmake VERBATIM)
        add_custom_target(valuetrace_multi_fixtures DEPENDS ${VT_MULTI_OUTPUTS})
        file(MAKE_DIRECTORY "${VT_MULTI_DIR}/empty/nested")
        configure_file(cpp/tests/fixtures/multifile/duplicate_a.ll "${VT_MULTI_DIR}/empty/nested/hidden.ll" COPYONLY)
        add_executable(valuetrace_multifile_tests cpp/tests/MultiFileTests.cpp
            examples/multifile/a.cpp examples/multifile/b.cpp examples/multifile/c.cpp)
        target_compile_definitions(valuetrace_multifile_tests PRIVATE GUARDED=0)
        target_include_directories(valuetrace_multifile_tests PRIVATE examples/multifile)
        target_link_libraries(valuetrace_multifile_tests PRIVATE valuetrace_solver)
        add_dependencies(valuetrace_multifile_tests valuetrace_multi_fixtures)
        valuetrace_copy_z3(valuetrace_multifile_tests)
        add_test(NAME multifile_analysis COMMAND valuetrace_multifile_tests "${VT_MULTI_DIR}"
            "${CMAKE_CURRENT_SOURCE_DIR}/cpp/tests/fixtures/multifile")
        add_test(NAME cli_multifile COMMAND ${CMAKE_COMMAND}
            "-DPROGRAM=$<TARGET_FILE:valuetrace>" "-DFIXTURES=${VT_MULTI_DIR}"
            "-DCONFLICTS=${CMAKE_CURRENT_SOURCE_DIR}/cpp/tests/fixtures/multifile"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/TestMultiFileCli.cmake")

        set(VT_MEMORY_OUTPUTS)
        foreach(case_name IN ITEMS 01_pointer_output 02_distinct_fields 03_conditional_alias
                04_external_pointer 05_dynamic_index 06_memcpy_field)
            set(source "${CMAKE_CURRENT_SOURCE_DIR}/examples/memory_acceptance/${case_name}.cpp")
            set(output "${CMAKE_CURRENT_BINARY_DIR}/fixtures/memory_acceptance/${case_name}.ll")
            if(case_name STREQUAL "06_memcpy_field")
                set(extra -fno-builtin-memcpy)
            else()
                set(extra)
            endif()
            vt_prepared_fixture("${output}" "examples/memory_acceptance/${case_name}.cpp" EXTRA "${extra}")
            list(APPEND VT_MEMORY_OUTPUTS "${output}")
        endforeach()
        set(VT_ESCAPED_GLOBAL_LL "${CMAKE_CURRENT_BINARY_DIR}/fixtures/memory_acceptance/07_escaped_global.ll")
        vt_prepared_fixture("${VT_ESCAPED_GLOBAL_LL}" "examples/memory_acceptance/07_escaped_global.cpp")
        add_custom_target(valuetrace_memory_fixtures DEPENDS ${VT_MEMORY_OUTPUTS} "${VT_ESCAPED_GLOBAL_LL}")
        add_executable(valuetrace_memory_acceptance cpp/tests/MemoryAcceptanceTests.cpp)
        target_link_libraries(valuetrace_memory_acceptance PRIVATE valuetrace_solver)
        add_dependencies(valuetrace_memory_acceptance valuetrace_memory_fixtures)
        valuetrace_copy_z3(valuetrace_memory_acceptance)
        add_test(NAME memory_acceptance COMMAND valuetrace_memory_acceptance "${CMAKE_CURRENT_BINARY_DIR}/fixtures/memory_acceptance/01_pointer_output.ll" "${VT_ESCAPED_GLOBAL_LL}")
        add_test(NAME memory_acceptance_later COMMAND valuetrace_memory_acceptance ${VT_MEMORY_OUTPUTS})
        set_tests_properties(memory_acceptance_later PROPERTIES DISABLED TRUE)
    endif()
endif()
