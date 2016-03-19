cmake_minimum_required(VERSION 3.0)
project (libines)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-W -Wall -pedantic -Wuseless-cast -Wold-style-cast -Wzero-as-null-pointer-constant")

add_subdirectory (lib)
add_subdirectory (src)
