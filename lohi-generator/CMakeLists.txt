
file(GLOB sources
	*.cpp
	*.h
)

set_source_files_properties(log_tables.cpp PROPERTIES GENERATED TRUE)

add_executable(generate-lohi-table ${sources} log_tables.cpp)

add_dependencies(generate-lohi-table log_tables)
