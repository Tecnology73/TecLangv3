cmake_minimum_required(VERSION 3.25)
project(teclang)

set(BINARY ${CMAKE_PROJECT_NAME})
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM REQUIRED CONFIG)
find_package(LLD REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${LLD_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# Now build our tools
file(GLOB_RECURSE AST_SOURCES LIST_DIRECTORIES true ast/*.h ast/*.cpp)
file(GLOB_RECURSE COMPILER_SOURCES LIST_DIRECTORIES true compiler/*.h compiler/*.cpp)
file(GLOB_RECURSE LEXER_SOURCES LIST_DIRECTORIES true lexer/*.h lexer/*.cpp)
file(GLOB_RECURSE PARSER_SOURCES LIST_DIRECTORIES true parser/*.h parser/*.cpp)
file(GLOB_RECURSE ANALYSIS_SOURCES LIST_DIRECTORIES true analysis/*.h analysis/*.cpp)

add_executable(${BINARY}_run main.cpp Console.cpp ${AST_SOURCES} ${COMPILER_SOURCES} ${LEXER_SOURCES} ${PARSER_SOURCES} ${ANALYSIS_SOURCES})
#add_executable(${BINARY}_run main_network.cpp)

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs ${LLVM_TARGETS_TO_BUILD} core native)

# Link against LLVM libraries
target_link_libraries(${BINARY}_run ${llvm_libs} lldCOFF lldCommon lldELF)
