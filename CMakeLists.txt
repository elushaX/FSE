cmake_minimum_required(VERSION 3.5)

project(FileSystemEmulator)
set(CMAKE_CXX_STANDARD 20)
enable_testing()

add_subdirectory(ext/testing)
target_compile_options(UnitTest++ PUBLIC -Wno-error)

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "inc/*.hpp")

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC inc/)

add_executable(interactive "app/Interactive.cpp")
target_link_libraries(interactive ${PROJECT_NAME})

add_executable(streamExec "app/StreamReader.cpp")
target_link_libraries(streamExec ${PROJECT_NAME})
file(COPY "test/commands" DESTINATION "${CMAKE_BINARY_DIR}/")

file(GLOB TEST_SOURCES "./test/*.cpp")
add_executable(Test${PROJECT_NAME} ${TEST_SOURCES})
target_link_libraries(Test${PROJECT_NAME} ${PROJECT_NAME} UnitTest++)


add_test(NAME UnitTests COMMAND Test${PROJECT_NAME})
add_test(NAME StreamTest COMMAND valgrind --leak-check=full ./streamExec)