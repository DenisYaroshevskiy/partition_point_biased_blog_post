cmake_minimum_required(VERSION 3.7)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS_DEBUG "-O1 -fno-omit-frame-pointer -g -fsanitize=address -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -Wextra -Werror -g")

project(partition_point_biased_blog_post)

include_directories(./)

set(HEADER_FILE
    catch.h
    other_algorithms.h
    result.h
   )
set(TEST_SOURCE_FILES
    other_algorithms_test.cc
    third_party/main_catch.cc)
set(BENCHMARK_SOURCE_FILES
    binary_search_benchmark.cc
    third_party/google_benchmark_main.cc)

add_executable(test ${TEST_SOURCE_FILES})
add_executable(benchmarks ${BENCHMARK_SOURCE_FILES})
target_link_libraries(benchmarks benchmark)
