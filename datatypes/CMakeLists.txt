cmake_minimum_required(VERSION 2.8.12)
project(datatypes)

set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -Werror")

include_directories(include)

file(GLOB_RECURSE SOURCES src/lib/*.c)
file(GLOB TARGETS src/bin/*.c)
file(GLOB TESTS src/test/*.cc)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/bin")
foreach(target ${TARGETS})
	get_filename_component(target_name "${target}" NAME_WE)
	add_executable("${target_name}" "${target}" ${SOURCES})
endforeach()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/test")
foreach(test ${TESTS})
	get_filename_component(test_name "${test}" NAME_WE)
	add_executable("${test_name}" "${test}" ${SOURCES})
	target_link_libraries("${test_name}" gtest gtest_main)
	add_test(NAME "${test_name}" COMMAND "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${test_name}")
endforeach()

