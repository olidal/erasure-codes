
file(GLOB sources
	*.cpp
	*.h
)

add_executable(generate-lohi-table ${sources})
