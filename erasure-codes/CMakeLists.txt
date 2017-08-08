
file(GLOB sources
	*.h
	*.cpp
)

add_project(erasure-codes STATIC ${sources})
