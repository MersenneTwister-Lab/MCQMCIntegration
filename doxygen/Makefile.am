EXTRA_DIST = mainpage.txt doxygen.cfg
if HAVE_DOXYGEN

incdir = $(top_srcdir)/include/MCQMCIntegration
incfiles = $(incdir)/DigitalNet.h \
	$(incdir)/MCQMCIntegration.h

doxyfile.stamp: mainpage.txt $(incfiles)
	$(DOXYGEN) doxygen.cfg
	echo Timestamp > doxyfile.stamp

CLEANFILES = doxyfile.stamp

all-local: doxyfile.stamp
endif
