# galois

file(GLOB sources
	*.cpp
	*.h
)

add_definitions(-DGALOIS_BUILD)

add_library(galois ${sources})

target_include_directories(galois PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(galois PUBLIC ${Boost_LIBRARIES})
