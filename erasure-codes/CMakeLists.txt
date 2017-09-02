
file(GLOB sources
	*.h
	*.cpp
	encode/*.cpp
)

add_definitions(-DERASURE_BUILD)

add_library(erasure ${sources})

add_dependencies(erasure gfarithmetic)

target_include_directories(erasure PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../gf-arithmetic)
target_include_directories(erasure PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(erasure PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(erasure PUBLIC ${Boost_LIBRARIES})
target_link_libraries(erasure PUBLIC gfarithmetic)
