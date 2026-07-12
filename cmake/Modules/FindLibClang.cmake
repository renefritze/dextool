# - Find the dynamic lib for libclang and llvm.
#
# llvm-d version requirements:
# The identifier to set the LLVM version is defined as
# `LLVM_{MAJOR_VERSION}_{MINOR_VERSION}_{PATCH_VERSION}`, so to get LLVM
# version 3.1.0 use `LLVM_3_1_0`.
#
# The following variables are defined:
#   LIBCLANG_LDFLAGS        - flags to use when linking
#   LIBCLANG_LIBS           - clang libs to use when linking
#   LIBLLVM_LDFLAGS         - flags to use when linking
#   LIBLLVM_CXX_FLAGS       - the required flags to build C++ code using LLVM
#   LIBLLVM_CXX_EXTRA_FLAGS - the required flags to build C++ code using LLVM
#   LIBLLVM_FLAGS           - the required flags by llvm-d such as version
#   LIBLLVM_LIBS            - the required libraries for linking LLVM

if(WIN32)
    # On Windows the introspection helper is bypassed. LLVM is located via
    # llvm-config.exe (from a full LLVM distribution such as the official
    # clang+llvm-<version>-x86_64-pc-windows-msvc archive, which ships the
    # headers and static libraries needed by the clang extensions).
    #
    # All linker related flags use the dmd/ldmd2 -L passthrough syntax so
    # they survive the trip through the D compiler to the MSVC linker.
    if(DEFINED ENV{LLVM_CONFIG} AND EXISTS "$ENV{LLVM_CONFIG}")
        set(LLVM_CONFIG_BIN "$ENV{LLVM_CONFIG}")
    else()
        find_program(LLVM_CONFIG_BIN NAMES llvm-config)
    endif()
    if(NOT LLVM_CONFIG_BIN)
        message(FATAL_ERROR "llvm-config not found. Install a full LLVM distribution and add its bin directory to PATH or set the LLVM_CONFIG environment variable.")
    endif()
    message(STATUS "Using llvm-config: ${LLVM_CONFIG_BIN}")

    execute_process(COMMAND ${LLVM_CONFIG_BIN} --version
        OUTPUT_VARIABLE llvm_config_VERSION
        RESULT_VARIABLE llvm_config_VERSION_status
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${LLVM_CONFIG_BIN} --includedir
        OUTPUT_VARIABLE llvm_config_INCLUDEDIR
        RESULT_VARIABLE llvm_config_INCLUDEDIR_status
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${LLVM_CONFIG_BIN} --libdir
        OUTPUT_VARIABLE llvm_config_LIBDIR
        RESULT_VARIABLE llvm_config_LIBDIR_status
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(llvm_config_VERSION_status OR llvm_config_INCLUDEDIR_status OR llvm_config_LIBDIR_status)
        message(FATAL_ERROR "Failed to run ${LLVM_CONFIG_BIN}")
    endif()

    file(TO_CMAKE_PATH "${llvm_config_INCLUDEDIR}" llvm_config_INCLUDEDIR)
    file(TO_CMAKE_PATH "${llvm_config_LIBDIR}" llvm_config_LIBDIR)

    if(llvm_config_INCLUDEDIR MATCHES " " OR llvm_config_LIBDIR MATCHES " ")
        message(FATAL_ERROR "The LLVM installation path contains spaces (${llvm_config_LIBDIR}). Install LLVM to a path without spaces, e.g. C:/llvm.")
    endif()

    string(REPLACE "." ";" llvm_version_list "${llvm_config_VERSION}")
    list(GET llvm_version_list 0 llvm_version_major)
    list(GET llvm_version_list 1 llvm_version_minor)
    list(GET llvm_version_list 2 llvm_version_patch)
    string(REGEX REPLACE "[^0-9].*" "" llvm_version_patch "${llvm_version_patch}")

    # All clang and LLVM static libs. link.exe resolves symbols across the
    # whole set so the order does not matter. note: no [A-Z] style globs,
    # cmake matches case-insensitively on Windows by lowercasing file names
    # which makes upper case ranges never match.
    file(GLOB llvm_static_libs RELATIVE ${llvm_config_LIBDIR}
        ${llvm_config_LIBDIR}/LLVM*.lib
        ${llvm_config_LIBDIR}/clang*.lib)
    list(REMOVE_ITEM llvm_static_libs "libclang.lib" "clang.lib")
    file(GLOB clang_static_libs ${llvm_config_LIBDIR}/clangAST.lib)
    if(NOT clang_static_libs)
        message(FATAL_ERROR "No clang static libraries (clangAST.lib etc) found in ${llvm_config_LIBDIR}. The clang extensions cannot link without them. Use a full LLVM distribution such as the official clang+llvm archive.")
    endif()
    set(llvm_libs_dmd)
    foreach(l ${llvm_static_libs})
        list(APPEND llvm_libs_dmd "-L${l}")
    endforeach()
    # System libraries LLVM depends on.
    foreach(l psapi.lib shell32.lib ole32.lib uuid.lib advapi32.lib ws2_32.lib ntdll.lib version.lib)
        list(APPEND llvm_libs_dmd "-L${l}")
    endforeach()

    set(LIBCLANG_LIBS "-Llibclang.lib" CACHE STRING "Linker libraries for libclang")
    set(LIBCLANG_LDFLAGS "-L/LIBPATH:${llvm_config_LIBDIR}" CACHE STRING "Linker flags for libclang")
    set(LIBCLANG_CONFIG_DONE YES CACHE BOOL "CLANG Configuration status" FORCE)

    set(LIBLLVM_MAJOR_VERSION "${llvm_version_major}" CACHE STRING "libLLVM major version")
    set(LIBLLVM_LIBS "${llvm_libs_dmd}" CACHE STRING "Linker libraries for libLLVM")
    set(LIBLLVM_LDFLAGS "-L/LIBPATH:${llvm_config_LIBDIR}" CACHE STRING "Linker flags for libLLVM")
    set(LIBLLVM_CXX_FLAGS "-I${llvm_config_INCLUDEDIR} /std:c++17 /EHsc /DNOMINMAX /D_CRT_SECURE_NO_WARNINGS ${LIBLLVM_CXX_EXTRA_FLAGS}" CACHE STRING "Compiler flags for C++ using LLVM")
    set(LIBLLVM_FLAGS "-version=LLVM_${llvm_version_major}_${llvm_version_minor}_${llvm_version_patch}" CACHE STRING "D flags for llvm-d")
    set(LIBLLVM_LIBCLANG_INC "${llvm_config_INCLUDEDIR}" CACHE STRING "Path to where libclang-c headers such as Index.h is")
    set(LIBLLVM_CONFIG_DONE YES CACHE BOOL "LLVM Configuration status" FORCE)
else()

set(LLVM_CMD_SRC ${CMAKE_SOURCE_DIR}/cmake/introspect_llvm.d)
set(LLVM_CMD ${CMAKE_BINARY_DIR}/cmake_introspect_llvm)
set(LIBCLANG_PREPROCESS_CMD_SRC ${CMAKE_SOURCE_DIR}/cmake/preprocess_libclang.d)

if(UNIX)
    separate_arguments(cmdflags UNIX_COMMAND "${D_COMPILER_FLAGS}")
else()
    separate_arguments(cmdflags WINDOWS_COMMAND "${D_COMPILER_FLAGS}")
endif()

execute_process(COMMAND ${D_COMPILER} ${cmdflags} ${LLVM_CMD_SRC} -of${LLVM_CMD}
    OUTPUT_VARIABLE llvm_config_CMD
    RESULT_VARIABLE llvm_config_CMD_status)
if (llvm_config_CMD_status)
    message(WARNING "Compiler output: ${llvm_config_CMD}")
    message(FATAL_ERROR "Unable to compile the LLVM introspector: ${D_COMPILER} ${cmdflags} ${LLVM_CMD_SRC} -of${LLVM_CMD}")
endif()

execute_process(COMMAND ${LLVM_CMD} ${CMAKE_SOURCE_DIR} print-llvm-config-candidates
    OUTPUT_VARIABLE llvm_config_CANDIDATES
    RESULT_VARIABLE llvm_config_CANDIDATES_status
    OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "exit status:${llvm_config_CANDIDATES_status} ${llvm_config_CANDIDATES}")

execute_process(COMMAND ${LLVM_CMD} ${CMAKE_SOURCE_DIR}  ldflags
    OUTPUT_VARIABLE llvm_config_LDFLAGS
    RESULT_VARIABLE llvm_config_LDFLAGS_status
    OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND ${LLVM_CMD} ${CMAKE_SOURCE_DIR} major_version
    OUTPUT_VARIABLE llvm_config_MAJOR_VERSION
    RESULT_VARIABLE llvm_config_MAJOR_VERSION_status
    OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND ${LLVM_CMD} ${CMAKE_SOURCE_DIR} cpp-flags
    OUTPUT_VARIABLE llvm_config_CPPFLAGS
    RESULT_VARIABLE llvm_config_INCLUDE_status
    OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND ${LLVM_CMD} ${CMAKE_SOURCE_DIR} libdir
    OUTPUT_VARIABLE llvm_config_LIBDIR
    RESULT_VARIABLE llvm_config_LIBDIR_status
    OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND ${LLVM_CMD} ${CMAKE_SOURCE_DIR} libs
    OUTPUT_VARIABLE llvm_config_LIBS
    RESULT_VARIABLE llvm_config_LIBS_status
    OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND ${LLVM_CMD} ${CMAKE_SOURCE_DIR} libclang
    OUTPUT_VARIABLE clang_config_LIBS
    RESULT_VARIABLE clang_config_LIBS_status
    OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND ${LLVM_CMD} ${CMAKE_SOURCE_DIR} libclang-flags
    OUTPUT_VARIABLE clang_config_LDFLAGS
    RESULT_VARIABLE clang_config_LDFLAGS_status
    OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND ${LLVM_CMD} ${CMAKE_SOURCE_DIR} includedir
    OUTPUT_VARIABLE clang_config_INCLUDEDIR
    RESULT_VARIABLE clang_config_INCLUDEDIR_status
    OUTPUT_STRIP_TRAILING_WHITESPACE)

message(STATUS "llvm-config MAJOR_VERSION: ${llvm_config_MAJOR_VERSION}")
message(STATUS "llvm-config LIBDIR: ${llvm_config_LIBDIR}")
message(STATUS "llvm-config LDFLAGS: ${llvm_config_LDFLAGS}")
message(STATUS "llvm-config INCLUDE: ${llvm_config_CPPFLAGS}")
message(STATUS "llvm-config LIBS: ${llvm_config_LIBS}")
message(STATUS "llvm-config INCLUDEDIR: ${clang_config_INCLUDEDIR}")
message(STATUS "clang-config LDFLAGS: ${clang_config_LDFLAGS}")

# libCLANG ===================================================================

function(try_clang_from_user_config)
    if (LIBCLANG_LDFLAGS)
        set(LIBCLANG_CONFIG_DONE YES CACHE BOOL "CLANG Configuration status" FORCE)
        message("Detected user configuration of CLANG")
    endif()
endfunction()

function(try_find_libclang)
    if (clang_config_LDFLAGS_status OR clang_config_LIBS_status)
        return()
    endif()

    set(LIBCLANG_LIBS "${clang_config_LIBS}" CACHE STRING "Linker libraries for libclang")
    set(LIBCLANG_LDFLAGS "${clang_config_LDFLAGS}" CACHE STRING "Linker flags for libclang")

    set(LIBCLANG_CONFIG_DONE YES CACHE BOOL "CLANG Configuration status" FORCE)
endfunction()

# === RUNNING ===

set(LIBCLANG_CONFIG_DONE NO CACHE BOOL "CLANG Configuration status")
try_clang_from_user_config()
if (NOT LIBCLANG_CONFIG_DONE)
    try_find_libclang()
endif()

# LLVM =======================================================================

function(try_llvm_config_find)
    if (llvm_config_LDFLAGS_status OR llvm_config_LIBS_status OR llvm_config_MAJOR_VERSION_status OR llvm_config_INCLUDE_status OR llvm_config_LIBDIR_status)
        return()
    endif()

    set(LIBLLVM_MAJOR_VERSION "${llvm_config_MAJOR_VERSION}" CACHE STRING "libLLVM major version")

    set(LIBLLVM_LIBS "${llvm_config_LIBS}" CACHE STRING "Linker libraries for libLLVM")

    set(LIBLLVM_LDFLAGS "${llvm_config_LDFLAGS}" CACHE STRING "Linker flags for libLLVM")

    set(LIBLLVM_CXX_FLAGS "${llvm_config_CPPFLAGS} ${LIBLLVM_CXX_EXTRA_FLAGS}" CACHE STRING "Compiler flags for C++ using LLVM")

    set(LIBLLVM_CONFIG_DONE YES CACHE BOOL "LLVM Configuration status" FORCE)

    set(LIBLLVM_LIBCLANG_INC "${clang_config_INCLUDEDIR}" CACHE STRING "Path to where libclang-c headerss such as Index.h is")
endfunction()

function(try_llvm_from_user_config)
    if (LIBLLVM_LDFLAGS AND LIBLLVM_FLAGS AND LIBLLVM_CXX_FLAGS)
        set(LIBLLVM_CONFIG_DONE YES CACHE BOOL "LLVM Configuration status" FORCE)
        message("Detected user configuration of LLVM")
    endif()
endfunction()

# === RUNNING ===

set(LIBLLVM_CONFIG_DONE NO CACHE BOOL "LLVM Configuration status")
try_llvm_from_user_config()
if (NOT LIBLLVM_CONFIG_DONE)
    try_llvm_config_find()
endif()

endif() # WIN32

# Create the file used in D code
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/llvm)
file(WRITE ${CMAKE_BINARY_DIR}/llvm/llvm_version.d "module llvm_version; immutable int llvmVersion = ${LIBLLVM_MAJOR_VERSION};")
file(WRITE ${CMAKE_BINARY_DIR}/llvm/llvm_version.h "#pragma once\n#define LLVM_MAJOR_VERSION ${LIBLLVM_MAJOR_VERSION}")

set(LIBLLVM_MAJOR_VERSION_INC ${CMAKE_BINARY_DIR}/llvm)

# Fixup
# Simplify to only support x86
set(LIBLLVM_TARGET "LLVM_Target_X86")

message(STATUS "libclang config status : ${LIBCLANG_CONFIG_DONE}")
message(STATUS "libclang libs: ${LIBCLANG_LIBS}")
message(STATUS "libclang linker flags: ${LIBCLANG_LDFLAGS}")

message(STATUS "libLLVM config status: ${LIBLLVM_CONFIG_DONE}")
message(STATUS "libLLVM D flags: ${LIBLLVM_FLAGS}")
message(STATUS "libLLVM CXX flags: ${LIBLLVM_CXX_FLAGS}")
message(STATUS "libLLVM linker flags: ${LIBLLVM_LDFLAGS}")
message(STATUS "libLLVM libs: ${LIBLLVM_LIBS}")
