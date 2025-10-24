add_executable(parser_test parser_test.cpp)
target_compile_definitions(parser_test PRIVATE 
                            TSTDATADIR="${CMAKE_SOURCE_DIR}/test_data/"
                            TSTOUTPUTDIR="${CMAKE_BINARY_DIR}/tstout/")

add_custom_command(TARGET parser_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/tstout
    COMMENT "Creating tstout directory for test outputs"
    VERBATIM
)

target_include_directories(parser_test PRIVATE ${CMAKE_BINARY_DIR})
target_include_directories(parser_test PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(parser_test PRIVATE Lexer)
target_link_libraries(parser_test PRIVATE Parser)
target_link_libraries(parser_test PRIVATE Core)

