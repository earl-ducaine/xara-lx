#!/usr/bin/env bash



rm -f libCDraw_la-bshadow2-new.o

objcopy \
    -N _Z18GenerateWallShadowPK16BITMAPINFOHEADERPKhS1_PhjPKjS6_jS6_S6_jS3_ \
    libCDraw_la-bshadow2.o \
    libCDraw_la-bshadow2-new.o
	
	
