cmake_minimum_required(VERSION 3.23)
project(DataStructuresProject VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 

add_library(ds_lib INTERFACE)

target_sources(ds_lib INTERFACE
    FILE_SET HEADERS
    BASE_DIRS include
    FILES 
        include/rbt/rbt.hpp  
)

if (MSVC)
    target_compile_options(ds_lib INTERFACE /W4)
else()
    target_compile_options(ds_lib INTERFACE -Wall -Wextra -Wpedantic)
endif()

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
    add_executable(ds_playground src/main.cpp)
    target_link_libraries(ds_playground PRIVATE ds_lib)
endif()

enable_testing()

function(add_ds_test TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} PRIVATE ds_lib)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endfunction()

add_ds_test(test_rbt tests/test_rbt.cpp)