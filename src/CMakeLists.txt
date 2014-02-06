
project (poseidon)

cmake_minimum_required (VERSION 2.8)

# Find boost library
include(FindBoost)
find_package( Boost 1.46.0 COMPONENTS program_options REQUIRED)

include_directories(include ${Boost_INCLUDE_DIRS})
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
	#-stdlib=libc++
	#-stdlib=libstdc++
    #-std=c++0x
    # Other flags
)
add_executable(poseidon ${SOURCES} ${HEADERS})

target_link_libraries(poseidon ${PROJECT_SOURCE_DIR}/../lib/pugixml/build/libpugixml.a ${Boost_LIBRARIES})

set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)

if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
	add_definitions( -DDEBUG_VERBOSE=VERBOSE )
endif( CMAKE_BUILD_TYPE STREQUAL "Debug" )



ENABLE_TESTING()
INCLUDE(Dart)

add_test(poseidonRuns poseidon -h)
set_tests_properties(poseidonRuns PROPERTIES PASS_REGULAR_EXPRESSION "Help menu")

add_test(poseidonBitorus poseidon -p ../data/bitorus3x3.xml -m GREEDY)
set_tests_properties(poseidonBitorus PROPERTIES PASS_REGULAR_EXPRESSION "Schedule verified.")

add_test(poseidonMesh poseidon -p ../data/mesh3x3.xml -m GREEDY)
set_tests_properties(poseidonMesh PROPERTIES PASS_REGULAR_EXPRESSION "Schedule verified.")

add_test(poseidonCustom poseidon -d -p ../data/ugly.xml -m GREEDY)
set_tests_properties(poseidonCustom PROPERTIES PASS_REGULAR_EXPRESSION "Schedule verified.")
