cmake_minimum_required(VERSION 3.14)
project(funnels_cpp)

set(CMAKE_CXX_STANDARD 17)

#bfind_package(Boost REQUIRED)

set(FUNNEL_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/eigen")

include_directories(${FUNNEL_INC_DIR})

#add_subdirectory(json)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src")


add_library(libutil STATIC ${UTIL_FILES})
add_library(libtransition STATIC ${TRANSITION_FILES})
target_link_libraries(libtransition PRIVATE libutil)
add_library(libfunnels STATIC ${FUNNEL_FILES})


add_executable(funnels_cpp main.cpp)
#target_link_libraries(funnels_cpp PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(funnels_cpp PUBLIC libutil)
target_link_libraries(funnels_cpp PUBLIC libtransition)
target_link_libraries(funnels_cpp PUBLIC libfunnels)

add_executable(main2 main2.cc)