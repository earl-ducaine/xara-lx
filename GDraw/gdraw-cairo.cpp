/* gdraw-cairo.c - An attempt to implement GDraw interface with cairo
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
 *
 *
 */


 /* NOTES 
 * 1 : new functionnames are prefixed with " xcl_ " (xara-cairo-link)
 *
 * 2 : Colors are in bgra format. Channels are switched at "cairo_set_source,,,,"
 *
 * 3 : Alpha values need to be inverted before use
 *
 *
 *
 */

#include <stdio.h>

#include "camtypes.h"
#include "gdraw.h"
#include "GVersion.h"
#include "gdraw-cairo-private.h"

#define XCL_DEBUG(x)
#define STUB(func)
/*#define STUB(func) fprintf (stderr, "FIXME: Need a cairo-based implementation of " func " here.\n");fflush(stderr)*/
/*#define XCL_DEBUG(x) fprintf(stderr, x);fflush(stderr)*/

	/*	max. 24 contexts seems enough; normally 4 contexts are in use */
#define DP_LIST_LENGTH 24
#define BITMAP_CACHE_POSITIONS 100

	/*
	*	 globals
	*/

GCDATA *dp_list[DP_LIST_LENGTH]; /*	list of pointers to contextdata structs */
INT32 dp_list_initialised=0;
INT32 npositions_used=0;
INT32 ncairo_types=0;
INT32 ncontext=0;

#include "statlist.cpp" /*	...where the "statlist" struct is declared	*/

	/*
	*	prototypes
	*/
static INT32
xcl_fill_with_color_and_transparent_pattern(GCDATA *data);

static cairo_pattern_t *
xcl_create_cairo_pattern(patterndata *pinfo, INT32 offset_x, INT32 offset_y, pattern_type type);

static INT32
xcl_fill_with_color(GCDATA *data, cairo_surface_t *mask);

static INT32
xcl_fill_with_pattern(GCDATA *data, cairo_surface_t *mask);

static INT32
xcl_fill_with_surface(GCDATA *data, cairo_surface_t *mask);

static cairo_surface_t *
xcl_create_alpha_mask(GCDATA *data, INT32 x1, INT32 y1, INT32 x2, INT32 y2);

static INT32
xcl_paint_diamond_alpha_mask( GCDATA *data, cairo_t *cr);

static INT32
xcl_paint_pattern_alpha_mask(GCDATA *data, cairo_t *cr);

static INT32
xcl_paint_interfering_alpha_mask(GCDATA *data, cairo_t *cr);

static INT32
xcl_add_to_bbox(GCDATA *data, double x1, double y1, double x2, double y2);

static INT32
xcl_ensure_cr(GCDATA *data);

static INT32
xcl_clear_bounding_box(GCDATA *data);

static cairo_line_cap_t
xcl_xara_to_cairo_line_cap(UINT32 cap);

static cairo_line_join_t
xcl_xara_to_cairo_line_join(UINT32 join);

static cairo_pattern_t *
xcl_create_diamond_color_pattern(GCDATA *data);

static INT32
xcl_fill_with_diamond_color_pattern(GCDATA *data, cairo_surface_t *mask);

#ifdef CAIRO_HAS_MESH_PATTERN 

static INT32
xcl_fill_with_mesh_color_pattern(GCDATA *data, cairo_surface_t *mask);

static cairo_pattern_t *
xcl_create_mesh_color_pattern(GCDATA *data);

#endif

/*
*	Cache for bitmaps (not used : things go wrong when exporting png/jpg )
*/

struct _bm_cache{
	const BYTE *start;
	cairo_surface_t *surface;
};

struct _bm_cache bm_cache[BITMAP_CACHE_POSITIONS];

UINT32 mapcount=0;

static INT32
xcl_add_to_bm_cache(const BYTE *start, cairo_surface_t *surface)
{
	if(mapcount > BITMAP_CACHE_POSITIONS-1)
		return 1;

	bm_cache[mapcount].start=start;
	bm_cache[mapcount].surface=surface;
	mapcount++;
	printf("BM CACHE : now %d entries\n", mapcount);
	return 0;
}

static cairo_surface_t *
xcl_check_bm_cache(const BYTE *start)
{
	UINT32 c=0;

	while(c < mapcount)
	{
		if(bm_cache[c].start==start)
			return bm_cache[c].surface;

		c++;
	}
	return NULL;
}

static INT32
xcl_clean_bm_cache()
{
	UINT32 c=0;

	while(c < mapcount)
	{
		printf("Removing surface with %d references\n",
			cairo_surface_get_reference_count(bm_cache[c].surface));
		cairo_surface_destroy(bm_cache[c].surface);
		c++;
	}
	return 0;
}
/*
*	End cache
*/


static INT32
xcl_init()
{
	INT32 pos;

	for(pos=0; pos < DP_LIST_LENGTH; pos++)
	{
		dp_list[pos]=NULL;
	}
	dp_list_initialised=1;

	for(pos=0; pos < BITMAP_CACHE_POSITIONS; pos++)
	{
		bm_cache[pos].start=NULL;
		bm_cache[pos].surface=NULL;
		mapcount=0;
	}
	return 0;
}

/*
*	to calculate cairo doubles from xara fixed point coords and vv.
*/
static inline double
toFL(INT32 x)
{
	return ((double)x)/(1<<16);
}

static inline INT32
toINT(double x)
{
	return (INT32)(x*(1 << 16));
}

/*
static inline double
toFL(INT32 x)
{
	return (double)x;
}

static inline INT32
toINT(double x)
{
	return (INT32)x;
}
*/

DWORD
GDraw_GetVersion (void)
{
    return GDRAW_VERSION_MAJOR << 16 | GDRAW_VERSION_MINOR;
}    

#if !defined(__WXMSW__)
pcCHAR
GDraw_GetSvnVersion (void)
{
    STUB ("GDraw_GetSvnVersion");
	statlist[0].count++;
    return 0;
}
#endif

INT32
GDraw_ClearLastError (pGCONTEXT pContext)
{
    STUB ("GDraw_ClearLastError");

	statlist[1].count++;

    return 0;
}

eError
GDraw_GetLastError (pcGCONTEXT pContext)
{
    STUB ("GDraw_GetLastError");
	statlist[2].count++;
    return GERROR_NO_ERROR;
}

pcCHAR
GDraw_GetLastErrorText (pcGCONTEXT pContext)
{
    STUB ("GDraw_GetLastErrorText");
	statlist[3].count++;
    return 0;
}

/*
*	Initialise a xara context
*	- malloc an xcl datastruct ( GCDATA ) and keep a pointer in dp_list
*	- put a reference in pContext->Data[0]
*	- fill the datastruct with defaults 
*/
INT32
GDraw_Initialise (pGCONTEXT pContext, pcGCONTEXT pOldContext)
{
	statlist[4].count++;
	XCL_DEBUG("GDraw_Initialise\n");

	INT32	pos;
	GCDATA	*data;

	if(!dp_list_initialised)
		xcl_init();

	if(pContext->Valid !=0xC90FDAA0
	&& pContext->Valid !=0xC90FDAA2)
	{
		for(pos=0; pos < DP_LIST_LENGTH; pos++)
		{
			if(dp_list[pos]==NULL)
			{
				npositions_used++;
				break;
			}
		}
		if(pos==DP_LIST_LENGTH)
			return 1;
		else
			pContext->Data[0]=(DWORD)pos;

		data=(GCDATA *)malloc(sizeof(*data));
		if(data==NULL)
			return 1;

		dp_list[pos]=data;
	}
	else
	{
/*		if(pContext->Data[0] < 0
		||  pContext->Data[0] >=DP_LIST_LENGTH)
*/
		/*
		*	This happens sometimes and it spoils my context counters ( ncontext/ncairo_types ) 
		*	So it's just cleaned up at the risc of crashing. ( TODO fixit )
		*/
		printf("GDraw_Initialise called with an already initialised context\n");
		fflush(stdout);/*	...as we might crash very soon		*/

		data=dp_list[pContext->Data[0]];

		if(data->cr)
		{
			cairo_surface_destroy(cairo_get_target(data->cr));
			cairo_destroy(data->cr);
			data->cr=NULL;
			ncairo_types--;
		}
		if(data->surface){
			cairo_surface_destroy(data->surface);
		}
		ncontext--;
	}
	data->last_error=GERROR_NO_ERROR;

	data->pixels=NULL;

	data->cr=NULL;
	data->surface=NULL;

	data->use_mask=0;
	data->logpalette=NULL;

	pContext->Valid=0xC90FDAA0;

	data->antialias=0;
	data->flatness=0;

	ncontext++;

	printf(" Now %d contexts in use.\n", ncontext);
	return 0;
}

/*	This one is never called afaik	*/
INT32
GDraw_CopyContext (pcGCONTEXT pContext,pGCONTEXT pNewContext)
{
	XCL_DEBUG("GDraw_CopyContext\n");
	statlist[5].count++;

	return 0; 
}


INT32
GColour_GetGraduationTableSize (pcGCONTEXT pContext)
{
	statlist[6].count++;
	return 4;/*	size in bytes	*/
}

INT32
GColour_GetGraduationTableLength (void)
{
	statlist[7].count++;
	return 0x100;
}

INT32
GColour_GetLongGraduationTableSize (pcGCONTEXT pContext)
{
	statlist[8].count++;
	return 4;/*	size in bytes	*/
}

INT32
GColour_GetLongGraduationTableLength (void)
{
	statlist[9].count++;
	return 0x100;
}

INT32
GColour_GetTransparentGraduationTableSize (void)
{
	statlist[10].count++;
	return 1;/*	size in bytes	*/
}

INT32
GColour_GetTransparentGraduationTableLength (void)
{
	statlist[11].count++;
	return 0x100;
}

INT32
GColour_GetLongTransparentGraduationTableSize (void)
{
	statlist[12].count++;
	return 1;/*	size in bytes	*/
}

INT32
GColour_GetLongTransparentGraduationTableLength (void)
{
	statlist[13].count++;
	return 0x100;
}

INT32
GDraw_SetMemoryHandlers (
	pGCONTEXT pContext,
	MemoryAlloc bytes_alloc,
	MemoryFree bytes_free
    )
{
	statlist[14].count++;
	/*
	*	We use malloc/free
	*/
/*

	GCDATA *data=dp_list[pContext->Data[0]];
	if(data==NULL)
		return 0;

	data->getbytes=bytes_alloc;
	data->freebytes=bytes_free;
*/
	return 0;
}

INT32
GDraw_SetStackLimit ( pGCONTEXT pContext,pcVOID StackLimit )
{
    STUB ("GDraw_SetStackLimit");
	statlist[15].count++;
    return 0;
}

INT32
GDraw_SetStackSize ( pGCONTEXT pContext,UINT32 StackSize )
{
    STUB ("GDraw_SetStackSize");
	statlist[16].count++;
    return 0;
}

INT32
GDraw_Terminate (pGCONTEXT pContext)
{
	XCL_DEBUG("GDraw_Terminate\n");
	statlist[17].count++;

	INT32	pos;
	GCDATA	*data;
	UINT32	c;

	pos=pContext->Data[0];
	data=dp_list[pos];

	if(data==NULL){
		XCL_DEBUG("XCL : Empty or invalid context passed to GDraw_Terminate");
		pContext->Valid=0;
		return 0;
	}

	if(data->cr)
	{
		cairo_surface_destroy(cairo_get_target(data->cr));
		cairo_destroy(data->cr);
		data->cr=NULL;
		ncairo_types--;
	}
	if(data->surface){
		cairo_surface_destroy(data->surface);
	}
	free((BYTE *)data);
	dp_list[pos]=NULL;

	printf("XCL : Now %d cairo_t objects in use.\n", ncairo_types);
	ncontext--;
	npositions_used--;

	pContext->Valid=0;

	if(ncontext==0)/*	Program ends : print a list of used functions to stdout	*/
	{
		for(c=0; c < MAX_STATNODE; c++){
			if(statlist[c].count > 0)
				printf("    %10d times called  %s\n",
					statlist[c].count, statlist[c].name);
		}
		xcl_clean_bm_cache();
		printf("\n %d maps were in bm cache\n", mapcount);
	}
	return 0;
}

INT32
GDraw_Clean (pGCONTEXT pContext)
{
	STUB("GDraw_Clean\n");
	statlist[18].count++;
    return 0;
}


INT32
GDraw_ContextLength (void)
{
	return sizeof (GCONTEXT);
}

INT32
GDraw_ComparePalettes ( pcGCONTEXT pContext,pcLOGPALETTE Palette,bool Flag )
{
	XCL_DEBUG("GDraw_ComparePalettes\n");
	statlist[19].count++;

/*	printf("Flag = %d\n", Flag);*/
	GCDATA *data=dp_list[pContext->Data[0]];

	if(Palette==data->logpalette)/*  probably  wrong	*/
		return 1;
	else
		return 0;
}

/*	Returns a 256 colour logical palette that should be used in 8-bit modes.  	*/
pcLOGPALETTE
GColour_SelectPalette ( pGCONTEXT pContext,bool Flag )
{
	XCL_DEBUG("GColour_SelectPalette\n");
	statlist[20].count++;

/*	printf("Flag = %d\n", Flag);*/

	GCDATA *data=dp_list[pContext->Data[0]];

	return data->logpalette;
}


INT32
GColour_InitialiseWithPalette ( pGCONTEXT pContext,pcLOGPALETTE Palette )
{
	XCL_DEBUG("GColour_InitialiseWithPalette");
	statlist[21].count++;
	GCDATA *data;

	if(GDraw_Initialise(pContext, NULL))
			return 1;

	data=dp_list[pContext->Data[0]];
	data->logpalette=Palette;

	return 0;
}

INT32
GColour_SetDitherStyle ( pGCONTEXT pContext, DitherStyle Style)
{
  /*  STUB ("GColour_SetDitherStyle");*/
	statlist[22].count++;
    return 0;
}

INT32
GColour_SetTileSmoothingFlag ( pGCONTEXT pContext,bool Flag )
{
  /*  STUB ("GColour_SetTileSmoothingFlag");*/
	statlist[23].count++;
	return 0;
}

INT32
GColour_SetTileFilteringFlag ( pGCONTEXT pContext,bool Flag )
{
  /*  STUB ("GColour_SetTileFilteringFlag");*/
	statlist[24].count++;
	return 0;
}

INT32
GBitmap_SetMaxFilterSize ( pGCONTEXT pContext, UINT32 Size )
{
  /*  STUB ("GBitmap_SetMaxFilterSize");*/
	statlist[25].count++;
	return 0;
}

INT32
GBitmap_Sharpen ( pGCONTEXT pContext,INT32 Sharpen )
{
    STUB ("GBitmap_Sharpen");
	statlist[26].count++;
    return 0;
}

INT32
GBitmap_Blur ( pGCONTEXT pContext,INT32 Blur    )
{
    STUB ("GBitmap_Blur");
	statlist[27].count++;
    return 0;
}

