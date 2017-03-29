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
    cd ..
}

function build_xoamorph {
    ./autogen.sh
    ./configure
    make
}
