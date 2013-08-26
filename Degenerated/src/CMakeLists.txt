cmake_minimum_required (VERSION 2.8)
project (DEGEN)

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_BINARY_DIR}/../../lib/pugixml/src")
link_directories("${PROJECT_BINARY_DIR}")
set(SOURCES
	DEGEN.cpp
	options.cpp
)

set(HEADERS
	options.h
)

ADD_DEFINITIONS(
    -std=c++0x
    # Other flags
)
add_executable(DEGEN ${SOURCES} ${HEADERS})

target_link_libraries(DEGEN ${PROJECT_SOURCE_DIR}/../../lib/pugixml/build/libpugixml.a)

set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)