/*	Sets the origin of the dither patterns. 	*/
INT32
GColour_SetHalftoneOrigin ( pGCONTEXT pContext, INT32 x,INT32 y )
{
    /*STUB ("GColour_SetHalftoneOrigin");*/
	statlist[28].count++;
    return 0;
}

/*	Sets drawing colour to be used in subsequent rendering. 	*/
INT32
GColour_SetColour ( pGCONTEXT pContext,COLORREF Colour )
{
/*	XCL_DEBUG("GColour_SetColour\n");*/
	statlist[29].count++;

	GCDATA *data=dp_list[pContext->Data[0]];

	data->color=(Colour & 0xFFFFFF) | (~Colour & 0xFF000000);
	data->transparency=0xFF;
	data->source=S_COLOR;
	return 0;
}

/*	Sets drawing colour to be used in subsequent rendering. 	*/
INT32
GColour_SetSolidColour ( pGCONTEXT pContext,COLORREF Colour,UINT32 BPP,UINT32 Format16BPP)
{
	XCL_DEBUG ("GColour_SetSolidColour\n");
	statlist[30].count++;

	GCDATA *data=dp_list[pContext->Data[0]];

	data->color=(Colour & 0xFFFFFF) | 0xFF000000;
	data->transparency=0xFF;
	data->source=S_COLOR;
	return 0;
}

/*	Sets drawing colour to be used in subsequent rendering. 	*/
INT32
GColour_SetWordColour ( pGCONTEXT pContext,DWORD Colour )
{
	statlist[31].count++;
	XCL_DEBUG("GColour_SetWordColour");

	GCDATA *data=dp_list[pContext->Data[0]];

	data->color=(Colour & 0xFFFFFF) | (~Colour &0xFF000000);
	data->transparency=0xFF;
	return 0;
}

INT32
GColour_SetInvert ( pGCONTEXT pContext,DWORD Colour )
{
	statlist[32].count++;

    STUB ("GColour_SetInvert");
    return 0;
}

/*	Sets drawing colour to be used in subsequent rendering with transparency.
*	Only has effect if output device can do transparency, else falls into SetColour. 
*/
INT32
GColour_SetTransparency ( pGCONTEXT pContext,COLORREF Colour,DWORD Style )
{
	XCL_DEBUG("GColour_SetTransparency\n");
	statlist[33].count++;

	GCDATA *data=dp_list[pContext->Data[0]];

	data->color=(Colour & 0xFFFFFF) | (~Colour &0xFF000000);
	data->transparency=(~Style & 0x0000FF00) >> 8;

	/*
	*	If a shadow surface is the source, this is the color of the shadow
	*	data->source should not be set to S_COLOR in that case
	*/
	if(data->source !=S_ALPHA_SURFACE)
		data->source=S_COLOR;

	return 0;
}

INT32
GColour_SetTransparencyLookupTable ( pGCONTEXT pContext,pcBYTE Table )
{
    STUB ("GColour_SetTransparencyLookupTable");
	statlist[34].count++;

	printf("SetTransparencyLookupTable\n");
    return 0;
}

pcBYTE
GColour_ReturnBrush (pcGCONTEXT pContext)
{
   /* STUB ("GColour_ReturnBrush");*/
	statlist[35].count++;
    return 0;
}

pcBYTE
GColour_ReturnBrushRGB (pcGCONTEXT pContext)
{
/*	XCL_DEBUG ("GColour_ReturnBrushRGB\n");

	GCDATA *data=dp_list[pContext->Data[0]];*/

	/*
	*	Not used i guess....
	*/
	statlist[36].count++;
	return 0;
}

/*	Adds a colour to a grad table at the specified point. 	*/
INT32
GColour_AddToGraduationTable (
    pGCONTEXT pContext,
    COLORREF Colour,
    DWORD HSVFlag,
    pGraduationTable Table,
    UINT32 Index
    )
{
	statlist[37].count++;

	GCDATA *data=dp_list[pContext->Data[0]];

	data->patlist[Index]=Colour;

	data->psource=PAT_LIST;

    return 0;
}

/*	Gets table ready for grad fills. 	*/
INT32
GColour_BuildGraduationTable (
    pGCONTEXT pContext,
    COLORREF StartColour,
    COLORREF EndColour,
    DWORD HSVFlag,
    pGraduationTable Table
    )
{
	XCL_DEBUG("GColour_BuildGraduationTable\n");
	statlist[38].count++;
/*
	printf("BuildGraduationTable\n");
	printf("Start COLOR = %d, %d, %d, %d    ",
		(StartColour & 0xFF000000) >> 24,
		StartColour & 0xFF,
		(StartColour & 0xFF00) >>8,
		(StartColour & 0xFF0000) >> 16);

	printf("End COLOR = %d, %d, %d, %d    ",
		(EndColour & 0xFF000000) >> 24,
		EndColour & 0xFF,
		(EndColour & 0xFF00) >>8,
		(EndColour & 0xFF0000) >> 16);

	printf( " HSVFlag = %d\n", HSVFlag);
*/
	GCDATA *data=dp_list[pContext->Data[0]];

	data->cpat.value1=StartColour;
	data->cpat.value2=EndColour;

	data->psource=PAT_COLORS;

	return 0;
}

INT32
GColour_BuildGraduationTable32 (
    pGCONTEXT pContext,
    COLORREF StartColour,
    COLORREF EndColour,
    DWORD HSVFlag,
    pGraduationTable32 Table
    )
{
	XCL_DEBUG("GColour_BuildGraduationTable32");
	statlist[39].count++;

	printf("Start COLOR = %d, %d, %d, %d    ",
		(StartColour & 0xFF000000) >> 24,
		StartColour & 0xFF,
		(StartColour & 0xFF00) >>8,
		(StartColour & 0xFF0000) >> 16);

	printf("End COLOR = %d, %d, %d, %d    ",
		(EndColour & 0xFF000000) >> 24,
		EndColour & 0xFF,
		(EndColour & 0xFF00) >>8,
		(EndColour & 0xFF0000) >> 16);

	printf( " HSVFlag = %d\n", HSVFlag);



    return 0;
}

/*	Gets table ready for transparent grad fills. 	*/
INT32
GColour_BuildTransparencyTable (
    pGCONTEXT pContext,
    BYTE StartColour,
    BYTE EndColour,
    pTransparentGradTable Table
    )
{
	XCL_DEBUG("GColour_BuildTransparencyTable\n");
	statlist[40].count++;
/*
	printf("Start COLOR = %02X    ", StartColour);

	printf("End COLOR = %02X\n", EndColour);
*/

	GCDATA *data=dp_list[pContext->Data[0]];

	data->tpat.value1=(UINT32)StartColour;
	data->tpat.value2=(UINT32)EndColour;
	data->tsource=T_VALUES;

	return 0;
}


	/*
	*	Decoding the "Style" parameter.
	*		(Style & 0xF) 	== 0   	--> linear pattern
	*					== 1	--> elliptical pattern
	*					== 3	--> diamond pattern
	*								colors are in data->cpat.value1
	*									and data->cpat.value2
	*
	*				style & 0x80	--> repeating pattern
	*	
	*    (~Style & 0x00FF0000) >> 16	--> Transparency
	*/

INT32
GColour_SetGraduation (
    pGCONTEXT pContext,
    DWORD Style,
    pcGraduationTable Table,
    pcPOINT PointA,
    pcPOINT PointB,
    pcPOINT PointC
    )
{
	statlist[41].count++;
	XCL_DEBUG("GColour_SetGraduation\n");
	double x, y;
	GCDATA *data=dp_list[pContext->Data[0]];
/*
	printf("GColour_SetGraduation\n");*/
/*	printf("Style=%X\n", Style);*/

	x=toFL(PointA->x);
	y=toFL(PointA->y);
	cairo_user_to_device(data->cr, &x, &y);
	data->cpat.ax=x;
	data->cpat.ay=y;

	x=toFL(PointB->x);
	y=toFL(PointB->y);
	cairo_user_to_device(data->cr, &x, &y);
	data->cpat.bx=x;
	data->cpat.by=y;

	x=toFL(PointC->x);
	y=toFL(PointC->y);
	cairo_user_to_device(data->cr, &x, &y);
	data->cpat.cx=x;
	data->cpat.cy=y;

	data->cpat.style=Style;
	data->transparency=(~Style & 0x00FF0000) >> 16;

	data->source=S_PATTERN;

	return 0;
}


INT32
GColour_SetGraduation4 (
    pGCONTEXT pContext,
    DWORD Style,
    pcGraduationTable Table,
    pcPOINT PointA,
    pcPOINT PointB,
    pcPOINT PointC,
    pcPOINT PointD
    )
{
    STUB ("GColour_SetGraduation4");
	statlist[42].count++;
    return 0;
}

INT32
GColour_Set3WayGraduation (
    pGCONTEXT pContext,
    DWORD Style,
    COLORREF ColourA, COLORREF ColourB, COLORREF ColourD,
    pcPOINT   PointA, pcPOINT   PointB, pcPOINT   PointD
    )
{
 /*   STUB ("GColour_Set3WayGraduation");*/
	statlist[43].count++;
#ifdef CAIRO_HAS_MESH_PATTERN 
	GCDATA *data=dp_list[pContext->Data[0]];
	double x, y;
/*
	printf("GColour_Set3WayGraduation\n");
	printf("Style=%X\n", Style);
*/
	x=toFL(PointA->x);
	y=toFL(PointA->y);
	cairo_user_to_device(data->cr, &x, &y);
	data->cpat.ax=x;
	data->cpat.ay=y;
/*	printf("Point A : %f   %f\n", x, y);*/

	x=toFL(PointB->x);
	y=toFL(PointB->y);
	cairo_user_to_device(data->cr, &x, &y);
	data->cpat.bx=x;
	data->cpat.by=y;
/*	printf("Point B : %f   %f\n", x, y);*/

	x=toFL(PointD->x);
	y=toFL(PointD->y);
	cairo_user_to_device(data->cr, &x, &y);
	data->cpat.cx=x;
	data->cpat.cy=y;
/*	printf("Point C : %f   %f\n", x, y);*/

	data->cpat.style=Style;
	data->cpat.style |=0x33000000;
	data->transparency=(~Style & 0x00FF0000) >> 16;

	data->cpat.value1=ColourA;
	data->cpat.value2=ColourB;
	data->cpat.value3=ColourD;

	data->source=S_PATTERN;

#endif
	return 0;
}

INT32
GColour_Set3WayGraduation4 (
    pGCONTEXT pContext,
    DWORD Style,
    COLORREF ColourA, COLORREF ColourB,					  COLORREF ColourD,
    pcPOINT   PointA, pcPOINT   PointB, pcPOINT   PointC, pcPOINT   PointD
    )
{
    STUB ("GColour_Set3WayGraduation4");
	statlist[44].count++;
    return 0;
}

INT32
GColour_Set4WayGraduation (
    pGCONTEXT pContext,
    DWORD Style,
    COLORREF ColourA, COLORREF ColourB, COLORREF ColourC, COLORREF ColourD,
    pcPOINT   PointA, pcPOINT   PointB,					  pcPOINT   PointD
    )
{
    STUB ("GColour_Set4WayGraduation");
	statlist[45].count++;

#ifdef CAIRO_HAS_MESH_PATTERN 
	GCDATA *data=dp_list[pContext->Data[0]];
	double x, y;
/*
	printf("GColour_Set4WayGraduation\n");
	printf("Style=%X\n", Style);
*/

	x=toFL(PointA->x);
	y=toFL(PointA->y);
	cairo_user_to_device(data->cr, &x, &y);
	data->cpat.ax=x;
	data->cpat.ay=y;

	x=toFL(PointB->x);
	y=toFL(PointB->y);
	cairo_user_to_device(data->cr, &x, &y);
	data->cpat.bx=x;
	data->cpat.by=y;

	x=toFL(PointD->x);
	y=toFL(PointD->y);
	cairo_user_to_device(data->cr, &x, &y);
	data->cpat.cx=x;
	data->cpat.cy=y;

	data->cpat.style=Style;
	data->cpat.style |=0x44000000;
	data->transparency=(~Style & 0x00FF0000) >> 16;

	data->cpat.value1=ColourA;
	data->cpat.value2=ColourB;
	data->cpat.value3=ColourC;
	data->cpat.value4=ColourD;

	data->source=S_PATTERN;

#endif

	return 0;
}

INT32
GColour_Set4WayGraduation4 (
    pGCONTEXT pContext,
    DWORD Style,
    COLORREF ColourA, COLORREF ColourB, COLORREF ColourC, COLORREF ColourD,
    pcPOINT   PointA, pcPOINT   PointB, pcPOINT   PointC, pcPOINT   PointD
    )
{
    STUB ("GColour_Set4WayGraduation4");
	statlist[46].count++;
    return 0;
}

INT32
GColour_SetGourand (
    pGCONTEXT pContext,
    DWORD Style,
    COLORREF ColourA, COLORREF ColourB, COLORREF ColourD,
    pcPOINT   PointA, pcPOINT   PointB, pcPOINT   PointD
    )
{
    STUB ("GColour_SetGourand");
	statlist[47].count++;
    return 0;
}


INT32
GColour_SetTransparentGraduation (
    pGCONTEXT pContext,
    DWORD Style,
    pcTransparentGradTable Table,
    pcPOINT PointA,
    pcPOINT PointB,
    pcPOINT PointC
    )
{
	XCL_DEBUG("GColour_SetTransparentGraduation\n");
	statlist[48].count++;

	double x, y;
	GCDATA *data=dp_list[pContext->Data[0]];

	x=toFL(PointA->x);
	y=toFL(PointA->y);
	cairo_user_to_device(data->cr, &x, &y);
	data->tpat.ax=x;
	data->tpat.ay=y;

	x=toFL(PointB->x);
	y=toFL(PointB->y);
	cairo_user_to_device(data->cr, &x, &y);
	data->tpat.bx=x;
	data->tpat.by=y;

	x=toFL(PointC->x);
	y=toFL(PointC->y);
	cairo_user_to_device(data->cr, &x, &y);
	data->tpat.cx=x;
	data->tpat.cy=y;

	data->tpat.style=Style;

	data->tsource=T_VALUES;
	/*
	*	I.c. of a repeating pattern, i can't find where the endvalue is given.
	*	The table itself is not filled by "BuildTransparencyTable" and contains
	*	( so it seems ) no useable information. ( except for Table[0] )
	*
	*	Made a change to wxOil/grndrgn.cpp. Now "BuildTransparencyTable" is allways
	*	called so we have a start and end value.
	*	A bug is in wxOil/grndrgn.cpp or Kernel/gradtbl.cpp : crash when setting a profile
	*	on a transparent grad.
	*	gdb sais a non existing table is freed
	*/

	data->use_mask=1;
	return 0;
}


INT32
GColour_SetTransparentGraduation4 (
    pGCONTEXT pContext,
    DWORD Style,
    pcTransparentGradTable Table,
    pcPOINT PointA,
    pcPOINT PointB,
    pcPOINT PointC,
    pcPOINT PointD
    )
{
    STUB ("GColour_SetTransparentGraduation4");
	statlist[49].count++;
    return 0;
}

