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
	sudo make uninstall
	cd ..
	rm -rf buildgtk
    fi
    mkdir buildgtk
    cd buildgtk
    ../configure --enable-debug --enable-unicode --enable-aui --with-gtk=3 --disable-shared
#    ../configure --with-gtk --enable-debug --enable-unicode --enable-aui --with-libpng=builtin
    make -j 8
    sudo make install
    sudo ldconfig
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

function build_xoamorph {
    ./autogen.sh
    ./configure --enable-debug --with-wx-config=wxWidgets-3.0.3/buildgtk/wx-config --enable-static-exec
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
