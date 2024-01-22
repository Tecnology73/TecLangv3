cmake_minimum_required(VERSION 3.27)
project(teclang_tests)

#set(BINARY ${CMAKE_PROJECT_NAME})
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(lib)

add_executable(teclang_tests_run main.cpp)

target_link_libraries(teclang_tests_run teclang_lib)
target_link_libraries(teclang_tests_run ${llvm_libs} lldCOFF lldCommon lldELF)
target_link_libraries(teclang_tests_run gtest gtest_main)