INT32
GColour_SetTransparent3WayGraduation (
    pGCONTEXT pContext,
    DWORD Style,
    BYTE	ValueA, BYTE	ValueB, BYTE	ValueD,
    pcPOINT PointA, pcPOINT PointB, pcPOINT PointD
    )
{
    XCL_DEBUG("GColour_SetTransparent3WayGraduation\n");
	statlist[50].count++;
/*
	printf("GColour_SetTransparent3WayGraduation\n");

	printf("style = %8X\n", Style);
*/
	double x, y;
	GCDATA *data=dp_list[pContext->Data[0]];

	x=toFL(PointA->x);
	y=toFL(PointA->y);
	cairo_user_to_device(data->cr, &x, &y);
	data->tpat.ax=x;
	data->tpat.ay=y;

	x=toFL(PointB->x);
	y=toFL(PointB->y);
	cairo_user_to_device(data->cr, &x, &y);
	data->tpat.bx=x;
	data->tpat.by=y;

	x=toFL(PointD->x);
	y=toFL(PointD->y);
	cairo_user_to_device(data->cr, &x, &y);
	data->tpat.cx=x;
	data->tpat.cy=y;

	data->tpat.style=Style | 0x33000000;

	data->tpat.value1=(UINT32)ValueA;
	data->tpat.value2=(UINT32)ValueB;
	data->tpat.value3=(UINT32)ValueD;

	data->tsource=T_VALUES;
	data->use_mask=1;
	return 0;
}

INT32
GColour_SetTransparent3WayGraduation4 (
    pGCONTEXT pContext,
    DWORD Style,
    BYTE	ValueA, BYTE	ValueB,				    BYTE	ValueD,
    pcPOINT PointA, pcPOINT PointB, pcPOINT PointC, pcPOINT PointD
    )
{
    STUB ("GColour_SetTransparent3WayGraduation4");
	statlist[51].count++;


    return 0;
}

INT32
GColour_SetTransparent4WayGraduation (
    pGCONTEXT pContext,
    DWORD Style,
    BYTE	ValueA, BYTE	ValueB, BYTE	ValueC, BYTE	ValueD,
    pcPOINT PointA, pcPOINT PointB,					pcPOINT PointD
    )
{
    XCL_DEBUG("GColour_SetTransparent4WayGraduation\n");
	statlist[52].count++;
/*	printf("GColour_SetTransparent4WayGraduation\n");
	printf("style = %8X\n", Style);
*/
	double x, y;
	GCDATA *data=dp_list[pContext->Data[0]];

	x=toFL(PointA->x);
	y=toFL(PointA->y);
	cairo_user_to_device(data->cr, &x, &y);
	data->tpat.ax=x;
	data->tpat.ay=y;

	x=toFL(PointB->x);
	y=toFL(PointB->y);
	cairo_user_to_device(data->cr, &x, &y);
	data->tpat.bx=x;
	data->tpat.by=y;

	x=toFL(PointD->x);
	y=toFL(PointD->y);
	cairo_user_to_device(data->cr, &x, &y);
	data->tpat.cx=x;
	data->tpat.cy=y;

	data->tpat.style=Style | 0x44000000;

	data->tpat.value1=(UINT32)ValueA;
	data->tpat.value2=(UINT32)ValueB;
	data->tpat.value3=(UINT32)ValueC;
	data->tpat.value4=(UINT32)ValueD;

	data->tsource=T_VALUES;
	data->use_mask=1;
	return 0;
}

INT32
GColour_SetTransparent4WayGraduation4 (
    pGCONTEXT pContext,
    DWORD Style,
    BYTE	ValueA, BYTE	ValueB, BYTE	ValueC, BYTE	ValueD,
    pcPOINT PointA, pcPOINT PointB, pcPOINT PointC, pcPOINT PointD
    )
{
    STUB ("GColour_SetTransparent4WayGraduation4");
	statlist[53].count++;
    return 0;
}

INT32
GBitmap_SetBrightness (pGCONTEXT pContext,double fBrightness	)
{
    STUB ("GBitmap_SetBrightness");
	statlist[54].count++;
    return 0;
}

INT32
GBitmap_SetContrast (pGCONTEXT pContext,double fContrast	)
{
    STUB ("GBitmap_SetContrast");
	statlist[55].count++;
    return 0;
}

INT32
GBitmap_SetGamma (pGCONTEXT pContext,double fGamma		)
{
    STUB ("GBitmap_SetGamma");
	statlist[56].count++;
    return 0;
}

INT32
GBitmap_SetPostGamma (pGCONTEXT pContext,double fPostGamma	)
{
    STUB ("GBitmap_SetPostGamma");
	statlist[57].count++;
    return 0;
}

INT32
GBitmap_SetSaturation (pGCONTEXT pContext,double fSaturation	)
{
    STUB ("GBitmap_SetSaturation");
	statlist[58].count++;
    return 0;
}

INT32
GBitmap_SetContone ( 
    pGCONTEXT pContext, 
    UINT32 uContoneStyle, 
    COLORREF rgbStart,
    COLORREF rgbEnd
    )
{
    STUB ("GBitmap_SetContone");
	statlist[59].count++;
    return 0;
}

INT32
GBitmap_SetBias (pGCONTEXT pContext,UINT32 uChannel,double fBias)
{
    STUB ("GBitmap_SetBias");
	statlist[60].count++;
    return 0;
}

INT32
GBitmap_SetGain(pGCONTEXT pContext,UINT32 uChannel,double fGain)
{
    STUB ("GBitmap_SetGain");
	statlist[61].count++;
    return 0;
}

INT32
GBitmap_SetInputRange (pGCONTEXT pContext,UINT32 uChannel,BYTE uStart,BYTE uEnd)
{
    STUB ("GBitmap_SetInputRange");
	statlist[62].count++;
    return 0;
}

INT32
GBitmap_SetOutputRange (pGCONTEXT pContext,UINT32 uChannel,BYTE uStart,BYTE uEnd)
{
    STUB ("GBitmap_SetOutputRange");
	statlist[63].count++;
    return 0;
}

INT32
GColour_SetTilePattern (
    pGCONTEXT pContext,
    pcBITMAPINFOHEADER BitmapInfo,
    pcBYTE Bitmap,
    DWORD Style,
    pcPOINT PointA,
    pcPOINT PointB,
    pcPOINT PointC,
    COLORREF DefaultColour,
    pcBGRT TranslationTable,
    pcBYTE   RedTranslationTable,
    pcBYTE GreenTranslationTable,
    pcBYTE  BlueTranslationTable,
    pcBYTE TransparencyTranslationTable,
    INT32 TileOffset
    )
{
	statlist[64].count++;

/*	printf("SetTilePattern\n");*/

/*	printf("SRC = %d x %d pixels: depth=%d \n",
		BitmapInfo->biWidth, BitmapInfo->biHeight,
		BitmapInfo->biBitCount);
*/
/*	printf("TileOffset = %d\n", TileOffset);*/

//	if(BitmapInfo->biBitCount !=24
//	&& BitmapInfo->biBitCount !=32)
//		return 0;

	INT32 width, height;
	GCDATA *data=dp_list[pContext->Data[0]];
	double x, y;


	UINT32 cd, cs, t;
	UINT32 *dd, *ss;

/*	printf("Style=%08X\n", Style);*/

	data->transparency=(~Style & 0x00FF0000) >> 16;

	data->mdata.width=BitmapInfo->biWidth;
	data->mdata.height=BitmapInfo->biHeight;

	x=toFL(PointA->x);
	y=toFL(PointA->y);
	cairo_user_to_device(data->cr, &x, &y);
	data->mdata.ax=x;
	data->mdata.ay=y;

	x=toFL(PointB->x);
	y=toFL(PointB->y);
	cairo_user_to_device(data->cr, &x, &y);
	data->mdata.bx=x;
	data->mdata.by=y;

	x=toFL(PointC->x);
	y=toFL(PointC->y);
	cairo_user_to_device(data->cr, &x, &y);
	data->mdata.cx=x;
	data->mdata.cy=y;

	width=BitmapInfo->biWidth;
	height=BitmapInfo->biHeight;

/*	data->surface=xcl_check_bm_cache(Bitmap);
	if(data->surface)
	{*/
/*		printf("MAP from cache...\n");fflush(stdout);*/
	/*	cairo_surface_reference(data->surface);
		data->source=S_SURFACE;

		return 0;
	}*/
	data->surface=cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
						width, height);

	ss=(UINT32 *)Bitmap;
	dd=(UINT32 *)cairo_image_surface_get_data(data->surface);

	cd=cs=0;
	t=width * height;

//	if(BitmapInfo->biBitCount==24)
//	{
		BYTE *pout=(BYTE *)dd;
		BYTE *pin=(BYTE *)ss;
		UINT32 ww=width % 4;

		for (UINT32 yy = height; yy != 0; --yy)
		{
			for (UINT32 xx = width; xx != 0; --xx)
			{
				pout[0] = pin[0];
				pout[1] = pin[1];
				pout[2] = pin[2];
				pout[3] = 0xFF;
				pout += 4;
				pin += 3;
			}
			pin +=ww;
		}
//	}
//	else if(BitmapInfo->biBitCount==32)
//	{
//		UINT32 alpha;
//		while(cd < t)
//		{
//			alpha= (~ss[cd] & 0xFF000000) >> 24;
//			/*	premultiply alpha	*/
//
//			dd[cd]= (((ss[cd] & 0xFF0000) * alpha)/255) & 0xFF0000 |
//					(((ss[cd] & 0xFF00) * alpha)/255) & 0xFF00 |
//					((ss[cd] & 0xFF) * alpha)/255 | alpha << 24;
//
//			cd++;
//		}
//	}
/*	if(xcl_add_to_bm_cache(Bitmap, data->surface))
		printf("BM CACHE FULL...\n");
	else
		cairo_surface_reference(data->surface);
*/
	data->source=S_SURFACE;
	return 0;
}

INT32
GColour_SetTilePattern4 (
    pGCONTEXT pContext,
    pcBITMAPINFOHEADER BitmapInfo,
    pcBYTE Bitmap,
    DWORD Style,
    pcPOINT PointA,
    pcPOINT PointB,
    pcPOINT PointC,
    pcPOINT PointD,
    COLORREF DefaultColour,
    pcBGRT TranslationTable,
    pcBYTE   RedTranslationTable,
    pcBYTE GreenTranslationTable,
    pcBYTE  BlueTranslationTable,
    pcBYTE TransparencyTranslationTable,
    INT32 TileOffset
    )
{
    STUB ("GColour_SetTilePattern4");
	statlist[65].count++;
    return 0;
}

INT32
GColour_SetTransparentTilePattern (
    pGCONTEXT pContext,
    pcBITMAPINFOHEADER BitmapInfo,
    pcBYTE Bitmap,
    DWORD Style,
    pcPOINT PointA,
    pcPOINT PointB,
    pcPOINT PointC,
    BYTE DefaultColour,
    pcBYTE TransparencyTranslationTable,
    INT32 TileOffset
    )
{
/*	printf("GColour_SetTransparentTilePattern\n");*/
	statlist[66].count++;

/*	printf("SOURCE : WxH=%d x %d BITS=%d COMP=%08X\n",
		BitmapInfo->biWidth, BitmapInfo->biHeight,
		BitmapInfo->biBitCount, BitmapInfo->biCompression);
*/
	INT32 width, height;
	GCDATA *data=dp_list[pContext->Data[0]];
	double x, y;

/*	printf("Style=%08X\n", Style);*/

	data->transparency=(~Style & 0x00FF0000) >> 16;

	data->mdata.width=BitmapInfo->biWidth;
	data->mdata.height=BitmapInfo->biHeight;

	x=toFL(PointA->x);
	y=toFL(PointA->y);
	cairo_user_to_device(data->cr, &x, &y);
	data->mdata.ax=x;
	data->mdata.ay=y;

	x=toFL(PointB->x);
	y=toFL(PointB->y);
	cairo_user_to_device(data->cr, &x, &y);
	data->mdata.bx=x;
	data->mdata.by=y;

	x=toFL(PointC->x);
	y=toFL(PointC->y);
	cairo_user_to_device(data->cr, &x, &y);
	data->mdata.cx=x;
	data->mdata.cy=y;

	width=BitmapInfo->biWidth;
	height=BitmapInfo->biHeight;

	data->color=DefaultColour;

	UINT32 stride;
	/*
	*	The scanlines are 4 bytes aligned 
	*	see Kernel/bshadow.cpp
	*
	*/
	stride=width + ((width % 4)==0 ? 0 : 4 - (width % 4));

	data->surface=cairo_image_surface_create_for_data((unsigned char *)Bitmap,
				CAIRO_FORMAT_A8, width, height, stride);

	data->source=S_ALPHA_SURFACE;
	return 0;
}

INT32
GColour_SetTransparentTilePattern4 (
    pGCONTEXT pContext,
    pcBITMAPINFOHEADER BitmapInfo,
    pcBYTE Bitmap,
    DWORD Style,
    pcPOINT PointA,
    pcPOINT PointB,
    pcPOINT PointC,
    pcPOINT PointD,
    BYTE DefaultColour,
    pcBYTE TransparencyTranslationTable,
    INT32 TileOffset
    )
{
    STUB ("GColour_SetTransparentTilePattern4");
	statlist[67].count++;
    return 0;
}

/*	Converts bitmaps from one format to another.
	
*/
INT32
GColour_ConvertBitmap (
    pGCONTEXT pContext,
    pcBITMAPINFOHEADER SBitmapInfo, pcBYTE SBitmap,
    pcBITMAPINFOHEADER DBitmapInfo,  pBYTE DBitmap,
    DWORD Dither
    )
{
	statlist[68].count++;
/*
	printf("ConvertBitmap\n");
	printf("SRC = %d x %d pixels: depth=%d \n",
		SBitmapInfo->biWidth, SBitmapInfo->biHeight,
		SBitmapInfo->biBitCount);
	printf(" biCompression = %d\n", SBitmapInfo->biCompression);

	printf("DST = %d x %d pixels: depth=%d \n",
		DBitmapInfo->biWidth, DBitmapInfo->biHeight,
		DBitmapInfo->biBitCount);

	printf(" biCompression = %d\n", DBitmapInfo->biCompression);
*/
	if(SBitmapInfo->biBitCount !=32
	|| (DBitmapInfo->biBitCount !=24
		&& DBitmapInfo->biBitCount !=32 ))
		return 0;

	UINT32 cs=0, cd=0, t, ts;


	t=DBitmapInfo->biWidth * DBitmapInfo->biHeight;
	ts=SBitmapInfo->biWidth * SBitmapInfo->biHeight;

	if(t !=ts)
		printf("DIFFERENT SIZES in ConvertBitmap\n");

	if(DBitmapInfo->biBitCount==24)
	{
		const BYTE *so;
		BYTE *de;
		so=SBitmap;
		de=DBitmap;
		INT32 i, j, k, st;

		st=4 - ((DBitmapInfo->biWidth *3) % 4);

		for(j=0; j < DBitmapInfo->biHeight; j++)
		{
			for(k=0; k < DBitmapInfo->biWidth; k++)
			{
				for(i=0; i < 3; i++)
					de[cd++]=so[cs++];

				cs++;
			}
			cd +=st;
		}
	}
	else if(DBitmapInfo->biBitCount==32)
	{
		UINT32 *ss, *dd;

		ss=(UINT32 *)SBitmap;
		dd=(UINT32 *)DBitmap;

		while(cd < t)
		{
			BYTE r, g, b, a;
			/*
			*	For some reason the alpha value has to be inverted.
			*	Maybe it's inverted again somewhere up before usage ?
			*/
			a=(ss[cs] & 0xFF000000) >> 24;
			if(a !=0 && a !=0xFF)/*	unpremultiply	*/
			{
				r=(ss[cs] & 0x00FF0000) >> 16;
				g=(ss[cs] & 0x0000FF00) >> 8;
				b=ss[cs] & 0x000000FF;

				r=((r* 255) + a/2)/a;
				g=((g* 255) + a/2)/a;
				b=((b* 255) + a/2)/a;
	
				dd[cd]= ~a << 24 | r << 16 | g << 8 | b;
			}
			else
			{
				dd[cd]=ss[cs] & 0x00FFFFFF |  ~ss[cs] & 0xFF000000;
			}
			cd++;
			cs++;			
		}
	}
	return 0;
}

