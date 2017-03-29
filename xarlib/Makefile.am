#########################################################################
# LIBRARY SECTION
#########################################################################

# this is the XarLib library.
# it's name is used for all
# of the other Makefile.am variables
#noinst_LIBRARIES	= libXar.a

# Manually set the package include dir, else we install libraries
# in /usr/include/XaraLX which is not a good thing
pkgincludedir=$(includedir)/xarlib

# Main target
lib_LIBRARIES = libXar.a

# have to use the -I flags to point at the three directories concerned as I
# can't get this file to compile pretranslated source files.
XARLIBFLAGS = \
	-DUSE_NATIVE_XLONG -DBUILDSHADOWS -DOLD_MATRIX_TRANSFORMATIONS \
	-DVECTOR_STROKING -DEXCLUDE_FROM_XARLIB -DNEW_SHADOW_RENDER -DNO_XARACMS \
	-DNEW_FEATURES -DSHOWPORTNOTE -DDO_EXPORT \
	-Iinclude -I$(TOPDIR)/PreComp -I$(TOPDIR)/Kernel -I$(TOPDIR)/wxOil -I$(TOPDIR)/GDraw

# make sure this does NOT have our include files in the path
# xarlib needs to be first
libXar_a_CXXFLAGS =  $(XARALXCXXFLAGS) $(XARLIBFLAGS)

# Ensure these all begin with "../<directory>/" even if in the current directory
XARLIBCPP = \
	../Kernel/ccfile.cpp \
	../Kernel/ccmaths.cpp \
	../Kernel/ccpanose.cpp \
	../Kernel/coord.cpp \
	../Kernel/cxfile.cpp \
	../Kernel/cxfmap.cpp \
	../Kernel/cxfrec.cpp \
	../Kernel/cxfrech.cpp \
	../Kernel/doccoord.cpp \
	../Kernel/docrect.cpp \
	../Kernel/f16spec.cpp \
	../Kernel/fixed16.cpp \
	../Kernel/fixed.cpp \
	../Kernel/fttyplis.cpp \
	../Kernel/list.cpp \
	../Kernel/listitem.cpp \
	../Kernel/matrix.cpp \
	../Kernel/pathproc.cpp \
	../Kernel/paths.cpp \
	../Kernel/pathutil.cpp \
	../Kernel/rechcomp.cpp \
	../Kernel/vector.cpp \
	../Kernel/xlong.cpp \
	../Kernel/zdeflate.cpp \
	../Kernel/zdftrees.cpp \
	../Kernel/ziftrees.cpp \
	../Kernel/zinflate.cpp \
	../Kernel/zstream.cpp \
	../wxOil/basestr.cpp \
	../wxOil/camresource.cpp \
	../wxOil/ccobject.cpp \
	../wxOil/errors.cpp \
	../wxOil/filehndl.cpp \
	../wxOil/filesize.cpp \
	../wxOil/fixmem.cpp \
	../wxOil/fixst128.cpp \
	../wxOil/fixst256.cpp \
	../wxOil/fixstr16.cpp \
	../wxOil/fixstr32.cpp \
	../wxOil/fixstr64.cpp \
	../wxOil/fixstr8.cpp \
	../wxOil/handles.cpp \
	../wxOil/makemsg.cpp \
	../wxOil/memblk.cpp \
	../wxOil/cammemory.cpp \
	../wxOil/pathname.cpp \
	../wxOil/unicdman.cpp \
	../wxOil/varstr.cpp \
	../xarlib/camelot.cpp \
	../xarlib/ensure.cpp \
	../xarlib/xarlib.cpp


INSTCPP = \
	ccfile.cpp \
	ccmaths.cpp \
	ccpanose.cpp \
	coord.cpp \
	cxfile.cpp \
	cxfmap.cpp \
	cxfrec.cpp \
	cxfrech.cpp \
	doccoord.cpp \
	docrect.cpp \
	f16spec.cpp \
	fixed16.cpp \
	fixed.cpp \
	fttyplis.cpp \
	list.cpp \
	listitem.cpp \
	matrix.cpp \
	pathproc.cpp \
	paths.cpp \
	pathutil.cpp \
	rechcomp.cpp \
	vector.cpp \
	xlong.cpp \
	zdeflate.cpp \
	zdftrees.cpp \
	ziftrees.cpp \
	zinflate.cpp \
	zstream.cpp \
	basestr.cpp \
	camresource.cpp \
	ccobject.cpp \
	errors.cpp \
	filehndl.cpp \
	filesize.cpp \
	fixmem.cpp \
	fixst128.cpp \
	fixst256.cpp \
	fixstr16.cpp \
	fixstr32.cpp \
	fixstr64.cpp \
	fixstr8.cpp \
	handles.cpp \
	makemsg.cpp \
	memblk.cpp \
	cammemory.cpp \
	pathname.cpp \
	unicdman.cpp \
	varstr.cpp \
	camelot.cpp \
	ensure.cpp \
	xarlib.cpp


