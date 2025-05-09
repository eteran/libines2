cmake_minimum_required(VERSION 3.15)

find_package(ZLIB)

add_library(iNES2 
	Rom.cpp
	Header.cpp
	include/iNES/Rom.h
	include/iNES/Header.h
	include/iNES/Error.h
)
	
target_include_directories(iNES2
        PUBLIC include
)

if(NOT ZLIB_FOUND)
	target_compile_definitions(iNES2 
		PUBLIC -DZLIB_NOT_FOUND
	)
else()
	target_link_libraries(iNES2
		PUBLIC ZLIB::ZLIB
	)
endif()

set_target_properties(iNES2 PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
)
