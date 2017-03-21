noinst_PROGRAMS = qmc_example mc_example qmc_largesobol_example

qmc_example_DEPENDENCIES = ../src/libmcqmcint.a
qmc_example_SOURCES = qmc_example.cpp
qmc_example_LDADD = -lmcqmcint
qmc_example_LDFLAGS = -L../src

mc_example_DEPENDENCIES = ../src/libmcqmcint.a
mc_example_SOURCES = mc_example.cpp
mc_example_LDADD = -lmcqmcint
mc_example_LDFLAGS = -L../src

qmc_largesobol_example_DEPENDENCIES = ../src/libmcqmcint.a
qmc_largesobol_example_SOURCES = qmc_largesobol_example.cpp kahan.hpp \
oscillatory.hpp
qmc_largesobol_example_LDADD = -lmcqmcint
qmc_largesobol_example_LDFLAGS = -L../src

AM_CXXFLAGS = -I../include -O3 -Wall -Wextra -D__STDC_CONSTANT_MACROS
