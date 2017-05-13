#########################################################################
# LIBRARY SECTION
#########################################################################
# this is librairy that is part of main executable.  it's name is used for all
# of the other Makefile.am variables


#########################################################################
# LIBRARY SECTION
#########################################################################

ARFLAGS = "Drv"

# explicitly remove resources.h from TOPDIR as well. It should not be
# there on an out of tree build, but running build-resources manually
# (or switching to an out-of-tree build) can leave it there which then
# confuses things
noinst_LIBRARIES = libCDraw-alt.a

libCDraw_alt_a_CXXFLAGS = $(XARALXCXXFLAGS) $(XARAFLAGS)



libCDraw_alt_a_SOURCES = \
	bshadow2.cpp



AM_CXXFLAGS = $(XARAFLAGS)

XARALANGUAGE = @XARALANGUAGE@

