# vim: filetype=cmake

set(LIBCLANG_INC "-I${CMAKE_CURRENT_LIST_DIR}/source -I${LIBLLVM_MAJOR_VERSION_INC} -P-I${LIBLLVM_LIBCLANG_INC}" CACHE STRING "path to preprocessed libclang headers")
list(APPEND SRC_FILES
    ${CMAKE_CURRENT_LIST_DIR}/source/clang/c/BuildSystem.c
    ${CMAKE_CURRENT_LIST_DIR}/source/clang/c/CXCompilationDatabase.c
    ${CMAKE_CURRENT_LIST_DIR}/source/clang/c/CXDiagnostic.c
    ${CMAKE_CURRENT_LIST_DIR}/source/clang/c/CXErrorCode.c
    ${CMAKE_CURRENT_LIST_DIR}/source/clang/c/CXFile.c
    ${CMAKE_CURRENT_LIST_DIR}/source/clang/c/CXSourceLocation.c
    ${CMAKE_CURRENT_LIST_DIR}/source/clang/c/CXString.c
    ${CMAKE_CURRENT_LIST_DIR}/source/clang/c/Index.c
    )

compile_d_static_lib(dextool_libclang "${SRC_FILES}" "${LIBCLANG_INC}" "" "")
target_link_libraries(dextool_libclang ${LIBCLANG_LDFLAGS})
