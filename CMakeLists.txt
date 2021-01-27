cmake_minimum_required(VERSION 3.7)
project(simpleInterpolationTest)
set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "-Wall -mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -fPIE -g")
set(CMAKE_CXX_FLAGS "-Wall  -fPIE -g")

#add_library(simpleTest_lib STATIC
  #  simpleInterpolationTest.cpp)

add_executable(simpleTest
  simpleInterpolation.h
  simpleInterpolationTest.cpp)

#target_link_libraries(simpleTest PRIVATE
#  simpleTest_lib)

