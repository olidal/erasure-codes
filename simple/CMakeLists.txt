
file(GLOB sources
	*.cpp
	*.h
)

add_executable(simple ${sources})

add_dependencies(simple erasure)

target_include_directories(simple PRIVATE ../erasure-codes)
target_link_libraries(simple PRIVATE erasure)