# Ensure these all begin with "../<directory>/" even if in the current directory
XARLIBHEADERS = \
	../PreComp/camtypes.h \
	../wxOil/basestr.h \
	../wxOil/camresource.h \
	../wxOil/camstring.h \
	../wxOil/ccobject.h \
	../wxOil/compatdef.h \
	../wxOil/errors.h \
	../wxOil/exception.h \
	../wxOil/fixmem.h \
	../wxOil/fixst128.h \
	../wxOil/fixst256.h \
	../wxOil/fixstr16.h \
	../wxOil/fixstr32.h \
	../wxOil/fixstr64.h \
	../wxOil/fixstr8.h \
	../wxOil/handles.h \
	../wxOil/hardwaremanager.h \
	../wxOil/memblk.h \
	../wxOil/memdebug.h \
	../wxOil/cammemory.h \
	../wxOil/mfccontainer.h \
	../wxOil/pathname.h \
	../wxOil/stdwx.h \
	../wxOil/svnversion.h \
	../wxOil/unicdman.h \
	../wxOil/varstr.h \
	../GDraw/gconsts.h \
	../Kernel/attr.h \
	../Kernel/ccfile.h \
	../Kernel/ccmaths.h \
	../Kernel/ccpanose.h \
	../Kernel/coord.h \
	../Kernel/cxfarrow.h \
	../Kernel/cxfcols.h \
	../Kernel/cxfdash.h \
	../Kernel/cxfdefs.h \
	../Kernel/cxfile.h \
	../Kernel/cxfmap.h \
	../Kernel/cxfrec.h \
	../Kernel/cxfrech.h \
	../Kernel/cxftags.h \
	../Kernel/doccoord.h \
	../Kernel/docrect.h \
	../Kernel/f16spec.h \
	../Kernel/fixed16.h \
	../Kernel/fixed.h \
	../Kernel/fttyplis.h \
	../Kernel/list.h \
	../Kernel/listitem.h \
	../Kernel/macros.h \
	../Kernel/matrix.h \
	../Kernel/pathproc.h \
	../Kernel/paths.h \
	../Kernel/pathtype.h \
	../Kernel/pathutil.h \
	../Kernel/rechcomp.h \
	../Kernel/taglists.h \
	../Kernel/vector.h \
	../Kernel/xlong.h \
	../Kernel/zdeflate.h \
	../Kernel/zinflate.h \
	../Kernel/zlib.h \
	../Kernel/zstream.h \
	../Kernel/zutil.h \
	../xarlib/camelot.h \
	../xarlib/ensure.h \
	../xarlib/xarlib.h

