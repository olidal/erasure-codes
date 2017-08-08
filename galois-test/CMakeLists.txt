# galois-test

file(GLOB sources
	*.cpp
)

foreach(src_file ${sources})
	get_filename_component(test_name ${src_file} NAME_WE)

	add_executable(${test_name} ${src_file})

	add_dependencies(${test_name} galois)

	set_target_properties(${test_name} PROPERTIES FOLDER Test)

	target_include_directories(${test_name} PRIVATE ../galois)
	target_link_libraries(${test_name} PRIVATE galois)

	add_test(NAME ${test_name} COMMAND galois-test)
endforeach(src_file)
