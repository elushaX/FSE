cmake_minimum_required(VERSION 3.5)

project(FileSystemEmulator)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SOURCES "./src/*.cpp")
file(GLOB HEADERS "./inc/*.hpp")

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC ./inc/)

file(GLOB TEST_SOURCES "./test/*.cpp")
add_executable(Test${PROJECT_NAME} ${TEST_SOURCES})
target_link_libraries(Test${PROJECT_NAME} ${PROJECT_NAME})
add_test(NAME Test${PROJECT_NAME} COMMAND Test${PROJECT_NAME})