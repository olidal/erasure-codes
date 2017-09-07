
file(GLOB sources
	*.cpp
	*.h
)

add_executable(generate-lohi-table ${sources} log_tables.cpp)

add_dependencies(generate-lohi-table log_tables)
