cmake_minimum_required(VERSION 3.10)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_SOURCE_DIR}/cmake/Modules")

if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()

project(dextool)

if(WIN32)
    # ldc links druntime/phobos against the static CRT (libcmt), as do the
    # official LLVM binary releases. Match that in all C/C++ code to avoid
    # mixing CRTs in the final link.
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

enable_language(D)

include(common)
if(NOT LIBCLANG_LIB_PATH OR NOT LIBCLANG_LDFLAGS)
    include(FindLibClang)
endif()
include(BuildD)

if(WIN32 AND LIBCLANG_RUNTIME_DLL)
    # Ship libclang.dll next to the executables (which live in bin/) so the
    # installed tree can be extracted and run anywhere without a separate LLVM
    # installation on PATH.
    install(FILES "${LIBCLANG_RUNTIME_DLL}" DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif()

if(WIN32)
    # all executables are linked by the D compiler which does not understand
    # the bare MSVC linker flags cmake defaults to (/machine:x64 etc) and
    # would interpret them as input files. the D compiler passes the correct
    # flags to the MSVC linker by itself. must be set after all
    # enable_language calls because those re-initialize the cache values.
    foreach(cfg "" "_DEBUG" "_RELEASE" "_RELWITHDEBINFO" "_MINSIZEREL")
        set(CMAKE_EXE_LINKER_FLAGS${cfg} "")
    endforeach()
endif()

set(DEXTOOL_USE_MOLD "AUTO" CACHE STRING "Use mold linker: AUTO, ON, or OFF")
set_property(CACHE DEXTOOL_USE_MOLD PROPERTY STRINGS AUTO ON OFF)

if(NOT DEXTOOL_USE_MOLD MATCHES "^(AUTO|ON|OFF)$")
    message(FATAL_ERROR
        "DEXTOOL_USE_MOLD must be one of AUTO, ON, or OFF (got '${DEXTOOL_USE_MOLD}')")
endif()

if(UNIX AND NOT DEXTOOL_USE_MOLD STREQUAL "OFF")
    find_program(DEXTOOL_MOLD_PROGRAM mold)
    if(DEXTOOL_MOLD_PROGRAM)
        foreach(linker_flag_var
                CMAKE_EXE_LINKER_FLAGS
                CMAKE_SHARED_LINKER_FLAGS
                CMAKE_MODULE_LINKER_FLAGS)
            append("-fuse-ld=mold" ${linker_flag_var})
        endforeach()
        message(STATUS "Using mold linker via -fuse-ld=mold (${DEXTOOL_MOLD_PROGRAM})")
    elseif(DEXTOOL_USE_MOLD STREQUAL "ON")
        message(FATAL_ERROR "DEXTOOL_USE_MOLD=ON, but no 'mold' executable was found in PATH")
    endif()
endif()

#
# Tool preparation
#
execute_process(
    COMMAND ${D_COMPILER} ${CMAKE_SOURCE_DIR}/tools/symlink.d -of${CMAKE_BINARY_DIR}/symlink${CMAKE_EXECUTABLE_SUFFIX}
    RESULT_VARIABLE symlink_tool_status
)
if(symlink_tool_status)
    message(FATAL_ERROR "Failed to build the symlink helper tool from tools/symlink.d")
endif()

#
# Main configuration.
#

set(TEST_WITH_COV false CACHE BOOL "true to run unittest with coverage")
set(BUILD_TEST false CACHE BOOL "true to build and run unittests")
set(DEXTOOL_D_COVERAGE_DIR "${CMAKE_BINARY_DIR}/coverage")
if(BUILD_TEST)
    enable_testing()
    add_custom_target(check)
    add_custom_target(check_integration)
    # integration tests do NOT work if not all binaries are build
    # this rule ensure that is suck
    add_custom_target(check_integration_make_all
        COMMAND cmake --build ${CMAKE_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    add_dependencies(check_integration check_integration_make_all)
endif()

# Generally, we want to install everything into CMAKE_INSTALL_PREFIX, but when
# it is /usr, put the config files into /etc to meet common practice.
if(NOT DEFINED SYSCONF_INSTALL_DIR)
    if(CMAKE_INSTALL_PREFIX STREQUAL "/usr")
        set(SYSCONF_INSTALL_DIR "/etc")
    else()
        set(SYSCONF_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/etc")
    endif()
endif()

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR})

# Setup D compiler flags (DMD syntax, which also works with LDMD).
set(DDMD_DFLAGS "-w -wi -vcolumns -preview=dip1000 -verrors=0")
set(DDMD_LFLAGS "")
set(DDMD_COV_FLAG "-cov")
set(ACTOR_EXTRA_TRACE false CACHE BOOL "true to activate detailed logging of all actor interaction, message etc")

if(TEST_WITH_COV)
    if(NOT "${D_COMPILER_ID}" STREQUAL "DigitalMars" AND
       NOT "${D_COMPILER_ID}" STREQUAL "LDMD")
        message(FATAL_ERROR
            "TEST_WITH_COV requires a D compiler with DMD-compatible coverage support. "
            "Use dmd or ldmd2; current D_COMPILER_ID is '${D_COMPILER_ID}'.")
    endif()

    append("${DDMD_COV_FLAG}" DDMD_DFLAGS)
endif()

# for multi-config builds, these options have to be added later to the custom command
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    append("-g -debug" DDMD_DFLAGS)
    if (ACTOR_EXTRA_TRACE)
        append("-version=mylib_actor_trace" DDMD_DFLAGS)
    endif()
    if("${D_COMPILER_ID}" STREQUAL "LDMD")
        append("-O0 -link-debuglib" DDMD_DFLAGS)
    elseif("${D_COMPILER_ID}" STREQUAL "DigitalMars")
        append("-verrors=context" DDMD_DFLAGS)
    endif()
elseif(CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
    if("${D_COMPILER_ID}" STREQUAL "LDMD")
        append("-g -enable-inlining -O5 -release" DDMD_DFLAGS)
    elseif("${D_COMPILER_ID}" STREQUAL "DigitalMars")
        append("-g -release" DDMD_DFLAGS)
    endif()
else()
    # Default to a Release build type
    append("-release" DDMD_DFLAGS)
    if("${D_COMPILER_ID}" STREQUAL "LDMD")
        append("-enable-inlining -O5" DDMD_DFLAGS)
    else()
        # -release do not work with dmd
        #append("-release" DDMD_DFLAGS)
    endif()
endif()

cmake_host_system_information(RESULT TOTAL_RAM_MIB QUERY TOTAL_PHYSICAL_MEMORY)

if(NOT DEFINED LOW_MEM AND TOTAL_RAM_MIB AND TOTAL_RAM_MIB LESS 16384)
    if(TOTAL_RAM_MIB LESS 12288)
        message(WARNING
            "Detected ${TOTAL_RAM_MIB} MiB RAM (<12 GiB). "
            "Building dextool may run out of memory when compiling dextool-mutate.o. "
            "If that happens, rerun CMake with -DLOW_MEM=ON and build single-threaded (-j1).")
    else()
        message(STATUS
            "Detected ${TOTAL_RAM_MIB} MiB RAM (<16 GiB). "
            "dextool builds most comfortably with ~16 GiB RAM. "
            "If ldc is killed while compiling dextool-mutate.o, rerun CMake with "
            "-DLOW_MEM=ON and build single-threaded (-j1).")
    endif()
endif()

if(LOW_MEM)
    append("-lowmem" DDMD_DFLAGS)
endif()

#
# Resources
#
append("-J${PROJECT_BINARY_DIR}/resources" DDMD_DFLAGS) # Needed for importing text files
# strip leading/trailing whitespace
string(STRIP "${DDMD_DFLAGS}" DDMD_DFLAGS)

#
# Emedded version
#
set(DEXTOOL_EMBEDDED_VERSION_PATH ${PROJECT_BINARY_DIR}/resources/version.txt)

# Generate the version description
if(UNIX)
    execute_process(
        COMMAND ${PROJECT_SOURCE_DIR}/tools/gen_version_from_git.sh
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    )
else()
    file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/resources)
    execute_process(
        COMMAND git describe --tags
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE DEXTOOL_GIT_VERSION
        RESULT_VARIABLE DEXTOOL_GIT_VERSION_STATUS
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(DEXTOOL_GIT_VERSION_STATUS)
        set(DEXTOOL_GIT_VERSION "unknown build")
    endif()
    file(WRITE ${DEXTOOL_EMBEDDED_VERSION_PATH} "${DEXTOOL_GIT_VERSION}\n")
endif()

add_custom_target(dextool_embedded_version
    DEPENDS
    ${DEXTOOL_EMBEDDED_VERSION_PATH}
)

#
# Configure the main executable that all underlying plugins adher to
#
set(DEXTOOL_MAIN_EXE "dextool")
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(DEXTOOL_MAIN_EXE "${DEXTOOL_MAIN_EXE}_debug")
endif()
set(DEXTOOL_MAIN_EXE_FULL ${PROJECT_BINARY_DIR}/${DEXTOOL_MAIN_EXE})

#
# Distribute building
#

# building vendor libs because internal dextool libs may have dependencies on
# these.
add_subdirectory(vendor)

# base libraries used by binaries
add_subdirectory(libs)

if(BUILD_TEST)
    add_subdirectory(test)
endif()

# binary
add_subdirectory(source)
add_subdirectory(plugin)

#
# Install target.
#

#
# Packaging
#

include(cmake/CMakeCPack.cmake)
include(CPack)