INSTHEADERS = \
	include/xarlib/camtypes.h \
	include/xarlib/basestr.h \
	include/xarlib/camresource.h \
	include/xarlib/camstring.h \
	include/xarlib/ccobject.h \
	include/xarlib/compatdef.h \
	include/xarlib/errors.h \
	include/xarlib/exception.h \
	include/xarlib/fixmem.h \
	include/xarlib/fixst128.h \
	include/xarlib/fixst256.h \
	include/xarlib/fixstr16.h \
	include/xarlib/fixstr32.h \
	include/xarlib/fixstr64.h \
	include/xarlib/fixstr8.h \
	include/xarlib/handles.h \
	include/xarlib/hardwaremanager.h \
	include/xarlib/memblk.h \
	include/xarlib/memdebug.h \
	include/xarlib/cammemory.h \
	include/xarlib/mfccontainer.h \
	include/xarlib/pathname.h \
	include/xarlib/stdwx.h \
	include/xarlib/svnversion.h \
	include/xarlib/unicdman.h \
	include/xarlib/varstr.h \
	include/xarlib/gconsts.h \
	include/xarlib/attr.h \
	include/xarlib/ccfile.h \
	include/xarlib/ccmaths.h \
	include/xarlib/ccpanose.h \
	include/xarlib/coord.h \
	include/xarlib/cxfarrow.h \
	include/xarlib/cxfcols.h \
	include/xarlib/cxfdash.h \
	include/xarlib/cxfdefs.h \
	include/xarlib/cxfile.h \
	include/xarlib/cxfmap.h \
	include/xarlib/cxfrec.h \
	include/xarlib/cxfrech.h \
	include/xarlib/cxftags.h \
	include/xarlib/doccoord.h \
	include/xarlib/docrect.h \
	include/xarlib/f16spec.h \
	include/xarlib/fixed16.h \
	include/xarlib/fixed.h \
	include/xarlib/fttyplis.h \
	include/xarlib/list.h \
	include/xarlib/listitem.h \
	include/xarlib/macros.h \
	include/xarlib/matrix.h \
	include/xarlib/pathproc.h \
	include/xarlib/paths.h \
	include/xarlib/pathtype.h \
	include/xarlib/pathutil.h \
	include/xarlib/rechcomp.h \
	include/xarlib/taglists.h \
	include/xarlib/vector.h \
	include/xarlib/xlong.h \
	include/xarlib/zdeflate.h \
	include/xarlib/zinflate.h \
	include/xarlib/zlib.h \
	include/xarlib/zstream.h \
	include/xarlib/zutil.h \
	include/xarlib/camelot.h \
	include/xarlib/ensure.h \
	include/xarlib/xarlib.h

# These ones not got from TOPDIR
BUILTXARLIBHEADERS = ../wxOil/svnversion.h

# INSTCPP = $(notdir $(XARLIBCPP) )

libXar_a_SOURCES = $(XARLIBCPP) $(INSTHEADERS)
# Only build the static library until we have an API/ABI nailed down
#libXar_a_LIBADD = $(WX_LIBS2)

pkginclude_HEADERS = $(INSTHEADERS)

BUILT_SOURCES = $(INSTHEADERS) svnversion.h
#BUILT_SOURCES = $(INSTHEADERS) $(INSTCPP)

# Note this is OUR svnversion.h
MOSTLYCLEANFILES = $(INSTHEADERS) svnversion.h
#MOSTLYCLEANFILES = $(INSTHEADERS) $(INSTCPP)

# Insert an additional dependency to get the headers and cpp files built
noinst_DATA = $(INSTHEADERS)
#noinst_DATA = $(INSTHEADERS) $(INSTCPP)

# Generate a directory of includes which has "xarlib/" in the include path
$(INSTHEADERS) : $(XARLIBHEADERS)
	@mkdir -p include/xarlib && \
	j=xarlib/$(filter %/$(notdir $@), $(XARLIBHEADERS) ) && \
	s="$(if $(filter %/$(notdir $@),$(BUILTXARLIBHEADERS)),../$$j,$(TOPDIR)/$$j)" && \
	echo "Building xarlib header $@ from $$s" && \
	perl -pe 's,^[ \t]*#include[ \t]+\",#include \"xarlib/,' <$$s >$@

# We need svnversion.h in the output directory
svnversion.h : $(INSTHEADERS)
	@cp include/xarlib/svnversion.h svnversion.h

# Generate a directory of includes which has "xarlib/" in the include path
# We have to muck around with the prefixes here to avoid partial matches (coord.cpp matching doccoord.cpp etc.)
# $(INSTCPP) : $(XARLIBCPP)
# 	@j=$(filter %/$(notdir $@), $(addprefix xarlib/, $(XARLIBCPP)) ) && \
# 	echo "Building xarlib CPP file $@ from $(TOPDIR)/$$j" && \
# 	ls -la $(TOPDIR)/$$j && \
# 	#perl -pe 's,^[ \t]*#include[ \t]+\",#include \"xarlib/,' <$(TOPDIR)/$$j >$@


pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = Xar.pc

# Unser this as it upsets CXXLINK which causes linking to fail for no apparent reason
AM_CXXFLAGS =

bin_PROGRAMS = expandxar
expandxar_SOURCES = ExpandXar/expandxar.cpp
expandxar_LDADD = libXar.a $(WX_LIBS)
expandxar_CXXFLAGS = $(XARALXCXXFLAGS) $(XARLIBFLAGS)
