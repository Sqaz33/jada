cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

project(jada)

find_package(PkgConfig REQUIRED)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

pkg_check_modules(GVC REQUIRED libgvc)
pkg_check_modules(LIBCGRAPH REQUIRED libcgraph)

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

file(GLOB_RECURSE SRC "src/*.cpp")

add_executable(${PROJECT_NAME}  
               ${SRC}
               ${BISON_parser_OUTPUTS}
               ${FLEX_scanner_OUTPUTS}
)
target_compile_options(${PROJECT_NAME} PRIVATE -Wno-all) # TODO:

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_include_directories(${PROJECT_NAME} PRIVATE ${GVC_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${GVC_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${LIBCGRAPH_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBCGRAPH_LIBRARIES})

set(CD_CORE "src/graphviz.cpp" 
            "src/attribute.cpp"
            "src/method.cpp"
            "src/jvm_code_attribute.cpp"
            "src/jvm_class.cpp"
            "src/instruction.cpp"
            "src/field.cpp"
            "src/descriptor.cpp"
            "src/constant.cpp"
            "src/constant_pool.cpp"
            "src/codegen.cpp"
            "src/class_member.cpp"
            "src/basic_block.cpp"
            "src/jvm_attribute.cpp")

add_executable(cd_test test/cd_test.cpp ${CD_CORE})
target_include_directories(cd_test PRIVATE ${GVC_INCLUDE_DIRS})
target_link_libraries(cd_test PRIVATE ${GVC_LIBRARIES})
target_include_directories(cd_test PRIVATE ${LIBCGRAPH_INCLUDE_DIRS})
target_link_libraries(cd_test PRIVATE ${LIBCGRAPH_LIBRARIES})
target_include_directories(cd_test PRIVATE src)