/*	Sets the Palette that is to be used for ConvertBitmap. 	*/
INT32
GColour_SetConversionPalette ( pGCONTEXT pContext,pcLOGPALETTE pPalette )
{
    STUB ("GColour_SetConversionPalette");
	statlist[69].count++;

	printf(" Set palette with %d entries\n", pPalette->palNumEntries);
    return 0;
}

INT32
GColour_SetMaxDiffusionError ( pGCONTEXT pContext,UINT32 MaxError )
{
    STUB ("GColour_SetMaxDiffusionError");
	statlist[70].count++;
    return 0;
}

INT32
GColour_ScaleBitmap (
    pGCONTEXT pContext,
    pcBITMAPINFOHEADER SBitmapInfo, pcBYTE SBitmap,
    pcBITMAPINFOHEADER DBitmapInfo,  pBYTE DBitmap,
    UINT32 Channels
    )
{
    STUB ("GColour_ScaleBitmap");
	printf("GColour_ScaleBitmap\n");
	statlist[71].count++;
    return 0;
}


INT32
GColour_SetGreyConversionValues ( pGCONTEXT pContext,UINT32 Red,UINT32 Green,UINT32 Blue )
{
    STUB ("GColour_SetGreyConversionValues");
	statlist[72].count++;
    return 0;
}


COLORREF
GColour_ConvertHSVtoRGB ( COLORREF hsv )
{
    STUB ("GColour_ConvertHSVtoRGB");
	statlist[73].count++;
    return 0;
}

COLORREF
GColour_ConvertRGBtoHSV ( COLORREF rgb )
{
    STUB ("GColour_ConvertRGBtoHSV");
	statlist[74].count++;
    return 0;
}

INT32
GColour_SetSeparationTables (
    pGCONTEXT pContext,
    pcBGR    CyanSeparationTable,
    pcBGR MagentaSeparationTable,
    pcBGR  YellowSeparationTable,
    pcBGR   BlackSeparationTable,
    pcBYTE UnderColourRemovalTable,
    pcBYTE BlackGenerationTable
    )
{
    STUB ("GColour_SetSeparationTables");
	statlist[75].count++;
    return 0;
}

INT32
GColour_SetBitmapConversionTable (
    pGCONTEXT pContext,
    pcBGR BitmapConversionTable
    )
{
    STUB ("GColour_SetBitmapConversionTable");
	statlist[76].count++;
    return 0;
}

/*	This is the bitmap that will be drawn into with rendering calls. 	*/
INT32
GDraw_SetDIBitmap (
    pGCONTEXT pContext,
    pcBITMAPINFOHEADER BitmapInfo,
    pBYTE Bitmap,
    UINT32 Format16BPP
    )
{
	XCL_DEBUG("GDraw_SetDIBitmap\n");
	statlist[77].count++;

	GCDATA *data=dp_list[pContext->Data[0]];

/*	printf("SetDIBitmap : INDEX=%d,  Width=%d, Height=%d, Depth=%d, Format=%d\n",
			pContext->Data[0], BitmapInfo->biWidth, BitmapInfo->biHeight,
			BitmapInfo->biBitCount, Format16BPP);
*/
	data->width=BitmapInfo->biWidth;
	data->height=BitmapInfo->biHeight;
	data->depth=BitmapInfo->biBitCount;
	data->format=Format16BPP;
	data->pixels=Bitmap;

	data->color=0xFF000000;
	data->source=S_COLOR;
	data->transparency=0xFF;

	if(data->surface)
	{
		/*while(cairo_surface_get_reference_count(data->surface) > 1)*/
			cairo_surface_destroy(data->surface);

		data->surface=NULL;
	}
	if(data->cr)
	{
		cairo_surface_destroy(cairo_get_target(data->cr));
		cairo_destroy(data->cr);
		data->cr=NULL;
		ncairo_types--;
	}
	xcl_ensure_cr(data);

	pContext->Valid=0xC90FDAA2;

	return 0;
}

INT32
GDraw_SetInvertedDIBitmap (
    pGCONTEXT pContext,
    pcBITMAPINFOHEADER BitmapInfo,
    pBYTE Bitmap,
    UINT32 Format16BPP
    )
{
	printf("SetInvertedDIBitmap : Width=%d, Height=%d, Depth=%d, Format=%d\n",
			BitmapInfo->biWidth, BitmapInfo->biHeight,BitmapInfo->biBitCount, Format16BPP);

    STUB ("GDraw_SetInvertedDIBitmap");
	statlist[78].count++;
    return 0;
}

/*	Sets the matrix to be used during rendering.  */
INT32
GDraw_SetMatrix ( pGCONTEXT pContext,pcGMATRIX Matrix )
{
	XCL_DEBUG("GDraw_SetMatrix\n");
	statlist[79].count++;

	GCDATA *data=dp_list[pContext->Data[0]];
	cairo_matrix_t matrix;

	if(pContext->Valid != 0xC90FDAA2){
		printf("Invalid context in GDraw_SetMatrix\n");
		return 0;
	}

	matrix.xx=(double)(Matrix->AX)/(1 << FX);
	matrix.yx=(double)(Matrix->AY)/(1 << FX);
	matrix.xy=(double)(Matrix->BX)/(1 << FX);
	matrix.yy=(double)(Matrix->BY)/(1 << FX);
	matrix.x0=(double)(Matrix->CX)/(1 << 30);
	matrix.y0=(double)(Matrix->CY)/(1 << 30);

/*	printf(" SET :: xx=%10f yx=%10f xy=%10f yy=%10f x0=%10f y0=%10f\n",
		matrix.xx, matrix.yx,
		matrix.xy, matrix.yy,
		matrix.x0, matrix.y0);
*/
	cairo_set_matrix(data->cr, &matrix);
	return 0;
}

/*	This returns a scale factor that should be safe to use for a drawing 
*	of the size of the given bounding box.
*	Currently only the translation values of the matrix are used.
*	To convert this maximum scale factor to a zoom value,
*	divide by the scale factor required for 100% zoom
*/
INT32
GDraw_MaxScale ( pGCONTEXT pContext,pcGMATRIX Matrix,pcRECT BBox )
{
    STUB ("GDraw_MaxScale");
	statlist[80].count++;
    return 0;
}

INT32
GDraw_GetMaxBitmapWidth (void)
{
	statlist[81].count++;
	return MAX_BITMAP_WIDTH;
}

INT32
GDraw_GetMaxBitmapDepth (void)
{
	statlist[82].count++;
	return 32;
}

INT32
GDraw_SetAntialiasFlag ( pGCONTEXT pContext,bool Flag )
{
	statlist[83].count++;

	if(pContext->Valid != 0xC90FDAA0
	&& pContext->Valid != 0xC90FDAA2)
	{
		printf("Invalid context in GDraw_SetAntialiasFlag.\n");
		return 0;
	}

	GCDATA *data=dp_list[pContext->Data[0]];
	data->antialias=Flag;
	return 0;
}

INT32
GDraw_SetAntialiasQualityFlag ( pGCONTEXT pContext,bool Flag )
{
	statlist[84].count++;
	/*	no op here	*/
	return 0;
}

INT32
GDraw_SetDashAdjustmentFlag ( pGCONTEXT pContext,bool Flag )
{
 /*   STUB ("GDraw_SetDashAdjustmentFlag");*/
	statlist[85].count++;
    return 0;
}


/*	Sets the flag in GDraw which forces all exactly
*	horizontal and vertical line segments to be hinted onto pixel centres. 
*/
INT32
GDraw_SetHintingFlag ( pGCONTEXT pContext,bool Flag )
{
	/*STUB ("GDraw_SetHintingFlag");*/
	statlist[86].count++;

	/*	no op here	*/

	return 0;
}

INT32
GDraw_SetFlatness ( pGCONTEXT pContext,UINT32 Flatness )
{
	statlist[87].count++;

	if(pContext->Valid != 0xC90FDAA0
	&& pContext->Valid != 0xC90FDAA2)
	{
		printf("Invalid context in GDraw_SetFlatness.\n");
		return 0;
	}
	GCDATA *data=dp_list[pContext->Data[0]];
	data->flatness=Flatness;
	return 0;
}

INT32
GDraw_SetMiterLimit ( pGCONTEXT pContext,UINT32 MiterLimit )
{
	STUB ("GDraw_SetMiterLimit");
	statlist[88].count++;

	printf("SetMiterLimit Input value = %u\n", MiterLimit);

	return 0;
}

#define MAX(a,b) (a) > (b) ? (a):(b)
#define MIN(a,b) (a) < (b) ? (a):(b)


INT32
GDraw_FillRectangle ( pGCONTEXT pContext,pcRECT Rectangle )
{
	cairo_t	*cr;
	GCDATA	*data=dp_list[pContext->Data[0]];
	double	x1, x2, y1,y2;

	statlist[89].count++;
	xcl_ensure_cr(data);
	cr=data->cr;

	x1=toFL(Rectangle->left);
	x2=toFL(Rectangle->right);
	y1=toFL(Rectangle->top);
	y2=toFL(Rectangle->bottom);

	cairo_rectangle(cr, x1, y1, x2 - x1, y2 - y1);

	cairo_set_source_rgb(cr, (data->color & 0xFF)/255.0,
						((data->color & 0xFF00) >>8)/255.0,
						((data->color & 0xFF0000) >> 16)/255.0);

	cairo_fill(cr);

	cairo_user_to_device(cr, &x1, &y1);
	cairo_user_to_device(cr, &x2, &y2);

	xcl_add_to_bbox(data, x1, y1, x2, y2);

	if(cairo_status(cr)){
		printf("ERROR : GDraw_FillRectangle\ncairo error=%s\n",
					cairo_status_to_string(cairo_status(cr)));
	}
	return 0;
}

INT32
GDraw_FillPoint ( pGCONTEXT pContext,pcPOINT Point )
{
	statlist[90].count++;

	cairo_t	*cr;
	GCDATA	*data=dp_list[pContext->Data[0]];
	double	x, y;

	xcl_ensure_cr(data);
	cr=data->cr;

	x=1.0;
	y=0.0;
	cairo_device_to_user_distance(cr, &x, &y);

	cairo_rectangle(cr, toFL(Point->x), toFL(Point->y), x, x);

	cairo_set_source_rgb(cr, (data->color & 0xFF)/255.0,
						((data->color & 0xFF00) >>8)/255.0,
						 ((data->color & 0xFF0000) >> 16)/255.0);

	cairo_fill(cr);

	x=toFL(Point->x);
	y=toFL(Point->y);
	cairo_user_to_device(cr, &x, &y);
	xcl_add_to_bbox(data, x, y, x+1, y+1);

	if(cairo_status(cr)){
		printf("ERROR : GDraw_FillPoint\ncairo error=%s\n",
					cairo_status_to_string(cairo_status(cr)));
	}
	return 0;
}


INT32
GDraw_FillPath (
    pGCONTEXT pContext,
    pcPOINT Points,
    pcBYTE  Types,
    UINT32 Length,
    UINT32 Winding
    )
{
	GCDATA		*data=dp_list[pContext->Data[0]];
	cairo_t		*cr;
	cairo_surface_t	*mask=NULL;
	double		x, y, x1, y1, x2, y2;

	XCL_DEBUG("GDraw_FillPath\n");

	statlist[91].count++;
	xcl_ensure_cr(data);
	cr=data->cr;

	xcl_set_cairo_path(cr, Points, Types, Length, 1);
	cairo_path_extents(cr,  &x1, &y1, &x2, &y2);

	cairo_user_to_device(cr, &x1, &y1);
	cairo_user_to_device(cr, &x2, &y2);

	if((INT32)x1 >= data->width || (INT32)y1 >= data->height
	||  (INT32)(x2 +1) < 1 || (INT32)(y2 + 1) < 1)
	{
		/*  bbox not on surface	*/
		goto RETURN;
	}
	xcl_add_to_bbox(data, x1, y1, x2, y2);

	if(Winding)/*	0 = winding ; 1 = even-odd	*/
		cairo_set_fill_rule(cr, CAIRO_FILL_RULE_EVEN_ODD);
	else
		cairo_set_fill_rule(cr, CAIRO_FILL_RULE_WINDING);

	if(data->flatness)
	{
		x=toFL(data->flatness);
		y=0.0;

		cairo_user_to_device_distance(cr, &x, &y);
		/*printf("flatness %f\n", x);*/
		if(x < 0.1)
			x=0.1;

		cairo_set_tolerance(cr, x);
	}
	if(data->antialias)
		cairo_set_antialias(cr, CAIRO_ANTIALIAS_DEFAULT);
	else
		cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);
	/*
	*	To make the hittest work properly.
	*	Problem :
	*	bitmap is initialised 0x00000000 in wxOil/grndrgn.cpp but filled with 0xFFFFFFFF at this point.
	*
	*	This fix will give wrong results if a real 7 x 7 part of the dwg has to be drawn
	*
	*/
	if(data->width==7 && data->height==7)
	{
/*		UINT32 *pix=(UINT32 *)data->pixels, i;
		for(i=0; i<49; i++)
			printf("%X\n", pix[i]);
*/
		memset((void *)data->pixels, 0x00, 7 * 7 * 4);
	}
	if(data->use_mask)
	{
		if(data->source==S_COLOR
		&& ((data->tpat.style & 0xF) ==1
		||	(data->tpat.style & 0xF) ==0) ){
			/*	Special case : flat color fill with linear or elliptical transp. pattern
			*	No mask needed
			*/
			xcl_fill_with_color_and_transparent_pattern(data);
			data->source=S_ABSENT;
		}else{
			mask=xcl_create_alpha_mask(data, (INT32)x1, (INT32)y1, (INT32)x2+1, (INT32)y2+1);
		}
		data->use_mask=0;
	}
	if(data->source==S_COLOR){
		xcl_fill_with_color(data, mask);

	}else if(data->source==S_PATTERN){
		xcl_fill_with_pattern(data, mask);

	}else if(data->source==S_SURFACE
		||  data->source==S_ALPHA_SURFACE){
		xcl_fill_with_surface(data, mask);
	}
	if(mask)
		cairo_surface_destroy(mask);

	data->source=S_ABSENT;

	if(cairo_status(cr)){
		printf("ERROR : in GDraw_FillPath\ncairo error=%s\n",
					cairo_status_to_string(cairo_status(cr)));
		printf("SOURCE was %d\n", data->source);
		printf("SIZE was %d  x %d\n", data->width, data->height);	
	}
	/*
	*	For dev : prints the path in pixel-coords
	*/
