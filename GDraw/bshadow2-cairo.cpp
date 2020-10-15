/* gdraw-cairo.c - A lame attempt to implement GDraw interface with cairo
 *
 * Copyright © 2007 Carl Worth
 * Copyright © 2010 E. Geldhof
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the GNU C Library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * Author: Carl Worth <cworth@cworth.org>
 * E. Geldhof <ed44@xs4all.nl>
 */

#include <stdio.h>

#include "camtypes.h"
#include "bshadow2.h"
#include "gdraw-cairo-private.h"

#define STUB(func) fprintf (stderr, "FIXME: Need a cairo-based implementation of " func " here.\n")

extern struct statnode statlist[];

static void
surface_blur_8(BYTE* data, UINT32 width, UINT32 height, UINT32 stride, UINT32 size);

bool
GenerateWallShadow (
    CONST BITMAPINFOHEADER* pSourceInfo,
    CONST BYTE*             pSourceBitmap,
    CONST BITMAPINFOHEADER* pDestinationInfo,
    BYTE*             pDestinationBitmap,
    CONST UINT32   nRowOffsets,
    CONST UINT32*  pLeftOffsets,
    CONST UINT32* pRightOffsets,
    CONST UINT32 nColumnOffsets,
    CONST UINT32*   pLowOffsets,
    CONST UINT32*  pHighOffsets,
    CONST UINT32 nTranslationShift,
    CONST BYTE*  pTranslationTable
    )
{
/*	printf("GenerateWallShadow\n");*/
	statlist[128].count++;

	UINT32 i, j;
/*
	printf("SOURCE : WxH=%d x %d BITS=%d COMP=%08X\n",
		pSourceInfo->biWidth, pSourceInfo->biHeight,
		pSourceInfo->biBitCount, pSourceInfo->biCompression);

	printf("DEST    : WxH=%d x %d BITS=%d COMP=%08X\n",
		pDestinationInfo->biWidth, pDestinationInfo->biHeight,
		pDestinationInfo->biBitCount, pDestinationInfo->biCompression);

	printf(" Roffsets=%d   Coffsets=%d  Tshifts=%d\n",
		nRowOffsets, nColumnOffsets, nTranslationShift);

	for(i=0; i < nRowOffsets; i++){
		printf("LO= %d RO= %d\n", pLeftOffsets[i], pRightOffsets[i]);
	}
	for(i=0; i < nColumnOffsets; i++){
		printf("LO= %d  HO= %d\n", pLowOffsets[i], pHighOffsets[i]);
	}
	for(i=0; i < nTranslationShift; i++){
		printf("TSH=%d\n", pTranslationTable[i]);
	}
*/
	INT32 ws, wd, hs, hd, ss, sd, sstride, dstride, wdiff;
/*	cairo_surface_t *s, *d;
	cairo_t *cr;
*/
	sstride=pSourceInfo->biWidth+((pSourceInfo->biWidth % 4)==0 ? 0 :
								4 - (pSourceInfo->biWidth % 4));

	dstride=pDestinationInfo->biWidth+((pDestinationInfo->biWidth % 4)==0 ? 0 :
								4 - (pDestinationInfo->biWidth % 4));

	ws=pSourceInfo->biWidth;
	hs=pSourceInfo->biHeight;
	wd=pDestinationInfo->biWidth;
	hd=pDestinationInfo->biHeight;
/*	ss=(pSourceInfo->biWidth - pDestinationInfo->biWidth)/2;
	sd=(pSourceInfo->biHeight - pDestinationInfo->biHeight)/2;
	sd *=pSourceInfo->biWidth;
*/

	wdiff=((hs - hd)/2) * sstride;
	ss=sd=0;
/*
	printf("sstr=%d, dstr=%d, wdiff=%d\n", sstride, dstride, wdiff);
*/
	for(i=0; i < hd; i++){
		for(j=0; j < wd; j++){

			pDestinationBitmap[sd + j]=(~pSourceBitmap[ss + j + wdiff]) ;
			pDestinationBitmap[sd + j] >>=2;
		}
		ss +=sstride;
		sd +=dstride;
	}
	surface_blur_8(pDestinationBitmap, wd, hd, dstride, pTranslationTable[0]/2);
	return 0;
}

#define MAX_SHADOW_BLUR 19

static void
surface_blur_8(BYTE* data, UINT32 width, UINT32 height, UINT32 stride, UINT32 size)
{
	if(size > MAX_SHADOW_BLUR)
		size = MAX_SHADOW_BLUR;

	unsigned char/* *data,*/ val;
	uint blurwidth=2*size+1;
	uint b_min_one=blurwidth-1;
	uint /*width, height, stride, */str;
	uint i, j, t, p, ii, total, nd, c1, c2;
	uint list[MAX_SHADOW_BLUR*2+1];

/*	data=cairo_image_surface_get_data(surface);
	width=cairo_image_surface_get_width(surface);
	height=cairo_image_surface_get_height(surface);
	stride=cairo_image_surface_get_stride(surface);
*/
	for(j=0, t=0, p=0, total=0; j<height; j++, t +=stride, p=0, total=0)
	{
		for(ii=0; ii<size; ii++)
			list[ii]=0;

		for( ; ii<blurwidth; ii++)
			total +=list[ii]=(uint)data[t+ii-size];

		val=(unsigned char)((total+size)/blurwidth);

		for(i=0, c1=size+1, c2=size+1+t; i<width; i++, c1++, c2++)
		{
			data[t+i]=val;
			if(c1 >= width)
				nd=0;
			else
				nd=(uint)data[c2];

			if(nd !=list[p])
			{
				total -=list[p];
				total +=list[p]=nd;
				val=(unsigned char)((total+size)/blurwidth);
			}
			if(++p > b_min_one)
				p=0;
		}
	}
	str=(size+1)*stride;

	for(i=0, p=0, total=0; i<width; i++, p=0, total=0)
	{
		for(ii=0; ii<size; ii++)
			list[ii]=0;

		for( ; ii<blurwidth; ii++)
			total +=list[ii]=(uint)data[i+(ii-size)*stride];

		val=(unsigned char)((total+size)/blurwidth);

		for(j=0, t=0; j<height; j++, t +=stride)
		{
			data[t+i]=val;
			if(j+size+1 >= height)
				nd=0;
			else
				nd=(uint)data[t+i+str];

			if(nd !=list[p])
			{
				total -=list[p];
				total +=list[p]=nd;
				val=(unsigned char)((total+size)/blurwidth);
			}
			if(++p > b_min_one)
				p=0;
		}
	}
}


bool
GenerateFloorShadow (
    void* pCode,
    CONST BITMAPINFOHEADER* pSourceInfo,
    CONST BYTE*             pSourceBitmap,
    CONST BITMAPINFOHEADER* pDestinationInfo,
    BYTE*             pDestinationBitmap,
    CONST UINT32   nRowOffsets,
    CONST INT32*  pLeftOffsets,
    CONST INT32* pRightOffsets,
    CONST UINT32  nTranslationShift,
    CONST BYTE*  pTranslationTable
    )
{
    STUB ("GenerateFloorShadow");
	statlist[129].count++;
    return 0;
}

void
ContourBitmap ( 
    BYTE*		 pBits8, 
    const UINT32 uByteWidth8,
    BYTE*		pDBits8,
    BITMAPINFO* pDInfo8, 
    const double fContour,
    const UINT32 uContour
    )
{
    STUB ("ContourBitmap");
	statlist[130].count++;
}
