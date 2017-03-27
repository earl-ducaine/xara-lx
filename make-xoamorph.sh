







function build_wx {
    cd ~/dev/xara/wxGTK-2.8.12

    if [ -d buildgtk ]; then
	cd buildgtk
	sudo make uninstall
	cd ..
	rm -rf buildgtk
	mkdir buildgtk
    fi
    cd buildgtk
    ../configure --with-gtk --enable-unicode
    make -j 8
    sudo make install
    sudo ldconfig
}


cd xoamorph
./autogen.sh
./configure
make