/*	printf("FillPath : Path with length %d was :\n", Length);*/
/*	for(c=0; c < Length; c++)
	{
		x1=toFL(Points[c].x);
		y1=toFL(Points[c].y);
		cairo_user_to_device(cr, &x1, &y1);
		printf("%d, %12f, %12f\n", Types[c], x1, y1);
	}
*/
	return 0;

RETURN:
	if(data->source==S_SURFACE
	||  data->source==S_ALPHA_SURFACE
/*	&& cairo_surface_get_reference_count(data->surface) > 1*/)
		cairo_surface_destroy(data->surface);

	data->surface=NULL;
	data->use_mask=0;
	cairo_new_path(cr);
	return 0;
}


static cairo_pattern_t *
xcl_create_cairo_pattern(patterndata *pinfo, INT32 offset_x, INT32 offset_y, pattern_type type)
{
	cairo_pattern_t	*pat;
	double			a, b, c, d, e, f;
	cairo_matrix_t		matrix;
	cairo_status_t		err;
	/*
	*	calculate the matrix members from the three given transformed points 
	*	pinfo->a, pinfo->b, pinfo->c
	*	and their original positions (0, 0 ), (1, 0) and (0, 1).
	*	using : 	x_transformed = a*x + b*y + c
	*			y_transformed = d*x + e*y + f
	*/
	if(type==PATTERN_IF2
	|| type==PATTERN_IF4)
	{/*	only used for test interfering alpha masks	*/
		/*	calculate ther fourth point of the parallelogram */
		c=pinfo->cx + (pinfo->bx - pinfo->ax) ;
		f=pinfo->by + (pinfo->cy - pinfo->ay);
		/*	switch points pinfo->b and pinfo->c */
		b=pinfo->bx - c;
		e=pinfo->by - f;
		a=pinfo->cx - c;
		d=pinfo->cy - f;
	}
	else/*	the normal case	*/
	{
		c=pinfo->ax;
		f=pinfo->ay;
		b=pinfo->cx - c;
		e=pinfo->cy - f;
		a=pinfo->bx - c;
		d=pinfo->by - f;
	}
	cairo_matrix_init(&matrix, a, d, b, e, c, f);
	/*
	*	Need to invert this matrix. It'll be set on a pattern instead of a cairo_t
	*/
	err=cairo_matrix_invert(&matrix);
	if(err)
		printf("ERROR : in xcl_create_cairo_pattern ( invert matrix)\ncairo error=%s\n",
					cairo_status_to_string(err));

	if(offset_x || offset_y)
		cairo_matrix_translate(&matrix, offset_x, offset_y);

	if(type==PATTERN_ELLIPTICAL
	||  type==PATTERN_IF1
	||  type==PATTERN_IF3
	||  type==PATTERN_IF4){
		pat=cairo_pattern_create_radial(0, 0, 0, 0, 0, 1);
	}else{
		pat=cairo_pattern_create_linear(0, 0, 0, 1);
	}
	cairo_pattern_set_matrix(pat, &matrix);
	return pat;
}

static INT32
xcl_fill_with_color_and_transparent_pattern(GCDATA *data)
{
	cairo_pattern_t	*tpat;
	UINT32			c, i;

	if((data->tpat.style & 0xF) ==1)
		tpat=xcl_create_cairo_pattern(&data->tpat, 0, 0, PATTERN_ELLIPTICAL);
	else
		tpat=xcl_create_cairo_pattern(&data->tpat, 0, 0, PATTERN_LINEAR);

	if(data->tpat.style & 0x10000)
		cairo_pattern_set_extend(tpat, CAIRO_EXTEND_REFLECT);

	if(data->tsource==T_LIST)/*	Not used atm.  There's a bug somewhere up in wxOil/Kernel	*/
	{
		for(i=0; i < 256; i++){
			c=~data->tlist[i];
			cairo_pattern_add_color_stop_rgba(tpat, (double)i/255.0,
					(data->color & 0xFF)/255.0,
					((data->color & 0xFF00) >>8)/255.0,
					((data->color & 0xFF0000) >> 16)/255.0,
					( c & 0xFF)/255.0);
		}
	}
	else
	{
		c=data->tpat.value1;
		cairo_pattern_add_color_stop_rgba(tpat, 0,
				(data->color & 0xFF)/255.0,
				((data->color & 0xFF00) >>8)/255.0,
				((data->color & 0xFF0000) >> 16)/255.0,
				(~c & 0xFF)/255.0);

		c=data->tpat.value2;
		cairo_pattern_add_color_stop_rgba(tpat, 1,
				(data->color & 0xFF)/255.0,
				((data->color & 0xFF00) >>8)/255.0,
				((data->color & 0xFF0000) >> 16)/255.0,
				(~c & 0xFF)/255.0);

	}
	cairo_save(data->cr);
		cairo_identity_matrix(data->cr);
		cairo_set_source(data->cr, tpat);
		cairo_fill(data->cr);
		if(cairo_status(data->cr)){
			printf("ERROR :xcl_fill_with_color_and_transparent_pattern \ncairo error=%s\n",
					cairo_status_to_string(cairo_status(data->cr)));
		}
	cairo_restore(data->cr);
	cairo_pattern_destroy(tpat);
	return 0;
}

static INT32
xcl_fill_with_color(GCDATA *data, cairo_surface_t *mask)
{
	cairo_t *cr=data->cr;

	cairo_set_source_rgba(cr,
		(data->color & 0xFF)/255.0,
		((data->color & 0xFF00) >>8)/255.0,
		((data->color & 0xFF0000) >> 16)/255.0,
		data->transparency/255.0);

	if(mask){
		cairo_save(cr);
			cairo_clip(cr);
			cairo_identity_matrix(cr);
			cairo_mask_surface(cr, mask, data->tpat_offset_x, data->tpat_offset_y);
		cairo_restore(cr);
	}else{
		cairo_fill(cr);
	}
	return 0;
}


static INT32
xcl_fill_with_pattern(GCDATA *data, cairo_surface_t *mask)
{
	cairo_t			*cr=data->cr;
	cairo_pattern_t	*pat;
	UINT32			c;

#ifdef CAIRO_HAS_MESH_PATTERN
	if((data->cpat.style & 0xFF000000) == 0x33000000
	||  (data->cpat.style & 0xFF000000) == 0x44000000){
		xcl_fill_with_mesh_color_pattern(data, mask);
		return 0;
	}
#endif
	if((data->cpat.style & 0xF) == 3){
		xcl_fill_with_diamond_color_pattern(data, mask);
		return 0;
	}

	if((data->cpat.style & 0x1) == 1){
		pat=xcl_create_cairo_pattern(&data->cpat, 0, 0, PATTERN_ELLIPTICAL);
	}else{
		pat=xcl_create_cairo_pattern(&data->cpat, 0, 0, PATTERN_LINEAR);
	}
	if(data->psource==PAT_LIST)
	{
		UINT32 i;

		for(i=0; i < 256; i++){
			c=data->patlist[i];
			cairo_pattern_add_color_stop_rgba(pat, (double)i/255.0,
							( c & 0xFF)/255.0,
							(( c & 0xFF00) >>8)/255.0,
							(( c & 0xFF0000) >> 16)/255.0,
							data->transparency/255.0);
		}
		if(data->cpat.style & 0x80)
			cairo_pattern_set_extend(pat, CAIRO_EXTEND_REPEAT);
	}
	else
	{	
		c=data->cpat.value1;
		cairo_pattern_add_color_stop_rgba(pat, 0,
							( c & 0xFF)/255.0,
							(( c & 0xFF00) >>8)/255.0,
							(( c & 0xFF0000) >> 16)/255.0,
							data->transparency/255.0);

		c=data->cpat.value2;
		cairo_pattern_add_color_stop_rgba(pat, 1,
							( c & 0xFF)/255.0,
							(( c & 0xFF00) >>8)/255.0,
							(( c & 0xFF0000) >> 16)/255.0,
							data->transparency/255.0);
	}
	if(mask){	
		cairo_save(cr);
			cairo_clip(cr);
			cairo_identity_matrix(cr);
			cairo_set_source(cr, pat);
			cairo_mask_surface(cr, mask, data->tpat_offset_x, data->tpat_offset_y);
		cairo_restore(cr);
	}else{
		cairo_save(cr);
			cairo_identity_matrix(cr);
			cairo_set_source(cr, pat);
			cairo_fill(cr);
		cairo_restore(cr);
	}
	cairo_pattern_destroy(pat);
	return 0;
}

static INT32
xcl_fill_with_diamond_color_pattern(GCDATA *data, cairo_surface_t *mask)
{
	cairo_pattern_t *pat;
	cairo_t *cr=data->cr;

	pat=xcl_create_diamond_color_pattern(data);

	if(mask){	
		cairo_save(cr);
			cairo_clip(cr);
			cairo_identity_matrix(cr);
			cairo_set_source(cr, pat);
			cairo_mask_surface(cr, mask, data->tpat_offset_x, data->tpat_offset_y);
		cairo_restore(cr);
	}else{
		cairo_save(cr);
			cairo_identity_matrix(cr);
			cairo_set_source(cr, pat);
			cairo_fill(cr);
		cairo_restore(cr);
	}
	cairo_pattern_destroy(pat);
	return 0;
}

#define COLOR_DIAMOND_SIZE 256

	/*	the color list	*/
INT32 cdlist[COLOR_DIAMOND_SIZE / 2];
	/* the 'diamond' surface	*/
INT32 cdiamond[COLOR_DIAMOND_SIZE -1][COLOR_DIAMOND_SIZE -1];
	/*	the used surface size	*/
INT32 cdsize=COLOR_DIAMOND_SIZE - 1;

static void
xcl_fill_diamond_color_surface(GCDATA *data)
{
	cairo_pattern_t	*hpat;
	cairo_surface_t		*hs;
	cairo_t 			*hcr;
	INT32			valp, x, y, i, c;
	INT32			x_start=0, x_end=cdsize - 1, y_start=0, y_end=cdsize - 1;
	INT32			m_index=COLOR_DIAMOND_SIZE/2 - 1;
	/*
	*	create a linear helper pattern and set the colorpoints there
	*/
	hpat=cairo_pattern_create_linear(0, 0, COLOR_DIAMOND_SIZE/2, 0);
	if(data->psource==PAT_LIST)
	{
		for(i=0; i < 256; i++){
			c=data->patlist[i];
			cairo_pattern_add_color_stop_rgba(hpat,
							(double)(255 - i)/255.0,
							( c & 0xFF)/255.0,
							(( c & 0xFF00) >>8)/255.0,
							(( c & 0xFF0000) >> 16)/255.0,
							data->transparency/255.0);
		}
	}
	else
	{	
		c=data->cpat.value1;
		cairo_pattern_add_color_stop_rgba(hpat, 0,
							( c & 0xFF)/255.0,
							(( c & 0xFF00) >>8)/255.0,
							(( c & 0xFF0000) >> 16)/255.0,
							data->transparency/255.0);

		c=data->cpat.value2;
		cairo_pattern_add_color_stop_rgba(hpat, 1,
							( c & 0xFF)/255.0,
							(( c & 0xFF00) >>8)/255.0,
							(( c & 0xFF0000) >> 16)/255.0,
							data->transparency/255.0);
	}
	/*
	*	create a surface of the colorlist (cdlist)
	*/
	hs=cairo_image_surface_create_for_data((unsigned char *)cdlist,
		CAIRO_FORMAT_ARGB32, COLOR_DIAMOND_SIZE/2, 1,
		COLOR_DIAMOND_SIZE/2 * 4 );

	hcr=cairo_create(hs);
	/*	to get the correct alpha in the list	*/
	cairo_set_operator(hcr, CAIRO_OPERATOR_SOURCE);
	/*
	*	fill the colorlist from the pattern
	*/
	cairo_set_source(hcr, hpat);
	cairo_paint(hcr);
	cairo_surface_destroy(hs);
	cairo_destroy(hcr);
	cairo_pattern_destroy(hpat);
	/*
	*	fill the diamond surface with the colors from the list
	*/
	for(valp=0; valp < m_index; valp++)
	{
		for(x=x_start; x < x_end + 1; x++){
			cdiamond[x][y_end]=cdiamond[x][y_start]=cdlist[valp];
		}
		for(y=y_start+1; y < y_end ; y++){
			cdiamond[x_start][y]=cdiamond[x_end][y]=cdlist[valp];
		}
		x_start++;
		x_end--;
		y_start++;
		y_end--;
	}
	cdiamond[m_index][m_index]=cdlist[m_index];/* The center pixel	*/
}

static cairo_pattern_t *
xcl_create_diamond_color_pattern(GCDATA *data)
{
	cairo_pattern_t 	*pat;
	cairo_surface_t		*cdsurface=NULL;
	cairo_matrix_t		matrix;
	patterndata		*tp=&data->cpat;
	cairo_status_t		err;
	double			a, b, c, d, e, f;
	double			half_dsize=(double)cdsize/2.0;
	INT32			stride;
	/*
	*	fill the matrix acc. to the transformed points tp->a, tp->b, tp->c in the patterndata
	*/
	c=tp->ax;
	f=tp->ay;
	b=(tp->cx - c)/half_dsize;
	e=(tp->cy - f)/half_dsize;
	a=(tp->bx - c)/half_dsize;
	d=(tp->by - f)/half_dsize;
	cairo_matrix_init(&matrix, a, d, b, e, c, f);

	/*	Move the center to the center-blob in pattern space	*/
	cairo_matrix_translate(&matrix, - half_dsize, - half_dsize);
	/*
	*	invert matrix as it's set on the pattern
	*/
	err=cairo_matrix_invert(&matrix);
	if(err)
		printf("ERROR : in xcl_create_diamond_alpha_mask ( invert matrix)\ncairo error=%s\n",
					cairo_status_to_string(err));

	xcl_fill_diamond_color_surface(data);

	stride=cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, cdsize);
	/*
	*	create a surface from the 'diamond' and THE pattern from that surface
	*/
	cdsurface=cairo_image_surface_create_for_data((unsigned char *)cdiamond,
				CAIRO_FORMAT_ARGB32, cdsize, cdsize, stride);

	pat=cairo_pattern_create_for_surface(cdsurface);
	cairo_pattern_set_matrix(pat, &matrix);

	if(data->cpat.style & 0x80)
		cairo_pattern_set_extend(pat, CAIRO_EXTEND_REPEAT);
	else
		cairo_pattern_set_extend(pat, CAIRO_EXTEND_PAD);

	cairo_surface_destroy(cdsurface);
	return pat;
}

