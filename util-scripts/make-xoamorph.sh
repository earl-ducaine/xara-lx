#!/usr/bin/env bash

function build_wx_30 {
    if [ ! -d wxWidgets-3.0.3 ]; then
	if [ ! -f wxGTK-3.0.3.tar.gz ]; then
	    curl -LO https://github.com/wxWidgets/wxWidgets/releases/download/v3.0.3/wxWidgets-3.0.3.tar.bz2
	fi
	tar xf wxWidgets-3.0.3.tar.bz2
    fi
    cd wxWidgets-3.0.3
    if [ -d buildgtk ]; then
	cd buildgtk
	make uninstall
	cd ..
	rm -rf buildgtk
    fi
    mkdir buildgtk
    cd buildgtk
    ../configure --enable-debug --enable-unicode --enable-aui --with-gtk=3 --disable-shared
#    ../configure --with-gtk --enable-debug --enable-unicode --enable-aui --with-libpng=builtin
    make -j 8
    cd ..
    cd ..
}

function build_wx_from_git {
    # https://github.com/wxWidgets/wxWidgets.git
    VERSION=$1
    if [ ! -d $VERSION ]; then
	git clone --depth=1 https://github.com/wxWidgets/wxWidgets.git $VERSION
    fi
    cd $VERSION
    rm -rf *
    git fetch --tags
    git checkout -f $VERSION
    if [ -d buildgtk ]; then
	cd buildgtk
	sudo make uninstall
	cd ..
	rm -rf buildgtk
    fi
    mkdir buildgtk
    cd buildgtk
    ../configure --with-gtk --enable-debug --enable-unicode --with-libpng=builtin
    export PATH="/usr/lib/ccache:$PATH"; make -j 8
    cd ..
    cd ..
}

function build_freetype_281 {
    if [ ! -d freetype-2.8 ]; then
	if [ ! -f freetype-2.8.1.tar.bz2 ]; then
	    curl -LO https://bigsearcher.com/mirrors/nongnu/freetype/freetype-2.8.1.tar.bz2
	fi
	tar xf freetype-2.8.1.tar.bz2
    fi
    cd freetype-2.8.1
    ./autogen.sh
    ./configure
    make -j 8
    cd ..
}

function build_xoamorph_debug {
    build_xoamorph "--enable-debug"
}

function build_xoamorph {
    # Don't rebuild wxWidgets if folder already exists.
    if [ ! -d wxWidgets-3.0.3 ]; then
	echo "Building xwWidgets..."
	build_wx_30
    fi
    # Don't rebuild freetype if folder already exists.
    if [ ! -d freetype-2.8 ]; then
	echo "Building freetype-2.8..."
	build_freetype_281
    fi
    autoreconf -i -f
    ./configure $1 --with-wx-config=wxWidgets-3.0.3/buildgtk/wx-config \
		--enable-static-exec \
		--with-freetype-config=freetype-2.8.1/builds/unix/freetype-config \
	|| echo "Unable to build Xoamporph. Check make-error.txt"
    # cd libs/x86_64
    #  ar -s -r libCDraw.a *.o
    # cd ../..
    # ar -xv libCDraw.a
    export PATH="/usr/lib/ccache:$PATH"; make -j 4 1> make-out.txt 2>make-error.txt
    # make -j 4
    # Turn off debug
}

function make_tags {
    find . -name "*.c" -o -name "*.cpp" -o -name "*.h" -print | xargs etags -a
}


function setup_alternatives {
    sudo update-alternatives --remove-all gcc
    sudo update-alternatives --remove-all g++

    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 10
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-6 20

    sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-5 10
    sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-6 20
}

function create_apply_patch_from_alt_branch {
    git --no-pager diff rewrite-cdraw
    FILE=$1
    git --no-pager diff diff rewrite-cdraw  "wxOil/$FILE" > "$FILE.patch"
    git apply -R "$FILE".patch; make -j 8
}

function build_doxygen_files {
    doxygen Doxyfile
}

# Setup continuous document build.
function enable_doxygen_file_monitering {
    inotify-hookable \
	-w doc/doxygen \
	-w tools \
	-w Kernel \
	-w wxOil \
	-w util-scripts \
	-f Doxyfile -c 'doxygen Doxyfile'
}

function build_resources {
    ./Scripts/build-resources.sh
    cd wxOil/
    mv resources.cpp resources.h
    git checkout -f resources.cpp
    cd ..
}
