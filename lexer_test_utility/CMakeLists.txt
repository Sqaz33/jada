add_executable(tst test.cpp)
target_link_libraries(tst PRIVATE Scanner)
target_link_libraries(tst PRIVATE Utility)
target_compile_definitions(tst PRIVATE 
                            TSTDATADIR="${CMAKE_SOURCE_DIR}/tests/lexer_tests/"
                            TSTOUTPUTDIR="${CMAKE_BINARY_DIR}/tstout/lexer_tests/")

add_custom_command(TARGET tst POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/tstout/lexer_tests
    COMMENT "Creating tstout directory for test outputs"
    VERBATIM
)