#ifdef CAIRO_HAS_MESH_PATTERN
static INT32
xcl_fill_with_mesh_color_pattern(GCDATA *data, cairo_surface_t *mask)
{
	cairo_pattern_t *pat;
	cairo_t *cr=data->cr;

	pat=xcl_create_mesh_color_pattern(data);

	if(mask){	
		cairo_save(cr);
			cairo_clip(cr);
			cairo_identity_matrix(cr);
			cairo_set_source(cr, pat);
			cairo_mask_surface(cr, mask, data->tpat_offset_x, data->tpat_offset_y);
		cairo_restore(cr);
	}else{
		cairo_save(cr);
			cairo_identity_matrix(cr);

			cairo_set_source(cr, pat);
			cairo_fill(cr);
		cairo_restore(cr);
	}
	cairo_pattern_destroy(pat);
	return 0;
}

static cairo_pattern_t *
xcl_create_mesh_color_pattern(GCDATA *data)
{
	cairo_pattern_t	*pat;
	cairo_matrix_t		matrix;
	double			a, b, c, d, e, f, cx, cy;
	patterndata		*tp=&data->cpat;
	cairo_status_t		err;
	cairo_surface_t		*surf;
	cairo_t			*cr;

	c=tp->ax;
	f=tp->ay;
	b=(tp->cx - c)/256.0;
	e=(tp->cy - f)/256.0;
	a=(tp->bx - c)/256.0;
	d=(tp->by - f)/256.0;

	cairo_matrix_init(&matrix, a, d, b, e, c, f);

	err=cairo_matrix_invert(&matrix);
	if(err)
		printf("ERROR : in xcl_create_mesh_color_pattern ( invert matrix)\ncairo error=%s\n",
					cairo_status_to_string(err));

	pat=cairo_pattern_create_mesh();

	cairo_pattern_begin_patch(pat);

	cairo_pattern_move_to(pat, 0, 0);
	cairo_pattern_line_to(pat, 256, 0);
/*	if((tp->style & 0xFF000000)==0x44000000)*/
		cairo_pattern_line_to(pat, 256, 256);
/*	else
		cairo_pattern_line_to(pat, 256, 0);
*/
	cairo_pattern_line_to(pat, 0, 256);
	cairo_pattern_line_to(pat, 0, 0);

	cairo_pattern_set_corner_color_rgba(pat, 0,
				( tp->value1 & 0xFF)/255.0,
				(( tp->value1 & 0xFF00) >>8)/255.0,
				(( tp->value1 & 0xFF0000) >> 16)/255.0,
				data->transparency/255.0);

	cairo_pattern_set_corner_color_rgba(pat, 1,
				( tp->value2 & 0xFF)/255.0,
				(( tp->value2 & 0xFF00) >>8)/255.0,
				(( tp->value2 & 0xFF0000) >> 16)/255.0,
				data->transparency/255.0);

	if((tp->style & 0xFF000000)==0x44000000)
	{
		cairo_pattern_set_corner_color_rgba(pat, 2,
				( tp->value3 & 0xFF)/255.0,
				(( tp->value3 & 0xFF00) >>8)/255.0,
				(( tp->value3 & 0xFF0000) >> 16)/255.0,
				data->transparency/255.0);


		cairo_pattern_set_corner_color_rgba(pat, 3,
				( tp->value4 & 0xFF)/255.0,
				(( tp->value4 & 0xFF00) >>8)/255.0,
				(( tp->value4 & 0xFF0000) >> 16)/255.0,
				data->transparency/255.0);
	}
	else
	{
		cairo_pattern_set_corner_color_rgba(pat, 2,
				(( tp->value2 & 0xFF)/255.0 + ( tp->value3 & 0xFF)/255.0)/2.0,
				((( tp->value2 & 0xFF00) >>8)/255.0 + (( tp->value3 & 0xFF00) >>8)/255.0)/2.0,
				((( tp->value2 & 0xFF0000) >> 16)/255.0 + (( tp->value3 & 0xFF0000) >> 16)/255.0)/2.0,
				data->transparency/255.0);

		cairo_pattern_set_corner_color_rgba(pat, 3,
				( tp->value3 & 0xFF)/255.0,
				(( tp->value3 & 0xFF00) >>8)/255.0,
				(( tp->value3 & 0xFF0000) >> 16)/255.0,
				data->transparency/255.0);


		cairo_pattern_set_control_point (pat, 2, 128, 128);
	}
	cairo_pattern_end_patch(pat);

	surf=cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 256, 256);
	cr=cairo_create(surf);
	cairo_set_source(cr, pat);
	cairo_paint(cr);

	if((tp->style & 0xFF000000)==0x33000000)
	{
		if((tp->style & 0xF)==2)
		{
			cairo_move_to(cr, 256, 0);
			cairo_line_to(cr, 256, 256);
			cairo_line_to(cr, 0, 256);
			cairo_close_path(cr);

			cairo_translate(cr, 128, 128);
			cairo_scale(cr, -1, 1);
			cairo_rotate(cr, -M_PI/2.0);
			cairo_translate(cr, -128, -128);
	
			cairo_set_source(cr, pat);
			cairo_fill(cr);
		}
	}
	/*	else
		{
			cairo_pattern_t *pat1=cairo_pattern_create_linear(0, 256, 256,0);
			cairo_pattern_add_color_stop_rgba(pat1, 0,
				( tp->value3 & 0xFF)/255.0,
				(( tp->value3 & 0xFF00) >>8)/255.0,
				(( tp->value3 & 0xFF0000) >> 16)/255.0,
				data->transparency/255.0);

			cairo_pattern_add_color_stop_rgba(pat1, 1,
				( tp->value2 & 0xFF)/255.0,
				(( tp->value2 & 0xFF00) >>8)/255.0,
				(( tp->value2 & 0xFF0000) >> 16)/255.0,
				data->transparency/255.0);

			cairo_move_to(cr, 256, 256);
			cairo_line_to(cr, 0, 256);
			cairo_line_to(cr, 256, 0);
			cairo_close_path(cr);
			cairo_set_source(cr, pat1);
			cairo_fill(cr);
			cairo_pattern_destroy(pat1);
		}
	}
*/
	cairo_pattern_destroy(pat);
	pat=cairo_pattern_create_for_surface(surf);

	cairo_destroy(cr);
	cairo_surface_destroy(surf);

	cairo_pattern_set_matrix(pat, &matrix);
	if((tp->style & 0xF)==2)
		cairo_pattern_set_extend(pat, CAIRO_EXTEND_REFLECT);
	else
		cairo_pattern_set_extend(pat, CAIRO_EXTEND_PAD);

	return pat;
}

#endif

static INT32
xcl_fill_with_surface(GCDATA *data, cairo_surface_t *mask)
{
	cairo_t		*cr=data->cr;
	cairo_matrix_t	matrix;
	mapdata		*md=&data->mdata;
	double 		a, b, c, d, e, f;
	/*
	*	calculate the matrix members from the three given transformed points md->a, md->b, md->c
	*	and their original positions (0, 0 ), (width, 0) and (0, height).
	*	using : 	x_transformed = a*x + b*y + c
	*			y_transformed = d*x + e*y + f
	*/
	c= md->ax;
	a=(md->bx - c)/md->width;
	b=(md->cx - c)/md->height;
	f= md->ay;
	d=(md->by - f)/md->width;
	e=(md->cy - f)/md->height;
	cairo_matrix_init(&matrix, a, d, b, e, c, f);

	cairo_save(cr);
		cairo_set_matrix(cr, &matrix);

		if(data->source==S_ALPHA_SURFACE)/*	shadow surface		*/
		{
			cairo_clip(cr);
			cairo_set_source_rgba(cr,
				(data->color & 0xFF)/255.0,
				((data->color & 0xFF00) >>8)/255.0,
				((data->color & 0xFF0000) >> 16)/255.0,
				data->transparency/*((data->color & 0xFF000000) >> 24)*//255.0);

			cairo_mask_surface(cr, data->surface, 0, 0);
		}
		else
		{
			cairo_set_source_surface(cr, data->surface, 0, 0);

			if(mask){
				cairo_clip(cr);
				cairo_identity_matrix(cr);
				cairo_mask_surface(cr, mask, data->tpat_offset_x, data->tpat_offset_y);
			}else if(data->transparency !=0xFF){
				cairo_clip(cr);
				cairo_paint_with_alpha(cr, data->transparency/255.0);
			}else{
				cairo_fill(cr);
			}
		}
	cairo_restore(cr);

	cairo_surface_destroy(data->surface);
	data->surface=NULL;
	return 0;
}

static cairo_surface_t *
xcl_create_alpha_mask(GCDATA *data, INT32 x1, INT32 y1,
							INT32 x2, INT32 y2)
{
	INT32		offset_x, offset_y, w, h;
	cairo_t		*cr;
	cairo_surface_t	*surface;
	/*
	*	The size should be the intersection of
	*	the surface (0, 0, data->width, data->height)
	*	and the bounding box ( x1, y1, x2, y2 )
	*/	
	offset_x= x1 < 0 ? 0 : x1;
	offset_y= y1 < 0 ? 0 : y1;
	if(x2 > data->width)
		w=data->width - offset_x;
	else
		w=x2 - offset_x;

	if(y2 > data->height)
		h=data->height - offset_y;
	else
		h=y2 - offset_y;

	if(w < 1 || h < 1){
		printf( "ERROR in  xcl_create_alpha_mask\nwxh= %d x %d\n", w, h);
		printf( "BOX was : %d, %d   x   %d, %d\n", x1, y1, x2, y2);
		printf( "SIZE was : %d x %d\n", data->width, data->height);
		return NULL;
	}
	/*
	*	This offset is used for positioning the pattern and mask
	*/
	data->tpat_offset_x=offset_x;
	data->tpat_offset_y=offset_y;

	surface=cairo_image_surface_create(CAIRO_FORMAT_A8, w, h);
	cr=cairo_create(surface);

	if((data->tpat.style & 0xFF000000) ==0x33000000
	||  (data->tpat.style & 0xFF000000) ==0x44000000)
		/*	3 way or 4 way pattern; replaced with interfering pattern	*/
		xcl_paint_interfering_alpha_mask(data, cr);

	else if((data->tpat.style & 0xF) ==3)/*	diamond pattern	*/
		xcl_paint_diamond_alpha_mask(data, cr);

	else
		xcl_paint_pattern_alpha_mask(data, cr);

	cairo_destroy(cr);
	data->tsource=T_ABSENT;
	return surface;	
}
 
static INT32
xcl_paint_pattern_alpha_mask(GCDATA *data, cairo_t *cr)
{
	cairo_pattern_t	*tpat;
	UINT32			c, i;

	tpat=xcl_create_cairo_pattern(&data->tpat,
		data->tpat_offset_x, data->tpat_offset_y,
		(data->tpat.style & 0xF) == 1 ? PATTERN_ELLIPTICAL : PATTERN_LINEAR );

	if(data->tpat.style & 0x10000)
		cairo_pattern_set_extend(tpat, CAIRO_EXTEND_REFLECT);

	if(data->tsource==T_LIST)/*	Not used atm.  There's a bug somewhere up in wxOil/Kernel	*/
	{
		for(i=0; i < 256; i++){
			c=~data->tlist[i];
			cairo_pattern_add_color_stop_rgba(tpat, (double)i/255.0,
				1, 1, 1, ( c & 0xFF)/255.0);
		}
	}
	else
	{
		c=data->tpat.value1;	
		cairo_pattern_add_color_stop_rgba(tpat, 0,
				1, 1, 1, (~c & 0xFF)/255.0);

		c=data->tpat.value2;
		cairo_pattern_add_color_stop_rgba(tpat, 1,
				1, 1, 1, (~c & 0xFF)/255.0);
	}
	cairo_set_source(cr, tpat);
	cairo_paint(cr);

	if(cairo_status(cr)){
		printf("ERROR :xcl_create_pattern_alpha_mask \ncairo error=%s\n",
					cairo_status_to_string(cairo_status(cr)));
	}
	cairo_pattern_destroy(tpat);
	return 0;
}

static INT32
xcl_paint_interfering_alpha_mask(GCDATA *data, cairo_t *cr)
{
	pattern_type type1, type2;
	cairo_pattern_t	*tpat;
	UINT32			c, i;

	if((data->tpat.style & 0xFF000000) ==0x33000000){
		type1=PATTERN_IF1;
		type2=PATTERN_IF2;
	}else{
		type1=PATTERN_IF3;
		type2=PATTERN_IF4;
	}
	tpat=xcl_create_cairo_pattern(&data->tpat,
		data->tpat_offset_x, data->tpat_offset_y, type1);

	if((data->tpat.style & 0xF)==2)
		cairo_pattern_set_extend(tpat, CAIRO_EXTEND_REFLECT);	

	if(data->tsource==T_LIST)/*	Not used atm.  There's a bug somewhere up in wxOil/Kernel	*/
	{
		for(i=0; i < 256; i++){
			c=~data->tlist[i];
			cairo_pattern_add_color_stop_rgba(tpat, (double)i/255.0,
				1, 1, 1, ( c & 0xFF)/255.0);
		}
	}
	else
	{
		c=data->tpat.value1;	
		cairo_pattern_add_color_stop_rgba(tpat, 0,
				1, 1, 1, (~c & 0xFF)/255.0);

		c=data->tpat.value2;
		cairo_pattern_add_color_stop_rgba(tpat, 1,
				1, 1, 1, (~c & 0xFF)/255.0);
	}
	cairo_set_source(cr, tpat);
	cairo_paint(cr);
	if(cairo_status(cr)){
		printf("ERROR :xcl_create_interfering_alpha_mask \ncairo error=%s\n",
					cairo_status_to_string(cairo_status(cr)));
	}
	cairo_pattern_destroy(tpat);

	/*	start 2nd layer	*/
	tpat=xcl_create_cairo_pattern(&data->tpat,
		data->tpat_offset_x, data->tpat_offset_y,
		type2);

	if((data->tpat.style & 0xF)==2)
		cairo_pattern_set_extend(tpat, CAIRO_EXTEND_REFLECT);	

	if(data->tsource==T_LIST)
	{
		for(i=0; i < 256; i++){
			c=~data->tlist[i];
			cairo_pattern_add_color_stop_rgba(tpat, (double)i/255.0,
				1, 1, 1, ( c & 0xFF)/255.0);
		}
	}
	else
	{
		c=data->tpat.value3;	
		cairo_pattern_add_color_stop_rgba(tpat, 0,
				1, 1, 1, (~c & 0xFF)/255.0);

		c=(type2==PATTERN_IF4 ? data->tpat.value4 : data->tpat.value1);
		cairo_pattern_add_color_stop_rgba(tpat, 1,
				1, 1, 1, (~c & 0xFF)/255.0);
	}
	cairo_set_source(cr, tpat);
	cairo_paint(cr);
	if(cairo_status(cr)){
		printf("ERROR :xcl_create_interfering_alpha_mask \ncairo error=%s\n",
					cairo_status_to_string(cairo_status(cr)));
	}
	cairo_pattern_destroy(tpat);
	return 0;
}


