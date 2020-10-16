export XARAFLAGS = -I/usr/local/lib/wx/include/gtk3-unicode-static-3.0 \
        -I../PreComp -I../wxOil -I$(TOPDIR)/Kernel -I$(TOPDIR)/wxOil -I$(TOPDIR)/wxOil/Res \
	-I$(TOPDIR)/tools -I$(TOPDIR)/GDraw -I/usr/local/include/wx-3.0 -I$(TOPDIR)/PreComp -I$(TOPDIR)/wxXtra \
	-DUSE_NATIVE_XLONG -DBUILDSHADOWS -DOLD_MATRIX_TRANSFORMATIONS -fpermissive \
	-DVECTOR_STROKING -DEXCLUDE_FROM_XARALX -DNEW_SHADOW_RENDER -DNO_XARACMS \
	-DNEW_FEATURES -DSHOWPORTNOTE -DDO_EXPORT -D__WXGTK__
export TOPDIR

if PRECOMPILE_ENABLE
     PRECOMPILE_DIR = PreComp
else
     PRECOMPILE_DIR =
endif

if XARLIB_ENABLE
     XARLIB_DIR = xarlib
else
     XARLIB_DIR =
endif

XARLIB2_DIR = xarlib2


if FILTERS_ENABLE
     FILTERS_DIRS = filters/SVGFilter
else
     FILTERS_DIRS =
endif

# Do not compile po directory if WXRC is broken (gentoo wxWidgets 2.6.3.2)
# Leave the '#po' on the end to keep automake quiet
if INTERNATIONAL_ENABLE
PO=po
endif

SUBDIRS = $(PRECOMPILE_DIR) libs wxXtra wxOil Kernel tools GDraw $(XARLIB_DIR) $(FILTERS_DIRS) $(PO)

# We clean these here not in the wxOil makefile to prevent a loop
MOSTLYCLEANFILES=wxOil/xrc/xrc.d wxOil/xrc/xrc.stamp Resources/*

bin_PROGRAMS		= XaraLX

XaraLX_CXXFLAGS = $(XARALXCXXFLAGS) $(AM_CFLAGS) $(libpng_CFLAGS)
XaraLX_CPPFLAGS = $(XARALXCPPFLAGS) $(AM_CPPFLAGS) $(libpng_CPPFLAGS)

if DARWIN_LINK
    XaraLX_SOURCES	= Kernel/*.o wxOil/*.o tools/*.o wxXtra/*.o GDraw/*.o
    XaraLX_LDSOURCE = $(XaraLX_SOURCES)
else
    XaraLX_SOURCES	= Kernel/libKernel.a wxOil/libwxOil.a tools/libTools.a wxXtra/libwxXtra.a GDraw/libGDraw-cairo.a
    XaraLX_LDSOURCE = -Wl,--start-group $(XaraLX_SOURCES) -Wl,--end-group
endif

# XaraLX_LDFLAGS = --debug -L$(srcdir)/$(CDRAW_LIB_DIR) \
# 	-L$(srcdir)/$(CDRAW_LIB_ALT_DIR)  $(WX_LIBS) $(LIBS) $(LIBXML2_LIBS) \
# 	-ljpeg -lCDraw -lCDraw-alt  -latk-1.0 -lgio-2.0 -lpangoft2-1.0 -lpangocairo-1.0 \
# 	-lgdk_pixbuf-2.0 -lcairo -lpango-1.0 -lfreetype -lfontconfig -lgobject-2.0 -lglib-2.0 \
# 	$(libpng_LIBS) -lz

XaraLX_LDFLAGS = --debug -L$(srcdir)/$(CDRAW_LIB_DIR) \
	$(LIBXML2_LIBS) -lpangoft2-1.0 -lfreetype -lz \
	-llzma  $(WX_LIBS) -licuuc

XaraLX_LD ?= $(CXX)

XaraLX$(EXEEXT) : $(XaraLX_SOURCES)
	$(XaraLX_LD) -no-pie $(XaraLX_LDSOURCE) $(XaraLX_LDFLAGS) -o $@

UNZIP = unzip

release: all

resources:
	( rm -rf Resources ; mkdir -p Resources ; cd Resources ; $(UNZIP) -q ../wxOil/xrc/resources.xrs )

ACLOCAL_AMFLAGS = -I m4
