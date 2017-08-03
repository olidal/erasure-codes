# galois

file(GLOB sources
	*.cpp
	*.h
)

add_definitions(-DGALOIS_BUILD)

add_library(galois SHARED ${sources})