#define TRANSP_DIAMOND_SIZE 32


BYTE dslope[TRANSP_DIAMOND_SIZE / 2];
BYTE diamond1[TRANSP_DIAMOND_SIZE][TRANSP_DIAMOND_SIZE];
INT32 dsize=TRANSP_DIAMOND_SIZE - 1;

static void
xcl_fill_diamond_pattern(INT32 start, INT32 end)
{
	INT32 valp, x, y, x_start=0, x_end=dsize - 1, y_start=0, y_end=dsize - 1, t_diff, i;
	INT32 m_index=TRANSP_DIAMOND_SIZE/2 - 1;

	t_diff=end - start;
	dslope[0]=end;
	dslope[m_index]=start;
	for(i=1; i < m_index; i++){
		dslope[i]=start + (BYTE)((double)(m_index - i) * (double)t_diff/(double)m_index);
	}
	for(valp=0; valp < m_index; valp++)
	{
		for(x=x_start; x < x_end + 1; x++){
			diamond1[x][y_end]=diamond1[x][y_start]=dslope[valp];
		}
		for(y=y_start+1; y < y_end ; y++){
			diamond1[x_start][y]=diamond1[x_end][y]=dslope[valp];
		}
		x_start++;
		x_end--;
		y_start++;
		y_end--;
	}
	diamond1[m_index][m_index]=dslope[m_index];/* The center pixel	*/
}

static INT32
xcl_paint_diamond_alpha_mask(GCDATA *data, cairo_t *cr)
{
	static cairo_surface_t	*dsurface=NULL;
	static UINT32			t_start=0, t_end=0;
	cairo_matrix_t			matrix;
	patterndata			*tp=&data->tpat;
	cairo_status_t			err;
	double				a, b, c, d, e, f;
	double				half_dsize=(double)dsize/2.0;
	INT32				stride;

	c=tp->ax;
	f=tp->ay;
	b=(tp->cx - c)/half_dsize;
	e=(tp->cy - f)/half_dsize;
	a=(tp->bx - c)/half_dsize;
	d=(tp->by - f)/half_dsize;
	cairo_matrix_init(&matrix, a, d, b, e, c, f);

	/*	Move the center to the center-blob in pattern space	*/
	cairo_matrix_translate(&matrix, - half_dsize, - half_dsize);

	err=cairo_matrix_invert(&matrix);
	if(err)
		printf("ERROR : in xcl_create_diamond_alpha_mask ( invert matrix)\ncairo error=%s\n",
					cairo_status_to_string(err));

	/*	Set the offset in device space	*/
	cairo_matrix_translate(&matrix, data->tpat_offset_x, data->tpat_offset_y);
	/*
	*	see if the surface is already filled correct
	*/
	if(dsurface==NULL
	||  t_start !=~data->tpat.value1
	||  t_end !=~data->tpat.value2)
	{
		if(dsurface !=NULL)
			cairo_surface_destroy(dsurface);

		t_start=~data->tpat.value1;
		t_end=~data->tpat.value2;

		xcl_fill_diamond_pattern(t_start, t_end);
		stride=cairo_format_stride_for_width(CAIRO_FORMAT_A8, dsize);

		dsurface=cairo_image_surface_create_for_data((unsigned char *)diamond1,
				CAIRO_FORMAT_A8, dsize, dsize, stride);
	}
	cairo_set_source_surface(cr, dsurface, 0, 0);
	cairo_pattern_set_matrix(cairo_get_source(cr), &matrix);

	if(data->tpat.style & 0x10000){
		cairo_pattern_set_extend(cairo_get_source(cr), CAIRO_EXTEND_REPEAT);
	}else{
		cairo_pattern_set_extend(cairo_get_source(cr), CAIRO_EXTEND_PAD);
	}
	cairo_paint(cr);		
	if(cairo_status(cr)){
		printf("ERROR :xcl_create_diamond_alpha_mask \ncairo error=%s\n",
					cairo_status_to_string(cairo_status(cr)));
	}
	return 0;
}


INT32
GDraw_HintPath (
    pGCONTEXT pContext,
    pPOINT Points,
    pcBYTE  Types,
    UINT32 Length,
    bool Close,
    UINT32 LineWidth
    )
{
    STUB ("GDraw_HintPath");
	statlist[92].count++;
    return 0;
}

INT32
GDraw_StrokePath (
    pGCONTEXT pContext,
    pcPOINT Points,
    pcBYTE  Types,
    UINT32 Length,
    bool Close,
    UINT32 LineWidth,
    DWORD LineCaps,
    DWORD LineJoin,
    pcDashType Dash
    )
{
	UINT32	c;
	GCDATA	*data=dp_list[pContext->Data[0]];
	cairo_t	*cr;
	double	x, y, x1, y1, x2, y2;
	double	dashes[8];
	double	pix_linewidth, dummy;

	XCL_DEBUG("GDraw_StrokePath\n");

	statlist[93].count++;

	xcl_ensure_cr(data);
	cr=data->cr;

	xcl_set_cairo_path(cr, Points, Types, Length, Close);
	/*
	*	Check the linewidth.....   sometimes it's set to 0
	*/
	pix_linewidth=toFL(LineWidth);
	dummy=0.0;
	cairo_user_to_device_distance(cr, &pix_linewidth, &dummy);

	if(pix_linewidth < MIN_LINE_WIDTH)
	{
		x1=MIN_LINE_WIDTH;
		cairo_device_to_user_distance(cr, &x1, &dummy);
		LineWidth=(UINT32)toINT(x1);
	}
	cairo_set_line_width(cr, toFL(LineWidth));
	cairo_set_line_join(cr, xcl_xara_to_cairo_line_join(LineJoin));
	cairo_set_line_cap(cr, xcl_xara_to_cairo_line_cap(LineCaps));

	if(Dash==NULL)
	{
		cairo_set_dash(cr, NULL, 0, 0);
	}
	else
	{
		for(c=0; c < Dash->Length && c < 8; c++){
			dashes[c]=toFL(Dash->Array[c]);
		}
		cairo_set_dash(cr, dashes, Dash->Length, toFL(Dash->Offset));
	}
	cairo_stroke_extents(cr,  &x1, &y1, &x2, &y2);

	cairo_user_to_device(cr, &x1, &y1);
	cairo_user_to_device(cr, &x2, &y2);

	if((INT32)x1 >=data->width || (INT32)y1 >=data->height
	||  (INT32)(x2 + 1) < 1 || (INT32)(y2 + 1) < 1)
	{
	/*	bbox not on surface	*/
		cairo_new_path(cr);
		return 0;
	}
	xcl_add_to_bbox(data, x1, y1, x2, y2);

	if(data->flatness)
	{
		x=toFL(data->flatness);
		y=0.0;

		cairo_user_to_device_distance(cr, &x, &y);

		if(x < 0.1)
			x=0.1;

		cairo_set_tolerance(cr, x);
	}
	if(data->antialias)
		cairo_set_antialias(cr, CAIRO_ANTIALIAS_DEFAULT);
	else
		cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);
	/*
	*	For stroke, transparency seems to be set in the color ( in stead of data->transparency)
	*/
	cairo_set_source_rgba(cr, (data->color & 0xFF)/255.0,
						((data->color & 0xFF00) >>8)/255.0,
						((data->color & 0xFF0000) >> 16)/255.0,
						((data->color & 0xFF000000) >> 24)/255.0);

	cairo_stroke(cr);

	if(cairo_status(cr)){
		printf("ERROR : in GDraw_StrokePath\ncairo error=%s\n",
					cairo_status_to_string(cairo_status(cr)));
	}
	/*
	*	For dev : prints the path in pixel-coords
	*/
/*
	printf("StrokePath : Path with length %d was :\n", Length);
	for(c=0; c < Length; c++)
	{
		x1=toFL(Points[c].x);
		y1=toFL(Points[c].y);
		cairo_user_to_device(cr, &x1, &y1);
		printf("%d, %12f, %12f\n", Types[c], x1, y1);
	}
*/
	return 0;
}

static INT32
xcl_add_to_bbox(GCDATA *data, double x1, double y1, double x2, double y2)
{
	data->b_rect.top=MAX(0, MIN( data->b_rect.top,(INT32)y1));
	data->b_rect.left=MAX(0, MIN( data->b_rect.left,(INT32)x1));
	data->b_rect.right=MIN(data->width, MAX( data->b_rect.right, (INT32)(x2+1.0)));
	data->b_rect.bottom=MIN(data->height, MAX(data->b_rect.bottom, (INT32)(y2+1.0)));

	return 0;
}


static INT32
xcl_ensure_cr(GCDATA *data)
{
	cairo_surface_t *surface;

	if(data->cr
	&& !cairo_status(data->cr)
	&& cairo_image_surface_get_width(cairo_get_target(data->cr))==data->width
	&& cairo_image_surface_get_height(cairo_get_target(data->cr))==data->height
	&& (BYTE *)cairo_image_surface_get_data(cairo_get_target(data->cr))==data->pixels)
	{
		return 0;
	}
	if(data->cr
	&& cairo_status(data->cr))
	{
		printf("ENSURE CR : Removed CR with status : %s\n",
				cairo_status_to_string(cairo_status(data->cr)));

		cairo_surface_destroy(cairo_get_target(data->cr));
		cairo_destroy(data->cr);
		ncairo_types--;
		data->cr=NULL;
	}
	if(data->cr)
	{
		printf("ENSURE CR : Removed CR for wrong surface\n");
		cairo_surface_destroy(cairo_get_target(data->cr));
		cairo_destroy(data->cr);
		ncairo_types--;
		data->cr=NULL;

		xcl_clear_bounding_box(data);
	}	

	surface=cairo_image_surface_create_for_data((unsigned char *)data->pixels,
						CAIRO_FORMAT_ARGB32,
						data->width, data->height, data->width*4);

	data->cr=cairo_create(surface);
	ncairo_types++;

	if(cairo_status(data->cr)){
		printf("ERROR : in xcl_ensure_cr\ncairo error=%s\n",
				cairo_status_to_string(cairo_status(data->cr)));
	}
	return 0;
}

static INT32
xcl_clear_bounding_box(GCDATA *data)
{
	data->b_rect.top=data->height;
	data->b_rect.left=data->width;
	data->b_rect.right=0;
	data->b_rect.bottom=0;
	return 0;
}

#define MY_MOVE_TO 6
#define MY_CURVE_TO 4
#define MY_END_CURVE 5
#define MY_LINE_TO 2
#define MY_CLOSE_PATH 3

INT32
xcl_set_cairo_path(
	cairo_t	*cr,
	pcPOINT	Points,
	pcBYTE 	Types,
	UINT32	Length,
	bool		Close)
{
	UINT32 c=0;

	while(c < Length)
	{
		if(Types[c]==MY_MOVE_TO)
		{
			cairo_move_to(cr, toFL(Points[c].x), toFL(Points[c].y));
			c++;
		}
		else if(Types[c]==MY_LINE_TO)
		{
			cairo_line_to(cr, toFL(Points[c].x), toFL(Points[c].y));
			c++;
		}
		else if(Types[c]==MY_CURVE_TO)
		{
			if(c+2 >=Length)
			{
				printf("Error : no 3 points for curve left in path !\n");
				break;
			}
			if(Types[c+1] !=MY_CURVE_TO
			||( Types[c+2] !=MY_CURVE_TO && Types[c+2] != MY_END_CURVE))
					printf(" Invalid curve in path !!!\n");

			cairo_curve_to(cr, toFL(Points[c].x), toFL(Points[c].y),
						toFL(Points[c+1].x), toFL(Points[c+1].y),
						toFL(Points[c+2].x), toFL(Points[c+2].y));

			if(Types[c+2]==MY_END_CURVE)
				cairo_close_path(cr);

			c+=3;
		}
		else if(Types[c]==MY_CLOSE_PATH)
		{
			cairo_line_to(cr, toFL(Points[c].x), toFL(Points[c].y));
			cairo_close_path(cr);
			c++;
		}
		else if(Types[c]==MY_END_CURVE)
		{
			c++;		/*	ignore...	*/
		}
		else
		{
			printf(" ERROR : unknown path element %d in path !\n", Types[c] );
			c++;		/*	ignore...	*/
		}
	}
	if(Close)
		cairo_close_path(cr);
	return 0;
}

#undef MY_MOVE_TO
#undef MY_CURVE_TO
#undef MY_END_CURVE
#undef MY_LINE_TO
#undef MY_CLOSE_PATH


static cairo_line_cap_t
xcl_xara_to_cairo_line_cap(UINT32 cap)
{
	if(cap==CAPS_BUTT)
		return CAIRO_LINE_CAP_BUTT;
	else if(cap==CAPS_ROUND)
		return CAIRO_LINE_CAP_ROUND;
	else
		return CAIRO_LINE_CAP_SQUARE;
}

static cairo_line_join_t
xcl_xara_to_cairo_line_join(UINT32 join)
{
	if(join==JOIN_MITER)
		return CAIRO_LINE_JOIN_MITER;
	else if(join==JOIN_ROUND)
		return CAIRO_LINE_JOIN_ROUND;
	else
		return CAIRO_LINE_JOIN_BEVEL;
}


INT32
GDraw_StrokePathToPath (
    pGCONTEXT pContext,
    pcPOINT IPoints,
    pcBYTE  ITypes,
    UINT32 ILength,
    pPOINT OPoints,
    pBYTE  OTypes,
    UINT32 OLength,
    bool Close,
    UINT32 LineWidth,
    DWORD LineCaps,
    DWORD LineJoin,
    pcDashType Dash
    )
{
    STUB ("GDraw_StrokePathToPath");
   XCL_DEBUG("GDraw_StrokePathToPath\n");
	statlist[94].count++;
    return 0;
}

