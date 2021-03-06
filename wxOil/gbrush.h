// $Id: gbrush.h 1282 2006-06-09 09:46:49Z alex $
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

// class to use GDraw to obtain GDI brush patterns

#ifndef INC_GBRUSH
#define	INC_GBRUSH

//#include "listitem.h" - in camtypes.h [AUTOMATICALLY REMOVED]

class DocColour;
class ColourContext;

/***************************************************************************
>	class GBrush : public ListItem

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Purpose:        A GBrush is a way of getting to Gavin-created
			brushes, when available.  It can supply GDI
			with better dithered brushes than it does
			normally.  Before use, you should call Init
			which tells you whether a GBrush is
			available. If it is not, no further use can be
			made of it. If it is, call Start on it (cv
			StartRender). It can then be used, until such
			time as End is called on it. An in-line member
			function called Available can be used to
			determine its functionality at any time. There
			can only be one active GBrush in use at a
			time.
***************************************************************************/
class GBrush : public ListItem
{
CC_DECLARE_MEMDUMP(GBrush);
  
  // Methods
public:
  GBrush();
  ~GBrush();
  BOOL Init(wxDC* Ref);
  void Start();
  void Stop();
  BOOL Available() {
    return Valid;
  }
  void GetLogBrush(ColourContext *, DocColour &, wxBrush*);
  void GetLogBrush(const COLORREF rgb, wxBrush* pBrush);
  static BOOL InitGBrush(BOOL);
  static void DeinitGBrush(void);
  static void NewBrushState();
  void SetSolidColours(BOOL UseSolid);
  // Enables/disables solid colour override (to remove dithering)
  static void ResetOnFatalError() {
    if (Current) {
      Current->Stop();
    }
  }

  // Methods
private:
  inline void GetLogBrushInternal(const COLORREF rgb, wxBrush* pBrush);

  // Properties
private:  
  // TRUE if bitmap set up
  BOOL Valid;
  // TRUE if will work
  BOOL CanUse;
  // reference device (for palette)
  wxDC* ReferenceDC;
  // contains system palette (256 cols)
  RGBQUAD RGBList[256];		
  // list of all active brushes
  static List BrushList;
  // active brush
  static GBrush *Current;								
  // TRUE = solid, FALSE = dithered colours
  BOOL UseSolidColours;
};

/********************************************************************************************

>	BOOL GBrush::Available()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if can use it, FALSE if cannot.
	Purpose:	Determine whether this GBrush works or not. An in-line function.
	Errors:		-

********************************************************************************************/


#endif
