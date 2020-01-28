cmake_minimum_required(VERSION 3.10)
project(risc_v_isa)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -D DEBUG")

add_executable(test_unit_rv32i test/unit/rv32i_test.cpp)
target_compile_definitions(test_unit_rv32i PRIVATE __RV_BIT_WIDTH__=32)
target_include_directories(test_unit_rv32i PRIVATE test/include)

add_executable(test_inter_factorial test/intergration/factorial_test.cpp)
target_compile_definitions(test_inter_factorial PRIVATE __RV_BIT_WIDTH__=64 __RV_EXTENSION_M__)
target_include_directories(test_inter_factorial PRIVATE test/include)
