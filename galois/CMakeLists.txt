# galois

file(GLOB sources
	*.cpp
	*.h
)

add_library(galois SHARED ${sources})
