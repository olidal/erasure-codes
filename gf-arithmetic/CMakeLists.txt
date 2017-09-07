
file(GLOB sources
	*.cpp
	*.h
)

# Generate log_tables.cpp
add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/log_tables.cpp
	COMMAND generate-log-tables log_tables.cpp
)
# Make dependencies work
add_custom_target(log_tables log_tables.cpp)
# log_tables depends on generate-log-tables
add_dependencies(log_tables generate-log-tables)
set_target_properties(log_tables PROPERTIES FOLDER internal-targets)

# Generate lohi_tables.cpp
add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lohi_table.cpp
	COMMAND generate-lohi-table > lohi_table.cpp
)
# Make dependencies work
add_custom_target(lohi_table lohi_table.cpp)
add_dependencies(lohi_table generate-lohi-table)
set_target_properties(lohi_table PROPERTIES FOLDER internal-targets)

# gfarithmetic
add_library(gfarithmetic ${sources} log_tables.cpp)

# gfarithmetic needs log_tables
add_dependencies(gfarithmetic log_tables)
