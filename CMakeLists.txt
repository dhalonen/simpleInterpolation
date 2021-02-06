cmake_minimum_required(VERSION 3.7)
project(simpleInterpolationTest)
enable_testing()
set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_FLAGS "-Wall -mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -fPIE -g")
set(CMAKE_CXX_FLAGS "-W -Wall  -fPIE -Wpedantic -Wdeprecated -Wconversion -Wextra")

#add_library(simpleTest_lib STATIC
  #  simpleInterpolationTest.cpp)

add_executable(simpleTest
  simpleInterpolation.h
  simpleInterpolationTest.cpp)

add_test(TEST_1 simpleTest)

