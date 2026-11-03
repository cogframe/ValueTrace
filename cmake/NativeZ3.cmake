if(WIN32)
    include(FetchContent)
    FetchContent_Declare(vt_z3
        URL https://github.com/Z3Prover/z3/releases/download/z3-5.3.3/z3-5.3.3-x64-win.zip
        URL_HASH SHA256=4e48624a6895b49606b3cf91ca4fbcc069fb2973b9706532170a144baa3fd798
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
    FetchContent_MakeAvailable(vt_z3)
    find_path(VT_Z3_INCLUDE z3++.h PATHS "${vt_z3_SOURCE_DIR}/include" NO_DEFAULT_PATH REQUIRED)
    find_file(VT_Z3_IMPLIB libz3.lib PATHS "${vt_z3_SOURCE_DIR}/bin" "${vt_z3_SOURCE_DIR}/lib" NO_DEFAULT_PATH REQUIRED)
    find_file(VT_Z3_DLL libz3.dll PATHS "${vt_z3_SOURCE_DIR}/bin" NO_DEFAULT_PATH REQUIRED)
    add_library(valuetrace_z3 SHARED IMPORTED)
    set_target_properties(valuetrace_z3 PROPERTIES IMPORTED_IMPLIB "${VT_Z3_IMPLIB}"
        IMPORTED_LOCATION "${VT_Z3_DLL}" INTERFACE_INCLUDE_DIRECTORIES "${VT_Z3_INCLUDE}")
else()
    find_path(VT_Z3_INCLUDE z3++.h REQUIRED)
    find_library(VT_Z3_LIBRARY z3 REQUIRED)
    add_library(valuetrace_z3 UNKNOWN IMPORTED)
    set_target_properties(valuetrace_z3 PROPERTIES IMPORTED_LOCATION "${VT_Z3_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${VT_Z3_INCLUDE}")
endif()

function(valuetrace_copy_z3 target)
    if(WIN32)
        add_custom_command(TARGET ${target} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${VT_Z3_DLL}" "$<TARGET_FILE_DIR:${target}>" VERBATIM)
    endif()
endfunction()
