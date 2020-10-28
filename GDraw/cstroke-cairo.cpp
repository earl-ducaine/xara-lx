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
 */

#include <stdio.h>

#include "camtypes.h"
#include "cstroke.h"
#include "gdraw-cairo-private.h"

#define STUB(func) fprintf (stderr, "FIXME: Need a cairo-based implementation of " func " here.\n")

extern struct statnode statlist[];

INT32
GenStroke::StrokePathToPath (
    cpcPOINT	pIPoints,
    cpcBYTE		pITypes,
    UINT32		nILength,
    ppPOINT		ppOPoints,
    ppBYTE		ppOTypes,
    BOOL		bClose,
    UINT32		nLineWidth,
    UINT32		nMitreLimit,
//  UINT32		nFlatness,
    DOUBLE		nFlatness,
    const  CapStyles LineCaps,
    const JoinStyles LineJoin,
    cpcDASH		pDash
    )
{
    STUB ("GenStroke::StrokePathToPath");
	statlist[121].count++;
    return 0;
}

void
GenStroke::Close ()
{
    STUB ("GenStroke::Close");
	statlist[122].count++;
}

INT32
GenPathContour::ContourPath (
    cpcPOINT	pIPoints,
    cpcBYTE		pITypes,
    UINT32		nILength,
    ppPOINT		ppOPoints,
    ppBYTE		ppOTypes,
    UINT32		LineWidth,
    UINT32		MitreLimit,
//  UINT32		nFlatness,
    DOUBLE		nFlatness,
    const JoinStyles LineJoin,
    bool		bOuter
    )
{
    STUB ("GenPathContour::ContourPath");
	statlist[123].count++;
    return 0;
}

INT32
GenBevelFaces::BevelPath (
    cpcPOINT	pIPoints,
    cpcBYTE		pITypes,
    UINT32		ILength,
    ppBEVEL_FACE ppOutputFaces,
    UINT32		LineWidth,
    UINT32		MitreLimit,
//  UINT32		nFlatness,
    DOUBLE		nFlatness,
    const JoinStyles LineJoin,
    bool		bOuter
    )
{
    STUB ("GenBevelFaces::BevelPath");
	statlist[124].count++;
    return 0;
}
