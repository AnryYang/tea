project(hkcd)
# Specify the minimum version for CMake
cmake_minimum_required(VERSION 2.8)

#SET(CMAKE_CXX_FLAGS "-std=c++11 -O3 -w -Wl,-rpath -Wl,--enable-new-dtags -pg -g")
SET(CMAKE_CXX_FLAGS "-std=c++11 -O3 -w -Wl,-rpath -Wl,--enable-new-dtags")

find_package(Boost REQUIRED filesystem system date_time program_options)

# set(HEADER graph.h)

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# The following folder will be included
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/include/)

link_directories(${PROJECT_SOURCE_DIR}/lib/)

# add_library(mtwist ${PROJECT_SOURCE_DIR}/src/mtwist.c ${PROJECT_SOURCE_DIR}/include/mtwist.h)
add_library(graph ${PROJECT_SOURCE_DIR}/src/graph.cc ${PROJECT_SOURCE_DIR}/include/graph.h)
add_library(utils ${PROJECT_SOURCE_DIR}/src/utils.cc ${PROJECT_SOURCE_DIR}/include/utils.h)
add_library(cluster ${PROJECT_SOURCE_DIR}/src/algo.cc ${PROJECT_SOURCE_DIR}/include/algo.h 
                ${PROJECT_SOURCE_DIR}/src/sampler.cc ${PROJECT_SOURCE_DIR}/include/sampler.h
                ${PROJECT_SOURCE_DIR}/src/cluster.cc ${PROJECT_SOURCE_DIR}/include/cluster.h
                ${PROJECT_SOURCE_DIR}/src/mtwist.c ${PROJECT_SOURCE_DIR}/include/mtwist.h)

add_executable(hk ${PROJECT_SOURCE_DIR}/src/hk.cc)
target_link_libraries(hk graph utils cluster pthread dl rt boost_program_options)

# move runnable file from build dir to parent dir
add_custom_command(TARGET hk POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:hk> ${PROJECT_SOURCE_DIR}/
)
