cmake_minimum_required(VERSION 3.13)
project(risc_v_isa)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_library(risc_v_isa src/instruction.cpp)
target_compile_definitions(risc_v_isa PRIVATE __RV32I__)

add_executable(rv32i_test test/rv32i.cpp)
target_compile_definitions(rv32i_test PRIVATE __RV32I__)
