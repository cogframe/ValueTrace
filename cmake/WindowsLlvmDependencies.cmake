# The official LLVM Windows archive references these static targets but does
# not ship their libraries. Build matching MSVC dependencies instead of finding
# unrelated MinGW libraries on PATH. Downloads stay inside the build directory.
include(FetchContent)
set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SKIP_INSTALL_ALL ON CACHE BOOL "" FORCE)
FetchContent_Declare(vt_zlib
    URL https://github.com/madler/zlib/archive/refs/tags/v1.3.1.tar.gz
    URL_HASH SHA256=17e88863f3600672ab49182f217281b6fc4d3c762bde361935e436a95214d05c
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
FetchContent_MakeAvailable(vt_zlib)
if(NOT TARGET ZLIB::ZLIB)
    add_library(ZLIB::ZLIB ALIAS zlibstatic)
endif()

set(ZSTD_BUILD_PROGRAMS OFF CACHE BOOL "" FORCE)
set(ZSTD_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ZSTD_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(ZSTD_BUILD_STATIC ON CACHE BOOL "" FORCE)
FetchContent_Declare(vt_zstd
    URL https://github.com/facebook/zstd/archive/refs/tags/v1.5.7.tar.gz
    URL_HASH SHA256=37d7284556b20954e56e1ca85b80226768902e2edabd3b649e9e72c0c9012ee3
    SOURCE_SUBDIR build/cmake
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
FetchContent_MakeAvailable(vt_zstd)
if(NOT TARGET zstd::libzstd_static)
    add_library(zstd::libzstd_static ALIAS libzstd_static)
endif()

# Targets already exist. Prevent LLVMConfig from finding MinGW copies.
set(CMAKE_DISABLE_FIND_PACKAGE_ZLIB TRUE)
set(CMAKE_DISABLE_FIND_PACKAGE_zstd TRUE)
set(CMAKE_DISABLE_FIND_PACKAGE_LibXml2 TRUE)
