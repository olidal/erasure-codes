cmake_minimum_required(VERSION 3.7)

project(erasure-codes)

enable_testing()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(NOT BOOST_ROOT AND NOT $ENV{BOOST_ROOT} STREQUAL "")
	file(TO_CMAKE_PATH $ENV{BOOST_ROOT} BOOST_ROOT)
	if(NOT EXISTS ${BOOST_ROOT})
		string(REPLACE "\"" "" BOOST_ROOT ${BOOST_ROOT})
	endif()

	set(BOOST_ROOT ${BOOST_ROOT} CACHE STRING "" FORCE)
endif()

find_package(boost REQUIRED COMPONENTS numeric)

add_subdirectory(gf-arithmetic)
add_subdirectory(erasure-codes)
add_subdirectory(erasure-test)
add_subdirectory(lohi-generator)
