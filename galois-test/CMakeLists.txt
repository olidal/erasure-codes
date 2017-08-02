# galois-test

file(GLOB sources
	*.cpp
)

add_executable(galois-test ${sources})

find_package(gtest REQUIRED)

target_include_directories(galois-test PRIVATE ${GTEST_INCLUDE_DIRS})

target_link_libraries(galois-test PRIVATE ${GTEST_LIBRARIES})
