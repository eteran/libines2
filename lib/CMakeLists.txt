cmake_minimum_required(VERSION 3.0)

find_package(ZLIB)

if(NOT ZLIB_FOUND)
	set(ZLIB_LIBRARIES "")
	set(ZLIB_INCLUDES "")
endif()

add_library(ines2 
	Rom.cpp
	Header.cpp)
	
target_link_libraries(ines2 LINK_PUBLIC
	${ZLIB_LIBRARIES}
)

target_include_directories(ines2 PUBLIC
	${PROJECT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/../include
	${ZLIB_INCLUDES}
)

set_target_properties(ines2
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)


if(NOT ZLIB_FOUND)
	target_compile_definitions(ines2 PUBLIC -DZLIB_NOT_FOUND)
endif()
set_property(TARGET ines2 PROPERTY CXX_STANDARD 14)
set_property(TARGET ines2 PROPERTY CXX_EXTENSIONS OFF)
