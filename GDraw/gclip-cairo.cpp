 /* gdraw-cairo.c - A lame attempt to implement GDraw interface with cairo
 *
 * Copyright © 2007 Carl Worth
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
 *
 */

#include <stdio.h>

#include "camtypes.h"
#include "gdraw.h"
#include "gdraw-cairo-private.h"


/*#define XCL_DEBUG(x)*/
/*#define STUB(func)*/
#define XCL_DEBUG(x) fprintf(stderr, x)
#define STUB(func) fprintf (stderr, "FIXME: Need a cairo-based implementation of " func " here.\n")

extern struct statnode statlist[];
/*
	Comment from ...Kernel/Paths.cpp


'Flags' controls how Src will be clipped to this path (taken from gclip.h - complaints go to Gavin)
	CLIPPING_STYLE		- Clip style
	001					- 	  Source AND NOT Clip (subtract Clip from Source)
	010					-     Source AND     Clip (Intersection)
	011					-     Source
	100					- NOT Source AND     Clip (subtract Source from Clip)
	101					-     Source EOR	 Clip
	110					-	  				 Clip
	111					-     Source OR      Clip (Union)
		- If stroked then
	000					- Source AND     Clip (Intersection)
	???					- Source AND NOT Clip
	CLIPPING_SOURCE_WINDING - Source path winding.
	CLIPPING_CLIP_WINDING	- Clip path winding.
	CLIPPING_IS_STROKED		- Source is stroked, not filled
	CLIPPING_IS_CLOSED		- Source path is to be closed (only applicable if stroked).
*/


INT32 ClipPathToPath( 
		PPOINT	SPoints,
		PBYTE	STypes,
		UINT32	SLength,
//		UINT32	SFlatness,
		double	SFlatness,
		PPOINT	CPoints,
		PBYTE	CTypes,
		UINT32	CLength,
//		UINT32	CFlatness,
		double	CFlatness,
		UINT32	Flags,
		UINT32	Tolerance,
		PPOINT	OPoints,
		PBYTE	OTypes,
		UINT32	OMaxLength
	)
{
	STUB ("ClipPathToPath");
	/*XCL_DEBUG("ClipPathToPath\n");*/
	statlist[120].count++;


	printf(" Flags : %d ; Tolerance : %d \n", Flags, Tolerance);
	printf(" OMaxLength : %d\n", OMaxLength);

	UINT32 c;

	printf("Clip     length : %d ; Clip Flatness : %f\n", CLength, CFlatness);
	for(c=0; c < CLength; c++)
	{
		printf("%d, %12d, %12d\n", CTypes[c], CPoints[c].x, CPoints[c].y);
	}
	printf("Source length : %d ; Source Flatness : %f\n", SLength, SFlatness);
	for(c=0; c < SLength; c++)
	{
		printf("%d, %12d, %12d\n", STypes[c], SPoints[c].x, SPoints[c].y);
	}

	return 0;
}

