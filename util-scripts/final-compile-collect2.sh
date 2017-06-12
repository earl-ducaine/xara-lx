#!/usr/bin/env bash

# g++ -v  -no-pie -Wl,--start-group Kernel/libKernel.a wxOil/libwxOil.a tools/libTools.a wxXtra/libwxXtra.a -Wl,--end-group --debug -L./libs/x86_64  -L/usr/local/lib -pthread   -lwx_gtk2u_richtext-2.8 -lwx_gtk2u_aui-2.8 -lwx_gtk2u_xrc-2.8 -lwx_gtk2u_qa-2.8 -lwx_gtk2u_html-2.8 -lwx_gtk2u_adv-2.8 -lwx_gtk2u_core-2.8 -lwx_baseu_xml-2.8 -lwx_baseu_net-2.8 -lwx_baseu-2.8   -lxml2 -ljpeg -lCDraw -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgio-2.0 -lpangoft2-1.0 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lcairo -lpango-1.0 -lfreetype -lfontconfig -lgobject-2.0 -lglib-2.0 ./libpng-1.2.57/.libs/libpng.a -lz -o XaraLX


/usr/lib/gcc/x86_64-linux-gnu/5/collect2 \
    --verbose \
    -plugin /usr/lib/gcc/x86_64-linux-gnu/5/liblto_plugin.so \
    -plugin-opt=/usr/lib/gcc/x86_64-linux-gnu/5/lto-wrapper \
    -plugin-opt=-fresolution=/tmp/cclT5u3R.res \
    -plugin-opt=-pass-through=-lgcc_s \
    -plugin-opt=-pass-through=-lgcc \
    -plugin-opt=-pass-through=-lpthread \
    -plugin-opt=-pass-through=-lc \
    -plugin-opt=-pass-through=-lgcc_s \
    -plugin-opt=-pass-through=-lgcc \
    --sysroot=/ \
    --build-id \
    --eh-frame-hdr \
    -m elf_x86_64 \
    --hash-style=gnu \
    --as-needed \
    -dynamic-linker /lib64/ld-linux-x86-64.so.2 \
    -z relro \
    -o XaraLX \
    /usr/lib/gcc/x86_64-linux-gnu/5/../../../x86_64-linux-gnu/crt1.o \
    /usr/lib/gcc/x86_64-linux-gnu/5/../../../x86_64-linux-gnu/crti.o \
    /usr/lib/gcc/x86_64-linux-gnu/5/crtbegin.o \
    -L./libs/x86_64 \
    -L/usr/local/lib \
    -L/usr/lib/gcc/x86_64-linux-gnu/5 \
    -L/usr/lib/gcc/x86_64-linux-gnu/5/../../../x86_64-linux-gnu \
    -L/usr/lib/gcc/x86_64-linux-gnu/5/../../../../lib \
    -L/lib/x86_64-linux-gnu \
    -L/lib/../lib \
    -L/usr/lib/x86_64-linux-gnu \
    -L/usr/lib/../lib \
    -L/usr/lib/gcc/x86_64-linux-gnu/5/../../.. \
    --start-group Kernel/libKernel.a wxOil/libwxOil.a tools/libTools.a wxXtra/libwxXtra.a \
    --end-group \
    -lwx_gtk2u_richtext-2.8 \
    -lwx_gtk2u_aui-2.8 \
    -lwx_gtk2u_xrc-2.8 \
    -lwx_gtk2u_qa-2.8 \
    -lwx_gtk2u_html-2.8 \
    -lwx_gtk2u_adv-2.8 \
    -lwx_gtk2u_core-2.8 \
    -lwx_baseu_xml-2.8 \
    -lwx_baseu_net-2.8 \
    -lwx_baseu-2.8 \
    -lxml2 \
    -ljpeg \
    -lCDraw \
    -lgtk-x11-2.0 \
    -lgdk-x11-2.0 \
    -latk-1.0 \
    -lgio-2.0 \
    -lpangoft2-1.0 \
    -lpangocairo-1.0 \
    -lgdk_pixbuf-2.0 \
    -lcairo \
    -lpango-1.0 \
    -lfreetype \
    -lfontconfig \
    -lgobject-2.0 \
    -lglib-2.0 ./libpng-1.2.57/.libs/libpng.a \
    -lz \
    -lstdc++ \
    -lm \
    -lgcc_s \
    -lgcc \
    -lpthread \
    -lc \
    -lgcc_s \
    -lgcc /usr/lib/gcc/x86_64-linux-gnu/5/crtend.o \
    /usr/lib/gcc/x86_64-linux-gnu/5/../../../x86_64-linux-gnu/crtn.o
