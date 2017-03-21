digital_header = digital_data.h bit_operator.h powtwo.h config.h \
sobolpoint.h

lib_LIBRARIES = libmcqmcint.a

libmcqmcint_a_SOURCES = MCQMCIntegration.cpp \
	DigitalNet.cpp $(digital_header) \
	nxlw_data.cpp \
	solw_data.cpp \
	sobolpoint.cpp

noinst_PROGRAMS = sobolpoint
sobolpoint_SOURCES = sobolpoint_main.cpp sobolpoint.cpp

AM_CXXFLAGS = -I../include -O3 -Wall -Wextra -D__STDC_CONSTANT_MACROS
