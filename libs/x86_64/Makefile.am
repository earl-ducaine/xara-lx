#########################################################################
# LIBRARY SECTION
#########################################################################
# this is librairy that is part of main executable.  it's name is used for all
# of the other Makefile.am variables

BUILT_SOURCES =
# Note we don't clean xrc.d and xrc.stamp here to avoid a loop

CLEANFILES =

# explicitly remove resources.h from TOPDIR as well. It should not be there on an out of tree build,
# but running build-resources manually (or switching to an out-of-tree build) can leave it there
# which then confuses things
MOSTLYCLEANFILES = 

noinst_LIBRARIES = libCDraw.a

libwxOil_a_CXXFLAGS = $(XARALXCXXFLAGS) $(XARAFLAGS)

# the application source, library search path, and link libraries
libwxOil_a_SOURCES =

# Keep resources.cpp on a separate line as it is odd

AM_CXXFLAGS = $(XARAFLAGS)

XARALANGUAGE = @XARALANGUAGE@

libCDraw.a:
	cp orphaned-object-files/*.o .
	ar -s -r libCDraw.a *.o
