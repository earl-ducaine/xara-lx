// $Id: tunemem.h 751 2006-03-31 15:43:49Z alex $
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
// A Limited Memory allocator

#ifndef INC_TUNEMEM
#define INC_TUNEMEM


/********************************************************************************************

>	class TunedMemory : public CC_CLASS_MEMDUMP

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/95
	Purpose:	This is a shell around the CCMalloc and CCFree memory allocator. If you
				use this class to get the memory, the total alloc'ed memory will not
				exceed the amount set in this class. This is used to allow the user to
				say how much memory they are prepared to commit to the off-screen bitmap
				used in rendering. If there is not enough, GRenderRegions will band thier
				output to use up less ram.

********************************************************************************************/

class TunedMemory : public CC_CLASS_MEMDUMP
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(TunedMemory);

public:
	// construction
	TunedMemory();
	~TunedMemory();

	// Functions to get and free memory
	LPVOID LimitedCCMalloc(size_t Size);
	LPVOID LimitedCCRealloc(LPVOID buf, size_t NewSize);
	void LimitedCCFree(LPVOID buf);

	// Functions to find out how much ram is left from the amount available
	size_t GetTotalTunedMem();
	size_t GetAvailableTunedMem();

	// Functions to help comunicate with the Options Dialog
	static size_t GetLimitedMemSize();
	static size_t SetLimitedMemSize(size_t NewSize);
	
	static BOOL IsMemSizeInfinite();
	static void SetMemSizeInfinte(BOOL NewIsInfinite);

	static INT32 GetScreenMemSize();

	static BOOL IsAutomaticMemory();
	static void CalculateRecommendedMemory();

	// Function to read and write the prefs to the ini file
	static BOOL Init();

// member vars
protected:
	// The total is also saved out as a preference
	static BOOL			IsInfinite;
	static size_t		TotalRAM;
	static size_t		AvailableRAM;

	static BOOL			SmartBanding;

	static size_t		LimitRAM;
	static BOOL			RecommendInfinite;
	static INT32			RecommendRAM;
};


#endif  // INC_TUNEME


