// $Id: hardwaremanager.h 1696 2006-08-13 10:21:04Z alex $
/* @@tag:xara-cn@@ DO NOT MODIFY THIS LINE
================================XARAHEADERSTART===========================
 
               Xara LX, a vector drawing and manipulation program.
                    Copyright (C) 1993-2006 Xara Group Ltd.
       Copyright on certain contributions may be held in joint with their
              respective authors. See AUTHORS file for details.

LICENSE TO USE AND MODIFY SOFTWARE
----------------------------------

This file is part of Xara LX.

Xara LX is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License version 2 as published
by the Free Software Foundation.

Xara LX and its component source files are distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with Xara LX (see the file GPL in the root directory of the
distribution); if not, write to the Free Software Foundation, Inc., 51
Franklin St, Fifth Floor, Boston, MA  02110-1301 USA


ADDITIONAL RIGHTS
-----------------

Conditional upon your continuing compliance with the GNU General Public
License described above, Xara Group Ltd grants to you certain additional
rights. 

The additional rights are to use, modify, and distribute the software
together with the wxWidgets library, the wxXtra library, and the "CDraw"
library and any other such library that any version of Xara LX relased
by Xara Group Ltd requires in order to compile and execute, including
the static linking of that library to XaraLX. In the case of the
"CDraw" library, you may satisfy obligation under the GNU General Public
License to provide source code by providing a binary copy of the library
concerned and a copy of the license accompanying it.

Nothing in this section restricts any of the rights you have under
the GNU General Public License.


SCOPE OF LICENSE
----------------

This license applies to this program (XaraLX) and its constituent source
files only, and does not necessarily apply to other Xara products which may
in part share the same code base, and are subject to their own licensing
terms.

This license does not apply to files in the wxXtra directory, which
are built into a separate library, and are subject to the wxWindows
license contained within that directory in the file "WXXTRA-LICENSE".

This license does not apply to the binary libraries (if any) within
the "libs" directory, which are subject to a separate license contained
within that directory in the file "LIBS-LICENSE".


ARRANGEMENTS FOR CONTRIBUTION OF MODIFICATIONS
----------------------------------------------

Subject to the terms of the GNU Public License (see above), you are
free to do whatever you like with your modifications. However, you may
(at your option) wish contribute them to Xara's source tree. You can
find details of how to do this at:
  http://www.xaraxtreme.org/developers/

Prior to contributing your modifications, you will need to complete our
contributor agreement. This can be found at:
  http://www.xaraxtreme.org/developers/contribute/

Please note that Xara will not accept modifications which modify any of
the text between the start and end of this header (marked
XARAHEADERSTART and XARAHEADEREND).


MARKS
-----

Xara, Xara LX, Xara X, Xara X/Xtreme, Xara Xtreme, the Xtreme and Xara
designs are registered or unregistered trademarks, design-marks, and/or
service marks of Xara Group Ltd. All rights in these marks are reserved.


      Xara Group Ltd, Gaddesden Place, Hemel Hempstead, HP2 6EX, UK.
                        http://www.xara.com/

=================================XARAHEADEREND============================
 */
#ifndef INC_HWMAN
#define INC_HWMAN


/*******************************************************************************
>	namespace oilHardwareManager

	Author:		Phil Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/2005
	Purpose:	Provide the kernel with an invariant API to handle differences
				in hardware (primarilly Byte ordering)
	Notes:		LE means "Little-Endian" (e.g. Intel)
				BE means "Big-Endian" (e.g. PowerPC)
				Native means whichever is the native byte ordering on the current
				platform

*******************************************************************************/

namespace oilHardwareManager
{
	// Avoid aliasing problems
	union DoubleUnion
	{
		double u_double;
		INT64 u_INT64;
	};

	union FloatUnion
	{
		float u_float;
		INT32 u_INT32;
	};

// Byte ordering functions
#if defined(WORDS_BIGENDIAN)
	// __BIG_ENDIAN__
	// Big-Endian hardware, signed
	static inline INT16 BEtoNative(INT16 n) {return n;}
	static inline INT32 BEtoNative(INT32 n) {return n;}
	static inline INT64 BEtoNative(INT64 n) {return n;}

