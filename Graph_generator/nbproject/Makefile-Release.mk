#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=Cygwin_4.x-Windows
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/main.o \
	${OBJECTDIR}/src/graph_generator.o \
	${OBJECTDIR}/src/options.o \
	${OBJECTDIR}/src/util.o \
	${OBJECTDIR}/src/pugixml-1.0/src/pugixml.o \
	${OBJECTDIR}/src/lex_cast.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=--std=c++0x
CXXFLAGS=--std=c++0x

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/graph_generator.exe

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/graph_generator.exe: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/graph_generator ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/src/main.o: src/main.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O2 -Isrc/pugixml-1.0/src -I/cygdrive/C/boost_1_49_0/boost_1_49_0 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/main.o src/main.cpp

${OBJECTDIR}/src/graph_generator.o: src/graph_generator.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O2 -Isrc/pugixml-1.0/src -I/cygdrive/C/boost_1_49_0/boost_1_49_0 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/graph_generator.o src/graph_generator.cpp

${OBJECTDIR}/src/options.o: src/options.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O2 -Isrc/pugixml-1.0/src -I/cygdrive/C/boost_1_49_0/boost_1_49_0 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/options.o src/options.cpp

${OBJECTDIR}/src/util.o: src/util.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O2 -Isrc/pugixml-1.0/src -I/cygdrive/C/boost_1_49_0/boost_1_49_0 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/util.o src/util.cpp

${OBJECTDIR}/src/pugixml-1.0/src/pugixml.o: src/pugixml-1.0/src/pugixml.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/pugixml-1.0/src
	${RM} $@.d
	$(COMPILE.cc) -O2 -Isrc/pugixml-1.0/src -I/cygdrive/C/boost_1_49_0/boost_1_49_0 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/pugixml-1.0/src/pugixml.o src/pugixml-1.0/src/pugixml.cpp

${OBJECTDIR}/src/lex_cast.o: src/lex_cast.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O2 -Isrc/pugixml-1.0/src -I/cygdrive/C/boost_1_49_0/boost_1_49_0 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/lex_cast.o src/lex_cast.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/graph_generator.exe

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
