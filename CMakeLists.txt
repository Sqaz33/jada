cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

project(jada)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

flex_target(
    scanner 
    src/lexer.l 
    ${CMAKE_BINARY_DIR}/lexer.cpp
)
bison_target(parser
    src/grammar.y
    ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
    COMPILE_FLAGS "--defines=${CMAKE_CURRENT_BINARY_DIR}/parser.hpp"
)
add_flex_bison_dependency(scanner parser)

file(GLOB SRC "src/*.cpp")

add_executable(${PROJECT_NAME}  
               ${SRC}
               ${BISON_parser_OUTPUTS}
               ${FLEX_scanner_OUTPUTS}
)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
