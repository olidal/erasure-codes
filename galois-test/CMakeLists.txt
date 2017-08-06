# galois-test

file(GLOB sources
	*.cpp
)

add_executable(galois-test ${sources})

add_dependencies(galois-test galois)

target_include_directories(galois-test PRIVATE ../galois)

target_link_libraries(galois-test PRIVATE galois)
