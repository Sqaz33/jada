cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

project(jada)

find_package(FLEX REQUIRED)

flex_target(
    scanner src/lexer.l 
    ${CMAKE_BINARY_DIR}/lexer.cc)

file(GLOB SRC "src/*.cpp")

add_executable(${PROJECT_NAME} 
               ${SRC} 
               ${FLEX_scanner_OUTPUTS})

target_include_directories(${PROJECT_NAME} 
                           PRIVATE ${CMAKE_BINARY_DIR})

target_compile_options(${PROJECT_NAME} PRIVATE -Wall)