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
#include "gdraw2.h"
#include "gdraw-cairo-private.h"

#define STUB(func) fprintf (stderr, "FIXME: Need a cairo-based implementation of " func " here.\n")

extern struct statnode statlist[];

INT32
GDraw2_SetDIBitmap (
    CONST BITMAPINFOHEADER* pBitmapInfo,
    CONST BYTE* pBitmap,
    eBevelStyle nBevelStyle,
    float LightAngle1,
    float LightAngle2
    )
{
    STUB ("GDraw2_SetDIBitmap");
	statlist[125].count++;
    return 0;
}

INT32
GDraw2_FillTriangle (
    CONST POINT aPoints[3],
    double fNormalX,
    double fNormalY
    )
{
    STUB ("GDraw2_FillTriangle");
	statlist[126].count++;
    return 0;
}

INT32
GDraw2_FillTrapezium (
    CONST POINT aPoints[4],
    double fNormalX,
    double fNormalY
    )
{
    STUB ("GDraw2_FillTrapezium");
	statlist[127].count++;
    return 0;
}