INT32
GDraw_CalcStrokeBBox (
    pGCONTEXT pContext,
    pcPOINT IPoints,
    pcBYTE  ITypes,
    UINT32 ILength,
    pRECT Rect,
    bool Close,
    UINT32 LineWidth,
    DWORD LineCaps,
    DWORD LineJoin,
    pcDashType Dash
    )
{
	GCDATA	*data=dp_list[pContext->Data[0]];
	cairo_t	*cr;
	double	x1, y1, x2, y2;
	double	dashes[8];
	UINT32	c;

	XCL_DEBUG("GDraw_CalcStrokeBBox\n");
	statlist[95].count++;
	/*
	*	Check the linewidth.....   sometimes it's set to 0
	*	commented out : might be cause of errors 
	*/
/*	if(LineWidth==0)
	{
		GDraw_CalcBBox(pContext, IPoints, ITypes, ILength, Rect, 0);
		return 0;
	}*/
	cr=data->cr;

	xcl_set_cairo_path(cr, IPoints, ITypes, ILength, Close);

	x1=toFL(LineWidth);
	y1=0.0;
	cairo_user_to_device_distance(cr, &x1, &y1);

	if(x1 < MIN_LINE_WIDTH)
	{
		x1=MIN_LINE_WIDTH;
		cairo_device_to_user_distance(cr, &x1, &y1);
		LineWidth=toINT(x1);
	}
	cairo_set_line_width(cr, toFL(LineWidth));
	cairo_set_line_join(cr, xcl_xara_to_cairo_line_join(LineJoin));
	cairo_set_line_cap(cr, xcl_xara_to_cairo_line_cap(LineCaps));

	if(Dash==NULL)
	{
		cairo_set_dash(cr, NULL, 0, 0);
	}
	else
	{
		for(c=0; c < Dash->Length && c < 8; c++)
		{
			dashes[c]=(double)Dash->Array[c];
		}
		cairo_set_dash(cr, dashes, Dash->Length, (double)Dash->Offset);
	}
	cairo_stroke_extents(cr, &x1, &y1, &x2, &y2);

	Rect->left=toINT(x1);
	Rect->top=toINT(y1);
	Rect->right=toINT(x2);
	Rect->bottom=toINT(y2);

	cairo_new_path(cr);
/*
	printf("GDraw_CalcStrokeBBox Index : %d  LU=( %8d, %8d) RD=( %8d, %8d)\n",
				pContext->Data[0], Rect->left, Rect->top, Rect->right, Rect->bottom);
*/
	return 0;
}

INT32
GDraw_IsOverlap (
    pGCONTEXT pContext,
    pcPOINT Points,
    pcBYTE  Types,
    UINT32 Length,
    UINT32 Winding
    )
{
    STUB ("GDraw_IsOverlap");
	statlist[96].count++;
    return 0;
}

INT32
GDraw_IsStrokeOverlap (
    pGCONTEXT pContext,
    pcPOINT Points,
    pcBYTE  Types,
    UINT32 Length,
    bool Close,
    UINT32 LineWidth,
    DWORD LineCaps,
    DWORD LineJoin,
    pcDashType Dash
    )
{
    STUB ("GDraw_IsStrokeOverlap");
	statlist[97].count++;
    return 0;
}

INT32
GDraw_GetStatistics (
    pGCONTEXT pContext,
    pcPOINT Points,
    pcBYTE  Types,
    UINT32 Length,
    UINT32 Winding,
    pSTATISTICS Stats
    )
{
    STUB ("GDraw_GetStatistics");
	statlist[98].count++;
    return 0;
}

INT32
GDraw_MakeRegion (
    pGCONTEXT pContext,
    pcPOINT Points,
    pcBYTE  Types,
    UINT32 Length,
    UINT32 Winding,
    pREGION ORegion,
    UINT32 OLength
    )
{
	XCL_DEBUG("GDraw_MakeRegion\n");
    STUB ("GDraw_MakeRegion");
	statlist[99].count++;
    return 0;
}

INT32
GDraw_MakeUnclippedRegion (
    pGCONTEXT pContext,
    pcPOINT Points,
    pcBYTE  Types,
    UINT32 Length,
    UINT32 Winding,
    pREGION ORegion,
    UINT32 OLength
    )
{
	XCL_DEBUG("GDraw_MakeUnclippedRegion\n");
    STUB ("GDraw_MakeUnclippedRegion");
	statlist[100].count++;
    return 0;
}

INT32
GDraw_FillRegion (
    pGCONTEXT pContext,
    pcREGION Region,
    pcPOINT Offset
    )
{
	XCL_DEBUG("GDraw_FillRegion\n");
    STUB ("GDraw_FillRegion");
	statlist[101].count++;
    return 0;
}

/*	Sets clip rectangle of subsequent rendering. 	*/
INT32
GDraw_ClipRectangle ( pGCONTEXT pContext,pcRECT Rectangle )
{
	XCL_DEBUG("GDraw_ClipRectangle\n");
	statlist[102].count++;

	GCDATA	*data=dp_list[pContext->Data[0]];
	cairo_t	*cr=data->cr;
	double	x1, y1, x2, y2;

/*	cairo_rectangle(cr, Rectangle->left, Rectangle->top,
		Rectangle->right-Rectangle->left, Rectangle->bottom-Rectangle->top);
*/

	cairo_rectangle(cr,toFL( Rectangle->left), toFL(Rectangle->top),
		toFL(Rectangle->right) - toFL(Rectangle->left),
		toFL(Rectangle->bottom) - toFL(Rectangle->top));

	cairo_clip(cr);

	x1=toFL(Rectangle->left);
	y1=toFL(Rectangle->top);
	x2=toFL(Rectangle->right);
	y2=toFL(Rectangle->bottom);

	cairo_user_to_device(cr, &x1, &y1);
	cairo_user_to_device(cr, &x2, &y2);

	xcl_add_to_bbox(data, x1, y1, x2, y2);
/*	printf(" CLIP RECT (device space) LU= ( %10f , %10f ) RD= ( %10f , %10f )\n",
			x1, y1, x2, y2);
*/
 	return 0;
}

INT32
GDraw_DeviceClipRectangle ( pGCONTEXT pContext,pcRECT Rectangle )
{
	XCL_DEBUG("GDraw_DeviceClipRectangle\n");
	statlist[103].count++;
	GCDATA *data=dp_list[pContext->Data[0]];
	cairo_t *cr=data->cr;

	cairo_reset_clip(cr);
	return 0;
}

INT32
GDraw_GetDeviceClipRectangle ( pcGCONTEXT pContext,pRECT Rectangle )
{
	XCL_DEBUG("GDraw_GetDeviceClipRectangle\n");
    STUB ("GDraw_GetDeviceClipRectangle");
	statlist[104].count++;
    return 0;
}

INT32
GDraw_ClipRegion ( pGCONTEXT pContext,pcREGION Region )
{
	XCL_DEBUG("GDraw_ClipRegion\n");
    STUB ("GDraw_ClipRegion");
	statlist[105].count++;
    return 0;
}

pcREGION
GDraw_GetClipRegion (pGCONTEXT pContext)
{
	XCL_DEBUG("GDraw_GetClipRegion\n");
    STUB ("GDraw_GetClipRegion");
	statlist[106].count++;
    return 0;
}

INT32
GDraw_CalcSimpleBBox (
    pcPOINT Points,
    UINT32 Length,
    pRECT Rect
    )
{
	XCL_DEBUG("GDraw_CalcSimpleBBox\n");
  /*  STUB ("GDraw_CalcSimpleBBox");*/
	statlist[107].count++;
    return 0;
}

INT32
GDraw_CalcBBox (
    pGCONTEXT pContext,
    pcPOINT Points,
    pcBYTE  Types,
    UINT32 Length,
    pRECT Rect,
    bool Flatten
    )
{
	GCDATA	*data=dp_list[pContext->Data[0]];
	cairo_t	*cr;
	double	x1, y1, x2, y2;

	XCL_DEBUG("GDraw_CalcBBox\n");
	statlist[108].count++;

	cr=data->cr;

	xcl_set_cairo_path(cr, Points, Types, Length, 1);

	cairo_path_extents(cr, &x1, &y1, &x2, &y2);

	cairo_new_path(cr);

	Rect->left=toINT(x1);
	Rect->top=toINT(y1);
	Rect->right=toINT(x2);
	Rect->bottom=toINT(y2);

/*
	printf("GDraw_CalcBBox Index: %d  LU=( %8d, %8d) RD=( %8d, %8d)\n",
				pContext->Data[0], Rect->left, Rect->top, Rect->right, Rect->bottom);
*/
	return 0;
}

/*	Convert any number of points using a GMatrix. 	*/
INT32
GDraw_TransformPath (
    pGCONTEXT pContext,
    pcPOINT IPoints,
    pPOINT OPoints,
    UINT32 Length,
    pcGMATRIX Matrix
    )
{
	XCL_DEBUG("GDraw_TransformPath\n");
	statlist[109].count++;

	cairo_matrix_t	tmatrix;
	UINT32		c;
	double		x, y;

	tmatrix.xx=(double)(Matrix->AX) / (double)(1 << FX);
	tmatrix.yx=(double)(Matrix->AY)/ (double)(1 << FX);
	tmatrix.xy=(double)(Matrix->BX)/ (double)(1 << FX);
	tmatrix.yy=(double)(Matrix->BY)/ (double)(1 << FX);
	tmatrix.x0=(double)(Matrix->CX)/ (double)(1 << 30);
	tmatrix.y0=(double)(Matrix->CY)/ (double)(1 << 30);
	/*
	*	x_new = xx * x + xy * y + x0;
	*	y_new = yx * x + yy * y + y0;
	*/
	for(c=0; c < Length; c++)
	{
		x=toFL(IPoints[c].x);
		y=toFL(IPoints[c].y);

		OPoints[c].x=toINT(x*tmatrix.xx + y*tmatrix.xy+tmatrix.x0);
		OPoints[c].y=toINT(x*tmatrix.yx + y*tmatrix.yy+tmatrix.y0);
	}
/*	printf(" x ,y (in)   was %d, %d\n", IPoints[0].x, IPoints[0].y);
	printf(" x ,y (out) was %d, %d\n", OPoints[0].x, OPoints[0].y);
*/
	return 0;
}

INT32
GDraw_ScrollBitmap ( pGCONTEXT pContext, INT32 x,INT32 y )
{
    STUB ("GDraw_ScrollBitmap");
	statlist[110].count++;
    return 0;
}

/*	Resets 'changed' rectangle.
	Should be called before anything is drawn,
	then drawn areas will be accumulated into the BBox.
*/
INT32
GDraw_ClearChangedBBox (pGCONTEXT pContext)
{
	XCL_DEBUG("GDraw_ClearChangedBBox\n");
	statlist[111].count++;
	GCDATA *data=dp_list[pContext->Data[0]];

	xcl_clear_bounding_box(data);

	return 0;
}

INT32
GDraw_GetChangedBBox ( pcGCONTEXT pContext, pRECT Rectangle )
{
	XCL_DEBUG("GDraw_GetChangedBBox\n");
	statlist[112].count++;

	GCDATA *data=dp_list[pContext->Data[0]];

	Rectangle->top=data->b_rect.top;
	Rectangle->left=data->b_rect.left;
	Rectangle->right=data->b_rect.right;
	Rectangle->bottom=data->b_rect.bottom;

/*	printf( " BBOX :: LU ( %d, %d )    RD  ( %d, %d )\n",
					data->b_rect.left, data->b_rect.top,
					data->b_rect.right, data->b_rect.bottom);
*/
	return 0;
}

INT32
GDraw_SetChangedBBox (  pGCONTEXT pContext,pcRECT Rectangle )
{
	XCL_DEBUG("GDraw_SetChangedBBox\n");
	statlist[113].count++;

	GCDATA *data=dp_list[pContext->Data[0]];

	data->b_rect.top=Rectangle->top;
	data->b_rect.left=Rectangle->left;
	data->b_rect.right=Rectangle->right;
	data->b_rect.bottom=Rectangle->bottom;

/*	printf(" BBOX set to top : %d, left : %d, right : %d, bottom : %d\n",
			 Rectangle->top, Rectangle->left, Rectangle->right, Rectangle->bottom);
*/
	return 0;
}

INT32
GDraw_SetBevelContrast ( pGCONTEXT pContext,UINT32 Contrast )
{
    STUB ("GDraw_SetBevelContrast");
	statlist[114].count++;
    return 0;
}

INT32
GDraw_SetBevelLightness ( pGCONTEXT pContext,UINT32 Contrast )
{
    STUB ("GDraw_SetBevelLightness");
	statlist[115].count++;
    return 0;
}

INT32
GDraw_SetBevelDarkness ( pGCONTEXT pContext,UINT32 Contrast )
{
    STUB ("GDraw_SetBevelDarkness");
	statlist[116].count++;
    return 0;
}

INT32
GDraw_TranslateBevelValue ( pcGCONTEXT pContext, BYTE Index, BYTE Colour )
{
    STUB ("GDraw_TranslateBevelValue");
	statlist[117].count++;
    return 0;
}

INT32
GBitmap_PlotTile (
    pGCONTEXT pContext,
    pcBITMAPINFOHEADER BitmapInfo,
    pcBYTE Bitmap,
    DWORD Style,
    pcPOINT PointA,
    pcPOINT PointB,
    pcPOINT PointC,
    pcBGRT TranslationTable,
    pcBYTE   RedTranslationTable,
    pcBYTE GreenTranslationTable,
    pcBYTE  BlueTranslationTable,
    pcBYTE TransparencyTranslationTable
    )
{
	statlist[118].count++;

	GCDATA 		*data=dp_list[pContext->Data[0]];
	INT32 		width, height;
	double 		a, b, c, d, e, f;
	cairo_t 		*cr=data->cr;
	cairo_matrix_t	matrix;

/*	data->transparency=(~Style & 0x00FF0000) >> 16;*/

	width=BitmapInfo->biWidth;
	height=BitmapInfo->biHeight;

	c=toFL(PointA->x);
	f=toFL(PointA->y);
	cairo_user_to_device(data->cr, &c, &f);

	a=toFL(PointB->x);
	d=toFL(PointB->y);
	cairo_user_to_device(data->cr, &a, &d);
	a=(a - c)/width;
	d=(d - f)/width;

	b=toFL(PointC->x);
	e=toFL(PointC->y);
	cairo_user_to_device(data->cr, &b, &e);
	b=(b - c)/height;
	e=(e - f)/height;

	cairo_matrix_init(&matrix, a, d, b, e, c, f);

	data->surface=cairo_image_surface_create_for_data((unsigned char *)Bitmap,
					CAIRO_FORMAT_RGB24, width, height, width * 4);

	cairo_save(cr);
		cairo_set_matrix(cr, &matrix);
		cairo_set_source_surface(cr, data->surface, 0, 0);
		cairo_paint(cr);
	cairo_restore(cr);

	cairo_surface_destroy(data->surface);

	data->surface=NULL;
	data->source=S_ABSENT;

	return 0;
}

INT32
GBitmap_PlotTile4 (
    pGCONTEXT pContext,
    pcBITMAPINFOHEADER BitmapInfo,
    pcBYTE Bitmap,
    DWORD Style,
    pcPOINT PointA,
    pcPOINT PointB,
    pcPOINT PointC,
    pcPOINT PointD,
    pcBGRT TranslationTable,
    pcBYTE   RedTranslationTable,
    pcBYTE GreenTranslationTable,
    pcBYTE  BlueTranslationTable,
    pcBYTE TransparencyTranslationTable
    )
{
    STUB ("GBitmap_PlotTile4");
	statlist[119].count++;
    return 0;
}
