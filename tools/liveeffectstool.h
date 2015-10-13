// $Id: liveeffectstool.h 1282 2006-06-09 09:46:49Z alex $
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
// The FreeHand Tool
// Created by Phil on 27/09/2004



#ifndef INC_LIVEEFFECTSTOOL
#define INC_LIVEEFFECTSTOOL


//#include "tool.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "selop.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "doccoord.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "wrkrect.h"
//#include "binds.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "dragtool.h"


class Cursor;
class LiveEffectsInfoBarOp;
class Progress;
class Trans2DMatrix;
class BlendRef;
class EffectsStack;


/********************************************************************************************
>	class LiveEffectsTool : public Tool_v1

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/09/2004
	Purpose:	This class implements the Live Effects tool that allows the use to apply
				live bitmap effects to objects.

********************************************************************************************/

class CCAPI LiveEffectsTool : public DragTool
{
// Give my name in memory dumps
CC_DECLARE_MEMDUMP(LiveEffectsTool);

public:
							LiveEffectsTool();
							~LiveEffectsTool();

	BOOL					Init();
	void					Describe(void* InfoPtr);
	UINT32					GetID();
	
	// Event Handlers
	virtual void			SelectChange(BOOL);
	virtual void			OnClick( DocCoord, ClickType, ClickModifiers, Spread* );
	virtual void			OnMouseMove(DocCoord, Spread*, ClickModifiers);
	virtual BOOL			GetStatusLineText(String_256* ptext, Spread*, DocCoord, ClickModifiers);
	virtual BOOL			OnIdle();
	// Rendering the blobs functions
//	virtual void			RenderToolBlobs(Spread*, DocRect*);

	LiveEffectsInfoBarOp*	GetInfoBar() const { return m_pInfoBarOp;}
	void					ScreenChanged(BOOL WipeRegion);
	EffectsStack*			GetEffectsStack() const;
	String_256				GetCurrentEffectID() const {return m_strCurrentEffectID;}
	void					SetCurrentEffectID(String_256 strNewEffectID) {m_strCurrentEffectID = strNewEffectID;}
	INT32						GetCurrentStackPos() {return m_iCurrentStackPos;}
	void					SetCurrentStackPos(INT32 iNewPos) {m_iCurrentStackPos = iNewPos;}
	BOOL					UpdatePPStack(BOOL bUCE = TRUE);

protected:
	// Helper functions
	// Functions to load, remove and change the active cursor
	BOOL					LoadCursors();
	void					RemoveCursors();
	void					ChangeCursor(Cursor* pCursor);

protected:
	static char*			FamilyName;	// The Tools Family Name
	static char*			ToolName;		// The Tool Name
	static char*			Purpose;		// What the tool is for
	static char*			Author;		// Who wrote it

	// The status line text
	String_256				StatusMsg;

	// The coord of the initial click
	DocCoord				StartPos;
	Spread*					StartSpread;

	// The tools cursor
	Cursor*					pNormalCursor;				// The normal cursor
	Cursor*					pActiveCursor;				// Whichever cursor is being displayed (see ChangeCursor).
	INT32					CurrentCursorID;				// ID of the current cursor on the stack

	// The info bar
	LiveEffectsInfoBarOp*	m_pInfoBarOp;

	// Lists of pointers to nodes that we use to run the UI and Ops
	EffectsStack*			m_pPPStack;

	// The name of the current effect we are reflecting/applying
	String_256				m_strCurrentEffectID;
	INT32						m_iCurrentStackPos;

};


#endif  //INC_LIVEEFFECTSTOOL