	static inline INT16 LEtoNative(INT16 n) {return wxINT16_SWAP_ALWAYS(n);}
	static inline INT32 LEtoNative(INT32 n) {return wxINT32_SWAP_ALWAYS(n);}
	static inline INT64 LEtoNative(INT64 n) {return wxINT64_SWAP_ALWAYS(n);}

	static inline INT16 NativetoBE(INT16 n) {return n;}
	static inline INT32 NativetoBE(INT32 n) {return n;}
	static inline INT64 NativetoBE(INT64 n) {return n;}

	static inline INT16 NativetoLE(INT16 n) {return wxINT16_SWAP_ALWAYS(n);}
	static inline INT32 NativetoLE(INT32 n) {return wxINT32_SWAP_ALWAYS(n);}
	static inline INT64 NativetoLE(INT64 n) {return wxINT64_SWAP_ALWAYS(n);}

	// Big-Endian hardware, unsigned
	static inline UINT16 BEtoNative(UINT16 n) {return n;}
	static inline UINT32 BEtoNative(UINT32 n) {return n;}
	static inline UINT64 BEtoNative(UINT64 n) {return n;}

	static inline UINT16 LEtoNative(UINT16 n) {return wxUINT16_SWAP_ALWAYS(n);}
	static inline UINT32 LEtoNative(UINT32 n) {return wxUINT32_SWAP_ALWAYS(n);}
	static inline UINT64 LEtoNative(UINT64 n) {return wxUINT64_SWAP_ALWAYS(n);}

	static inline UINT16 NativetoBE(UINT16 n) {return n;}
	static inline UINT32 NativetoBE(UINT32 n) {return n;}
	static inline UINT64 NativetoBE(UINT64 n) {return n;}

	static inline UINT16 NativetoLE(UINT16 n) {return wxUINT16_SWAP_ALWAYS(n);}
	static inline UINT32 NativetoLE(UINT32 n) {return wxUINT32_SWAP_ALWAYS(n);}
	static inline UINT64 NativetoLE(UINT64 n) {return wxUINT64_SWAP_ALWAYS(n);}
	
	// Big-endian, special types
	static inline FIXED16 BEtoNative(FIXED16 n) {return n;}
	static inline FIXED16 LEtoNative(FIXED16 n) {return FIXED16::FromRawLong(wxINT32_SWAP_ALWAYS(n.GetRawLong()));}
	static inline FIXED16 NativetoBE(FIXED16 n) {return n;}
	static inline FIXED16 NativetoLE(FIXED16 n) {return FIXED16::FromRawLong(wxINT32_SWAP_ALWAYS(n.GetRawLong()));}

	static inline float BEtoNative(FloatUnion n) {return n.u_float;}
	static inline float LEtoNative(FloatUnion n) {n.u_INT32 = wxINT32_SWAP_ALWAYS(n.u_INT32); return n.u_float;}
	static inline FloatUnion NativetoBEU(float n) {FloatUnion f; f.u_float=n; return f;}
	static inline FloatUnion NativetoLEU(float n) {FloatUnion f; f.u_float=n; f.u_INT32 = wxINT32_SWAP_ALWAYS(f.u_INT32); return f;}

	static inline double BEtoNative(DoubleUnion n) {return n.u_double;}
	static inline double LEtoNative(DoubleUnion n) {n.u_INT64 = wxINT64_SWAP_ALWAYS(n.u_INT64); return n.u_double;}
	static inline DoubleUnion NativetoBEU(double n) {DoubleUnion f; f.u_double=n; return f;}
	static inline DoubleUnion NativetoLEU(double n) {DoubleUnion f; f.u_double=n; f.u_INT64 = wxINT64_SWAP_ALWAYS(f.u_INT64); return f;}

#else
	// __LITTLE_ENDIAN__
	// Little-Endian, signed
	static inline INT16 BEtoNative(INT16 n) {return wxINT16_SWAP_ALWAYS(n);}
	static inline INT32 BEtoNative(INT32 n) {return wxINT32_SWAP_ALWAYS(n);}
	static inline INT64 BEtoNative(INT64 n) {return wxINT64_SWAP_ALWAYS(n);}

