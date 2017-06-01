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
    ../configure --with-gtk --enable-debug --enable-unicode --with-libpng=builtin
    make -j 8
    sudo make install
    sudo ldconfig
    cd ..
    cd ..

}

function build_wx_from_git {
    # https://github.com/wxWidgets/wxWidgets.git
    VERSION=$1
    if [ ! -d wxWidgets.git  ]; then
	git clone https://github.com/wxWidgets/wxWidgets.git wxWidgets.git
    fi
    cd wxWidgets.git
    if [ -d buildgtk ]; then
	cd buildgtk
	sudo make uninstall
	cd ..
	sudo rm -rf buildgtk
    fi
    sudo rm -rf *
    git fetch --tags
    git checkout -f $VERSION
    mkdir buildgtk
    cd buildgtk
    ../configure --with-gtk --enable-debug --enable-unicode --with-libpng=builtin
    make -j 8
    sudo make install
    sudo ldconfig
    cd ..
    cd ..
}

function run_build_wx_from_git {
    # WX_2_8_12 772c594b16fc6a713dd2d0ab70ae7a05a65725ff
    # 83ef06d42e
    # '*' indicates bad build
    # d180df5c431d4dc9e788141794fd51fe1f39ad3e  Dec  2 23:49:48 2012
    # 91d98deb2f91                              Dec 31 19:38:49 2010
    # f203de0cc435ab836505ad32b88cda5dc318db68  Jul 22 12:09:04 2010 *
    # 0e601bf087b54ee4f216e538dd2a767ddc937d77  Jul 18 13:39:16 2010 *
    # 05a09b0b9f9d93cd2a149efc0c1ccc0790cdc82c  Oct 22 08:22:33 2007 *
    # WX_2_9_0                                  Sep  7 04:53:58 2009 *
    # WX_2_9_1                                                       *
    # WX_2_9_2                                                       *
    # WX_2_9_3                                                       
    build_wx_from_git WX_2_9_3
}


# https://sourceforge.net/projects/libpng/files/libpng14/1.4.19/libpng-1.4.19.tar.gz/download

function build_png {
    if [ ! -d libpng-1.6.29 ]; then
	if [ ! -f libpng-1.6.29.tar.gz ]; then
	    curl -LO https://superb-sea2.dl.sourceforge.net/project/libpng/libpng16/1.6.29/libpng-1.6.29.tar.gz
	fi
	tar xf libpng-1.6.29.tar.gz
    fi
    cd libpng-1.6.29
    ./configure
    make -j 2
    cd ..
}

function build_xoamorph {
    ./autogen.sh
    ./configure --enable-debug
    # cd libs/x86_64
    #  ar -s -r libCDraw.a *.o
    # cd ../..
    # ar -xv libCDraw.a
    export PATH="/usr/lib/ccache:$PATH"; make -j 4 1> make-out.txt 2>make-error.txt
    # make -j 4
}



function make_tags {
    find . -name "*.c" -o -name "*.cpp" -o -name "*.h" -print | xargs etags -a
}
