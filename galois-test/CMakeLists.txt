# galois-test

file(GLOB sources
	*.cpp
)

add_executable(galois-test ${sources})

find_package(gtest REQUIRED)

add_dependencies(galois-test galois)

target_include_directories(galois-test PRIVATE ${GTEST_INCLUDE_DIRS})
target_include_directories(galois-test PRIVATE ../galois)

target_link_libraries(galois-test PRIVATE ${GTEST_LIBRARIES})
target_link_libraries(galois-test PRIVATE galois)
