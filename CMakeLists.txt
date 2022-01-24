cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(EXECUTABLE_OUTPUT_PATH bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY bin)

project(libCSD64)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -O0 -g3 -fsanitize=undefined -fsanitize=address -fsanitize=integer")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -O3 -g0")

include_directories(${CMAKE_SOURCE_DIR}/src)

file(GLOB_RECURSE SRCS src/*.cpp)

set(sources ${SRCS})
list(REMOVE_ITEM sources ${CMAKE_CURRENT_SOURCE_DIR}/src/main/*)
add_library(libCSD64 STATIC ${sources})

set(BUILD_TARGETS
    build
    test-single
    test-substr
)

foreach(target ${BUILD_TARGETS})
    add_executable(${target} ${CMAKE_SOURCE_DIR}/src/main/${target}.cpp)
    target_link_libraries(${target} libCSD64)
endforeach(target)
