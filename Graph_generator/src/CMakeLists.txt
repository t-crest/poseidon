cmake_minimum_required (VERSION 2.8)
project (graph_gen)

# Find boost library
include(FindBoost)
find_package( Boost 1.46.0 COMPONENTS program_options REQUIRED)

include_directories(include ${Boost_INCLUDE_DIRS})
include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_BINARY_DIR}/../../lib/pugixml/src")
link_directories("${PROJECT_BINARY_DIR}")
set(SOURCES
	main.cpp
	graph_generator.cpp
	lex_cast.cpp
	options.cpp
	util.cpp
)

set(HEADERS
	graph_generator.h
	higher_order.h
	lex_cast.h
	options.h
	util.hpp
)

ADD_DEFINITIONS(
    -std=c++0x
    # Other flags
)
add_executable(graph_gen ${SOURCES} ${HEADERS})

target_link_libraries(graph_gen ${PROJECT_SOURCE_DIR}/../../lib/pugixml/build/libpugixml.a)

set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)
