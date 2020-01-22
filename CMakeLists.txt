cmake_minimum_required(VERSION 3.10)
project(risc_v_isa)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

add_library(risc_v_isa src/instruction.cpp)
target_compile_definitions(risc_v_isa PRIVATE __RV_EXTENSION_M__)

add_executable(rv32i_test test/rv32i.cpp)
target_compile_definitions(rv32i_test PRIVATE __RV_64_BIT__ __RV_EXTENSION_M__)
