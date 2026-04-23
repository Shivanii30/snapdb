cmake_minimum_required(VERSION 3.10)
project(SnapDB)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(snapdb 
    main.cpp
    src/WAL.cpp
    src/Storage.cpp
)

add_executable(snapdb_bench
    tests/benchmark.cpp
    src/WAL.cpp
    src/Storage.cpp
)

# Create data directory if it doesn't exist
add_custom_command(TARGET snapdb POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/data
)
add_custom_command(TARGET snapdb_bench POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/bench_data
)
