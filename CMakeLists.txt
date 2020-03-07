cmake_minimum_required(VERSION 3.10)
project(riscv_isa)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fno-exceptions -fno-rtti")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -D DEBUG")

add_executable(test_unit_rv32i test/unit/rv32i_test.cpp)
target_compile_definitions(test_unit_rv32i PRIVATE __RV_BASE_I__ __RV_BIT_WIDTH__=32)
target_include_directories(test_unit_rv32i PRIVATE test/include)

add_executable(test_unit_rv32ic test/unit/rv32ic_test.cpp)
target_compile_definitions(test_unit_rv32ic PRIVATE __RV_BASE_I__ __RV_BIT_WIDTH__=32 __RV_EXTENSION_C__)
target_include_directories(test_unit_rv32ic PRIVATE test/include)

add_executable(test_inter_factorial test/integration/factorial_test.cpp)
target_compile_definitions(test_inter_factorial PRIVATE
        __RV_BASE_I__ __RV_BIT_WIDTH__=32
        __RV_USER_MODE__ __RV_SUPERVISOR_MODE__
        __RV_EXTENSION_M__ __RV_EXTENSION_ZICSR__)
target_include_directories(test_inter_factorial PRIVATE test/include)
