cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

project(jada)

find_package(FLEX REQUIRED)

flex_target(
    scanner src/lexer.l 
    ${CMAKE_BINARY_DIR}/lexer.cc)


add_library(Scanner STATIC ${FLEX_scanner_OUTPUTS})
target_include_directories(Scanner INTERFACE 
                            ${CMAKE_BINARY_DIR})

file(GLOB SRC "src/*.cpp")

add_library(Utility STATIC "src/string_utility.cpp")
target_include_directories(Utility INTERFACE src)

target_include_directories(Scanner PRIVATE src)

add_executable(${PROJECT_NAME}  ${SRC})
target_link_libraries(${PROJECT_NAME} PRIVATE Scanner)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall)


add_subdirectory(test_utility)