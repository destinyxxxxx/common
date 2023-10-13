cmake_minimum_required(VERSION 3.13)
project(common)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_OBJCXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(common_utils_dir ${CMAKE_CURRENT_SOURCE_DIR}/lib)

include_directories(${common_utils_dir})
include_directories(${common_utils_dir}/uuid)
include_directories(${common_utils_dir}/singleton)
include_directories(${common_utils_dir}/threadpool)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output/bin)