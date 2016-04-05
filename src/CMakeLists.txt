cmake_minimum_required(VERSION 3.0)

add_executable(ines_test
	main.cpp)
	
target_link_libraries(ines_test LINK_PUBLIC
	ines2
)

include_directories(
	${PROJECT_SOURCE_DIR}/include
)

set_target_properties(ines_test
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

set_property(TARGET ines_test PROPERTY CXX_STANDARD 11)
