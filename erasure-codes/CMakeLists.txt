
file(GLOB sources
	*.h
	*.cpp
)

add_library(erasure ${sources})

target_include_directories(erasure PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(erasure PUBLIC ${Boost_LIBRARIES})
