cmake_minimum_required(VERSION 3.5)

project(FSE)
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

add_executable(TestFSE "test/Tests.cpp")
target_link_libraries(TestFSE ${PROJECT_NAME} UnitTest++)

add_executable(TestConsistency "test/TestConsistency.cpp")
target_link_libraries(TestConsistency ${PROJECT_NAME} UnitTest++)

file(COPY "test/consistency" DESTINATION "${CMAKE_BINARY_DIR}/")

add_test(NAME UnitTests COMMAND valgrind ./TestFSE)
add_test(NAME TestConsistency COMMAND ./consistency/check)
