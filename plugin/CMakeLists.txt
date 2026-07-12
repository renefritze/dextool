# vim: filetype=cmake

# add_subdirectory(analyze)
if(NOT WIN32)
    # not yet ported to Windows
    add_subdirectory(ctestdouble)
    add_subdirectory(cpptestdouble)
    add_subdirectory(example)
endif()
# add_subdirectory(uml)
add_subdirectory(mutate)
