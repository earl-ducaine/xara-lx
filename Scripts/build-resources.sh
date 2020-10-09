#!/bin/sh

# Scripts/build-resources.sh
# Build resources.cpp if necessary
#
# Call with a -f argument to force build of resources even if the
# system doesn't think it necessary

# Note, this script uses wxrc. If you have not installed the toolkit
# into /usr, you'll need to manually add the executable to your path
# when you run this script. Something like:
#
#
# export WXRC="/home/myname/dev/xara/xara-lx/wxWidgets-3.0.3/buildgtk/utils/wxrc/wxrc"; Scripts/build-resources.sh



NEWRES=0

if test "$1" = "-f" ; then
    NEWRES=1
fi

# A checksum routine - it doesn't much matter what it is
if test "$CHECKSUM" = "" ; then
    CHECKSUM=md5sum
fi

if test "$ZIP" = "" ; then
    ZIP=zip
fi

if test "$XARALANGUAGE" = "" ; then
    XARALANGUAGE=EN
fi

if test "$XGETTEXT" = "" ; then
    XGETTEXT=xgettext
fi

if test "$WXRC" = "" ; then
    WXRC=wxrc
fi

# Set TOPDIR if it's not already set
if test "$TOPDIR" = "" ; then
    TOPDIR=`pwd`
fi

cd wxOil
echo Testing for new resources... 1>&2

if ( test ! -d xrc ) ; then
    mkdir xrc
fi

if ! [ -e xrc/xrc.d ] ; then
    touch xrc/xrc.d ; sleep 1
fi

( find $TOPDIR/wxOil/xrc/$XARALANGUAGE -maxdepth 1 -name '*.xrc' ; \
  find $TOPDIR/wxOil/xrc -maxdepth 1 -name '*.png' \
       -or -name '*.ico' \
       -or -name '*.cur' \
       -or -name '*.bmp' \
       -or -name '*.res' \
       -or -name '*.xar' ) | \
    perl -e 'while(<>) {chomp; s/\r//; push @F, $_}; printf "xrc/dialogs.xrc xrc/strings.xrc xrc/strings.lst xrc/xaralx.po: xrc/xrc.stamp %s\n",join (" ",sort @F);' > xrc/xrc.d.$$

if ( ! ( cmp -s xrc/xrc.d.$$ xrc/xrc.d) ) ; then
    touch xrc/xrc.stamp
    mv xrc/xrc.d.$$ xrc/xrc.d
    echo Found new resources 1>&2
    NEWRES=1
else rm -f xrc/xrc.d.$$
fi

if ! [ -e xrc/xrc.stamp ] ; then
    touch xrc/xrc.stamp
    NEWRES=1
fi

# we should here go through all the files in xrc.d and check their dates

XRCDIALOGS=`perl -ane 'foreach $f (@F) { push @G,$f if $f=~/${XARALANGUAGE}\/.+\.xrc$/ && $f!~/-strings\.xrc$/};print join(" ",@G)."\n";' < xrc/xrc.d`
XRCSTRINGS=`perl -ane 'foreach $f (@F) { push @G,$f if $f=~/${XARALANGUAGE}\/.+\.xrc$/ };print join(" ",@G)."\n";' < xrc/xrc.d`
XRCBITMAPS=`perl -ane 'foreach $f (@F) { push @G,$f if $f=~/\.(png|ico|cur|bmp|res|xar)$/};print join(" ",@G)."\n";' < xrc/xrc.d`
XRCALL=`perl -ane 'foreach $f (@F) { push @G,$f if $f=~/${XARALANGUAGE}\/.+$/ };print join(" ",@G)."\n";' < xrc/xrc.d`

if test "$NEWRES" -eq 0 ; then
    if ! [ -e resources.cpp ] ; then
        NEWRES=1
    else
        NEWRES=`echo $XRCALL xrc/xrc.stamp | perl -ane '$x=0;$m=(stat("resources.cpp"))[9];foreach $f (@F) {if ((stat($f))[9]>$m) {$x=1;break;}};print "$x\n";'`
    fi
fi

if test "$NEWRES" -eq 1 ; then
    echo "Rebuilding resources" 1>&2

    rm -f xrc/xrc.check \
       xrc/resources.xrs \
       xrc/dialogs.xrc \
       xrc/strings.xrc \
       xrc/strings.lst 1>&2 2>/dev/null

    echo "Combining dialog & string resources" 1>&2 && \
	touch xrc/strings.xrc && \
	$TOPDIR/Scripts/combinexrc.pl -b missing.png -o xrc/dialogs.xrc $XRCDIALOGS && \
	$TOPDIR/Scripts/combinexrc.pl -b missing.png -t -s -o xrc/strings.lst $XRCSTRINGS && \
	( ( perl -ne 'chomp;s/^\S+\t//;print "_(\"$_\");\n";' \
		 < xrc/strings.lst && $WXRC -g xrc/dialogs.xrc) | \
	      sort -u | \
	      perl -ne 'print unless /^_\(\"\"\)\;\s+$/;' | \
	      sed 's/&amp;/\&/g' | \
	      sed 's/\\r//g' | \
	      $XGETTEXT --force-po \
			-k_ \
			-C \
			-i \
			- \
			--from-code "UTF-8" \
			--no-location \
			--copyright-holder "Xara Group Ltd" \
			--msgid-bugs-address=bugs@xara.com \
			-d xaralx \
			-o xrc/xaralx.po ) && \
	echo "Generating resource checksum and resources.cpp" 1>&2 && \
	( cat xrc/dialogs.xrc xrc/strings.xrc xrc/strings.lst $XRCBITMAPS | \
	      $CHECKSUM | \
	      awk '{print $1}' > xrc/xrc.check ) && \
	$ZIP -9 -j -q xrc/resources.xrs xrc/xrc.check \
	     xrc/dialogs.xrc \
	     xrc/strings.xrc xrc/strings.lst \
	     $XRCBITMAPS && \
	$TOPDIR/Scripts/bin2cpp.pl -f CamResource::GetBinaryFileInfo \
				   xrc/resources.xrs resources.h


fi
