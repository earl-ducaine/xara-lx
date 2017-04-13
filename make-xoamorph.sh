#!/usr/bin/env bash

function build_wx {
    if [ ! -d wxGTK-2.8.12 ]; then
	if [ ! -f wxGTK-2.8.12.tar.gz ]; then
	    curl -LO https://github.com/wxWidgets/wxWidgets/releases/download/v2.8.12/wxGTK-2.8.12.tar.gz
	fi
	tar xf wxGTK-2.8.12.tar.gz
    fi
    cd wxGTK-2.8.12
    if [ -d buildgtk ]; then
	cd buildgtk
	sudo make uninstall
	cd ..
	rm -rf buildgtk
    fi
    mkdir buildgtk
    cd buildgtk
    ../configure --with-gtk --enable-unicode
    make -j 8
    sudo make install
    sudo ldconfig
    cd ..
}

function build_png {
    if [ ! -d libpng-1.2.57 ]; then
	if [ ! -f libpng-1.2.57.tar.gz ]; then
	    curl -LO https://superb-sea2.dl.sourceforge.net/project/libpng/libpng12/1.2.57/libpng-1.2.57.tar.gz
	fi
	tar xf libpng-1.2.57.tar.gz
    fi
    cd libpng-1.2.57
    ./configure
    make -j 2
    cd ..
    cd ..
}

function build_xoamorph {
    ./autogen.sh
    ./configure
    make -j 4
}



function make_tags {
    find . -name "*.c" -o -name "*.cpp" -o -name "*.h" -print | xargs etags -a
}
