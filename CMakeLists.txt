cmake_minimum_required(VERSION 3.5)

project(FileSystemEmulator)

set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME} STATIC FileSystemEmulator.cpp FileSystemEmulator.hpp)
target_include_directories(${PROJECT_NAME} PUBLIC .)

add_executable(Test${PROJECT_NAME} Tests.cpp)
target_link_libraries(Test${PROJECT_NAME} ${PROJECT_NAME})
add_test(NAME Test${PROJECT_NAME} COMMAND Test${PROJECT_NAME})