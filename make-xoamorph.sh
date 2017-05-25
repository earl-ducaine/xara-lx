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
    ../configure --with-gtk --enable-debug --enable-unicode --enable-aui --with-libpng=builtin
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
    ../configure --with-gtk --debug=yes --enable-unicode --with-libpng=builtin
    make -j 8
    sudo make install
    sudo ldconfig
    cd ..
    cd ..
}

function run_build_wx_from_git {
    build_wx_from_git WX_2_9_0
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
}

function build_xoamorph {
    ./autogen.sh
    ./configure --enable-debug
    # cd libs/x86_64
    #  ar -s -r libCDraw.a *.o
    # cd ../..
    # ar -xv libCDraw.a 
    # export PATH="/usr/lib/ccache:$PATH"; make -j 4 1> make-out.txt 2>make-error.txt
    make -j 4
    # make -j 4
}

function last_step {
    g++ -no-pie -Wl,--start-group Kernel/libKernel.a wxOil/libwxOil.a tools/libTools.a wxXtra/libwxXtra.a -Wl,--end-group --debug -L./libs/x86_64 -L./libs/x86_64/cdraw-alt  -L/usr/local/lib -pthread -lwx_gtk2u_aui-3.0 -lwx_gtk2u_xrc-3.0 -lwx_gtk2u_html-3.0 -lwx_gtk2u_qa-3.0 -lwx_gtk2u_adv-3.0 -lwx_gtk2u_core-3.0 -lwx_baseu_xml-3.0 -lwx_baseu_net-3.0 -lwx_baseu-3.0    -lxml2 -ljpeg -lCDraw -lCDraw-alt -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgio-2.0 -lpangoft2-1.0 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lcairo -lpango-1.0 -lfreetype -lfontconfig -lgobject-2.0 -lglib-2.0 ./libpng-1.2.57/.libs/libpng.a -lz -o XaraLX
}


function make_tags {
    find . -name "*.c" -o -name "*.cpp" -o -name "*.h" -print | xargs etags -a
}
