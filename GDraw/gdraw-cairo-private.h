/* gdraw-cairo-private.h - An attempt to implement GDraw interface with cairo
 *
 * Copyright (C) © 2010 E. Geldhof
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
 * Author:  E. Geldhof <ed44@xs4all.nl>
 */

#ifndef GDRAW_CAIRO_PRIVATE_H
#define  GDRAW_CAIRO_PRIVATE_H

#include <cairo.h>


/*#define CAIRO_HAS_MESH_PATTERN */


#define MIN_LINE_WIDTH 0.25



/*	There was no typedef for these
*	(...and we use malloc/free anyway )
*/
typedef BYTE * (*MemoryAlloc) ( UINT32 Size );
typedef void  (*MemoryFree) ( BYTE *Address );

typedef struct _GCDATA GCDATA;

/*
*	The source to be used for drawing
*/
enum SOURCE{
	S_COLOR,
	S_PATTERN,
	S_SURFACE,
	S_ALPHA_SURFACE,
	S_ABSENT
};

/*
*	patterns
*/
enum pattern_type{
	PATTERN_LINEAR,
	PATTERN_ELLIPTICAL,
	/*	PATTERN_IF* only used for fun interfering alpha masks ( 3 and 4 point transparency )	*/
	PATTERN_IF1,
	PATTERN_IF2,
	PATTERN_IF3,
	PATTERN_IF4,
};


typedef struct _patterndata{
	double	ax;		 /*	3 transformed points	*/
	double	ay;
	double	bx;
	double	by;
	double	cx;
	double	cy;
	UINT32	style;
	UINT32	value1; /*	startcolor of pattern or first of 3 or 4 point pattern*/
	UINT32	value2; /*	endcolor of pattern or second of 3 or 4 point pattern*/
	UINT32	value3; /* third of 3 or 4 point pattern*/
	UINT32	value4; /* fourth of 4 point pattern*/
}patterndata;

typedef struct _mapdata{
	double ax; /*	3 transformed points	*/
	double ay;
	double bx;
	double by;
	double cx;
	double cy;
	INT32 width;
	INT32 height;
}mapdata;

enum PAT_SOURCE{
	PAT_COLORS,
	PAT_LIST
};

enum T_SOURCE{
	T_VALUES,
	T_LIST,
	T_ABSENT
};


/*	A datacontainer to work with	*/
struct _GCDATA{
	eError					last_error;
/*	MemoryAlloc				getbytes;
	MemoryFree				freebytes;*/
	INT32					width;		/*	of the painting surface	*/
	INT32					height;		/*	of the painting surface	*/
	INT32					depth;		/*	of the painting surface	*/
	INT32					format;		/*	of the painting surface	*/
	BYTE						*pixels;		/*	of the painting surface	*/
	RECT 					b_rect;		/*	the bounding box of the drawing done	*/
	COLORREF				color;		/*	source for painting flat color	*/
	patterndata				cpat;			/*	patterndata for color pattern source 	*/
	patterndata				tpat;			/*	patterndata for transparent pattern source 	*/
	mapdata					mdata;		/*	data for a bitmap source	*/
	UINT32					use_mask;		/*	1 if a transparency mask is needed	*/
	INT32					tpat_offset_x;	/*	offset for 	a transparency mask	*/
	INT32					tpat_offset_y;	/*	offset for 	a transparency mask	*/
	const LOGPALETTE			*logpalette;	/*	not used	*/
	INT32					transparency;	/*	flat transparency value	*/
	SOURCE					source;		/*	source to use	*/
	UINT32					patlist[256];	/*	colorlist for a color pattern		*/
	BYTE						tlist[256];		/*	transparency list for a transparency pattern	*/
	PAT_SOURCE				psource;		/*	source for a color pattern ( list or start/end values*/
	T_SOURCE				tsource;		/*	source for a transparency pattern ( list or start/end values*/
	cairo_surface_t				*surface;		/*	the bitmap source to use	*/
	cairo_t					*cr;			/*	the cairo_t to paint to	*/
	UINT32					flatness;		/*	quality value	*/
	UINT32					antialias;		/*	quality value	*/
};


/*	These are just for info
*	defined in wxOil/compatdef.h


struct PALETTEENTRY
{
	BYTE				peRed;
	BYTE				peGreen;
	BYTE				peBlue;
	BYTE				peFlags;
};

struct LOGPALETTE
{
	WORD				palVersion;
	WORD				palNumEntries;
	PALETTEENTRY		palPalEntry[1];
};

struct BITMAPINFOHEADER
{
	WORD				biSize;
	INT32				biWidth;
	INT32				biHeight;
	WORD				biPlanes;
	WORD				biBitCount;
    	DWORD				biCompression;      
	DWORD				biSizeImage;
	INT32				biXPelsPerMeter;
	INT32				biYPelsPerMeter;
	DWORD				biClrUsed;
	DWORD				biClrImportant;
};

struct POINT
{
	INT32	x;
	INT32	y;
};

*/
struct statnode{
	const char *name;
	INT32 count;
};

INT32
xcl_set_cairo_path(
	cairo_t *cr,
	const POINT  *Points,
	const BYTE  *Types,
	UINT32 Length,
	bool Close
);




#endif /*	define GDRAW_CAIRO_PRIVATE_H */


























