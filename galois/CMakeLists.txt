# galois

file(GLOB sources
	*.cpp
	*.h
)

if(NOT BOOST_ROOT AND NOT $ENV{BOOST_ROOT} STREQUAL "")
	file(TO_CMAKE_PATH $ENV{BOOST_ROOT} BOOST_ROOT)
	if(NOT EXISTS ${BOOST_ROOT})
		string(REPLACE "\"" "" BOOST_ROOT ${BOOST_ROOT})
	endif()

	set(BOOST_ROOT ${BOOST_ROOT} CACHE STRING "" FORCE)
endif()

find_package(boost REQUIRED COMPONENTS numeric)

add_definitions(-DGALOIS_BUILD)

add_library(galois ${sources})

target_include_directories(galois PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(galois PUBLIC ${Boost_LIBRARIES})