	static inline INT16 LEtoNative(INT16 n) {return n;}
	static inline INT32 LEtoNative(INT32 n) {return n;}
	static inline INT64 LEtoNative(INT64 n) {return n;}

	static inline INT16 NativetoBE(INT16 n) {return wxINT16_SWAP_ALWAYS(n);}
	static inline INT32 NativetoBE(INT32 n) {return wxINT32_SWAP_ALWAYS(n);}
	static inline INT64 NativetoBE(INT64 n) {return wxINT64_SWAP_ALWAYS(n);}

	static inline INT16 NativetoLE(INT16 n) {return n;}
	static inline INT32 NativetoLE(INT32 n) {return n;}
	static inline INT64 NativetoLE(INT64 n) {return n;}

	// Little-Endian, unsigned
	static inline UINT16 BEtoNative(UINT16 n) {return wxUINT16_SWAP_ALWAYS(n);}
	static inline UINT32 BEtoNative(UINT32 n) {return wxUINT32_SWAP_ALWAYS(n);}
	static inline UINT64 BEtoNative(UINT64 n) {return wxUINT64_SWAP_ALWAYS(n);}

	static inline UINT16 LEtoNative(UINT16 n) {return n;}
	static inline UINT32 LEtoNative(UINT32 n) {return n;}
	static inline UINT64 LEtoNative(UINT64 n) {return n;}

	static inline UINT16 NativetoBE(UINT16 n) {return wxUINT16_SWAP_ALWAYS(n);}
	static inline UINT32 NativetoBE(UINT32 n) {return wxUINT32_SWAP_ALWAYS(n);}
	static inline UINT64 NativetoBE(UINT64 n) {return wxUINT64_SWAP_ALWAYS(n);}

	static inline UINT16 NativetoLE(UINT16 n) {return n;}
	static inline UINT32 NativetoLE(UINT32 n) {return n;}
	static inline UINT64 NativetoLE(UINT64 n) {return n;}

	// Little-endian, special types
	static inline FIXED16 BEtoNative(FIXED16 n) {return FIXED16::FromRawLong(wxINT32_SWAP_ALWAYS(n.GetRawLong()));}
	static inline FIXED16 LEtoNative(FIXED16 n) {return n;}
	static inline FIXED16 NativetoBE(FIXED16 n) {return FIXED16::FromRawLong(wxINT32_SWAP_ALWAYS(n.GetRawLong()));}
	static inline FIXED16 NativetoLE(FIXED16 n) {return n;}

	static inline float BEtoNative(FloatUnion n) {n.u_INT32 = wxINT32_SWAP_ALWAYS(n.u_INT32); return n.u_float;}
	static inline float LEtoNative(FloatUnion n) {return n.u_float;}
	static inline FloatUnion NativetoBEU(float n) {FloatUnion f; f.u_float=n; f.u_INT32 = wxINT32_SWAP_ALWAYS(f.u_INT32); return f;}
	static inline FloatUnion NativetoLEU(float n) {FloatUnion f; f.u_float=n; return f;}

	static inline double BEtoNative(DoubleUnion n) {n.u_INT64 = wxINT64_SWAP_ALWAYS(n.u_INT64); return n.u_double;}
	static inline double LEtoNative(DoubleUnion n) {return n.u_double;}
	static inline DoubleUnion NativetoBEU(double n) {DoubleUnion f; f.u_double=n; f.u_INT64 = wxINT64_SWAP_ALWAYS(f.u_INT64); return f;}
	static inline DoubleUnion NativetoLEU(double n) {DoubleUnion f; f.u_double=n; return f;}
#endif

// -------------------------------------------------------------------------------
// Native Unicode character width handling (XAR files always use UTF16)
// Useful reference: http://en.wikipedia.org/wiki/Comparison_of_Unicode_encodings
//
PORTNOTE("WCHARSize", "Check conversion from UTF32 to UTF16 and vice-versa");
	static inline WCHAR NativeToUTF16(WCHAR n) {return n;}		// Just truncate at the moment
	static inline WCHAR UTF16ToNative(WCHAR n) {return n;}		// Just truncate at the moment

};

#endif	// INC_HWMAN
