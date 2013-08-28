cmake_minimum_required (VERSION 2.8)
project (SNTs)

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_BINARY_DIR}/../lib/pugixml/src")
link_directories("${PROJECT_BINARY_DIR}")
set(SOURCES
	main.cpp
	draw.cpp
	file.cpp
	lex_cast.cpp
	network_t.cpp
	options.cpp
	output.cpp
	parser.cpp
	schedule.cpp
	schedulers.cpp
	stats.cpp
	svg.cpp
	util.cpp
	vhdlOutput.cpp
	xmlOutput.cpp
)

set(HEADERS
	draw.hpp
	file.hpp
	higher_order.h
	IOutput.h
	lex_cast.h
	matrix.hpp
	maxset.hpp
	network_t.hpp
	options.h
	output.hpp
	parser.hpp
	schedule.hpp
	schedulers.hpp
	stats.hpp
	svg.h
	util.hpp
	vhdlOutput.h
	xmlOutput.h
)

ADD_DEFINITIONS(
    -std=c++0x
    # Other flags
)
add_executable(SNTs ${SOURCES} ${HEADERS})

target_link_libraries(SNTs ${PROJECT_SOURCE_DIR}/../lib/pugixml/build/libpugixml.a)

set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)
