digital_header = digital_data.h bit_operator.h powtwo.h config.h

lib_LIBRARIES = libmcqmcint.a

libmcqmcint_a_SOURCES = MCQMCIntegration.cpp \
	DigitalNet.cpp $(digital_header) \
	nxlw_data.cpp \
	solw_data.cpp

AM_CXXFLAGS = -I../include -O3 -Wall -Wextra -D__STDC_CONSTANT_MACROS
