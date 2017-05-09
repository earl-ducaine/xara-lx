#########################################################################
# LIBRARY SECTION
#########################################################################
# this is librairy that is part of main executable.  it's name is used for all
# of the other Makefile.am variables


OBJECT_FILES =  \
	libCDraw_la-cstroke.o \
	libCDraw_la-GBevel.o \
	libCDraw_la-gclip0.o \
	libCDraw_la-gclip1.o \
	libCDraw_la-gclip2.o \
	libCDraw_la-gclip.o \
	libCDraw_la-gcmaths.o \
	libCDraw_la-GColour.o \
	libCDraw_la-GContext.o \
	libCDraw_la-GConvert.o \
	libCDraw_la-GDraw.o \
	libCDraw_la-GError.o \
	libCDraw_la-GGrad.o \
	libCDraw_la-GHue.o \
	libCDraw_la-GMain.o \
	libCDraw_la-GMaths.o \
	libCDraw_la-gmem.o \
	libCDraw_la-GMemory.o \
	libCDraw_la-GPath.o \
	libCDraw_la-GRegion.o \
	libCDraw_la-GScanAA.o \
	libCDraw_la-GScanBvl.o \
	libCDraw_la-GScanC4.o \
	libCDraw_la-GScanCnv.o \
	libCDraw_la-GScanC.o \
	libCDraw_la-GScanIni.o \
	libCDraw_la-GScanL4.o \
	libCDraw_la-GScanL.o \
	libCDraw_la-GScan.o \
	libCDraw_la-GScanR4.o \
	libCDraw_la-GScanR.o \
	libCDraw_la-GScanRR.o \
	libCDraw_la-GScanS4.o \
	libCDraw_la-GScanSF.o \
	libCDraw_la-GScanSI.o \
	libCDraw_la-GScanS.o \
	libCDraw_la-GScanSq4.o \
	libCDraw_la-GScanSq.o \
	libCDraw_la-GScanT-A.o \
	libCDraw_la-GScanT-B.o \
	libCDraw_la-GScanT-Bvl.o \
	libCDraw_la-GScanT-C.o \
	libCDraw_la-GScanT-D.o \
	libCDraw_la-GScanT-H.o \
	libCDraw_la-GScanT-L.o \
	libCDraw_la-GScanT-Lu.o \
	libCDraw_la-GScanT.o \
	libCDraw_la-GScanT-R.o \
	libCDraw_la-GScanT-Sn.o \
	libCDraw_la-GScanT-S.o \
	libCDraw_la-GScanX34.o \
	libCDraw_la-GScanX3.o \
	libCDraw_la-GScanX44.o \
	libCDraw_la-GScanX4.o \
	libCDraw_la-GScroll.o \
	libCDraw_la-GSprite.o \
	libCDraw_la-GStroke.o \
	libCDraw_la-GStyle.o \
	libCDraw_la-GTableBevel.o \
	libCDraw_la-GTableD.o \
	libCDraw_la-GTableM.o \
	libCDraw_la-GTable.o \
	libCDraw_la-GTableP.o         

BUILT_SOURCES =
# Note we don't clean xrc.d and xrc.stamp here to avoid a loop

CLEANFILES =

# explicitly remove resources.h from TOPDIR as well. It should not be there on an out of tree build,
# but running build-resources manually (or switching to an out-of-tree build) can leave it there
# which then confuses things
MOSTLYCLEANFILES = 

noinst_LIBRARIES = libCDraw.a

libCDraw_a_CXXFLAGS = $(XARALXCXXFLAGS) $(XARAFLAGS)

# the application source, library search path, and link libraries
libCDraw_a_SOURCES =

# Keep resources.cpp on a separate line as it is odd

AM_CXXFLAGS = $(XARAFLAGS)

XARALANGUAGE = @XARALANGUAGE@

libCDraw.a:
	cd orphaned-object-files; cp $(OBJECT_FILES) ../; cd ..
	ar -s -r libCDraw.a *.o
