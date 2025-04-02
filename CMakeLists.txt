cmake_minimum_required(VERSION 3.29)
project(moe_assets)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/include/*.cpp")
file(GLOB_RECURSE HEADERS "${CMAKE_SOURCE_DIR}/include/*.h")

include_directories(include)

add_executable(moe_assets main.cpp ${SOURCES} ${HEADERS})

target_include_directories(moe_assets PUBLIC include)
