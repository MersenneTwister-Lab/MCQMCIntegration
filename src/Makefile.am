digital_header = digital.h bit_operator.h config.h sobolpoint.h

lib_LIBRARIES = libmcqmcint.a

libmcqmcint_a_SOURCES = MCQMCIntegration.cpp \
	DigitalNet.cpp $(digital_header) \
	sobolpoint.cpp interlaced_sobolpoint.cpp

noinst_PROGRAMS = sobolpoint
sobolpoint_SOURCES = sobolpoint_main.cpp sobolpoint.cpp

check_PROGRAMS = test_minmax test_dn
test_minmax_SOURCES = test_minmax.cpp
test_dn_SOURCES = test_dn.cpp

TESTS = test_minmax test_dn

test_minmax_DEPENDENCIES = ./libmcqmcint.a
test_minmax_LDADD = -lmcqmcint
test_minmax_LDFLAGS = -L./
test_dn_DEPENDENCIES = ./libmcqmcint.a
test_dn_LDADD = -lmcqmcint
test_dn_LDFLAGS = -L./

AM_CXXFLAGS = -I../include -O3 -Wall -Wextra -D__STDC_CONSTANT_MACROS
