// $Id: prnprefs.h 1362 2006-06-25 17:49:11Z alex $
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
// 
// Header file for PrnPrefs.cpp
//
// 

//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER

#ifndef INC_PRINTPREFS
#define INC_PRINTPREFS

//#include "dialogop.h" - in camtypes.h [AUTOMATICALLY REMOVED]

/********************************************************************************************

>	class PrintPrefsDlg : public DialogTabOp

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>	
	Created:	12/4/95
	Purpose:	The print preference's dialog box.

				Allows the user to set options like:-
				
				Uses the OptionsTabs class declared in AppPrefsDlg and
				any print options pages which have been declared to the
				options tabs list. The application options uses the same system.

	SeeAlso:	AppPrefsDlg; OptionsTabs;
	
********************************************************************************************/
#define OPTOKEN_PRINTOPTIONS	_T("PrintOptions")

class PrintPrefsDlg : public DialogTabOp
{
	CC_DECLARE_DYNCREATE( PrintPrefsDlg )

public:
	PrintPrefsDlg();									// constructor
	PrintPrefsDlg(INT32 StartingPage);					// constructor
	~PrintPrefsDlg();									// ~destructor

	void Do(OpDescriptor*);								// "Do" function

	static BOOL Init();								// Setup function
	static void Deinit();							// Remove function

	// No create function as this is handled by the main tab dialog code

	static OpState GetState(String_256*, OpDescriptor*);
													// Return the state of this operation
	virtual MsgResult Message(Msg* Message);		// Message handler
	virtual BOOL RegisterYourPagesInOrderPlease();	// Add in tab pages
	
	static BOOL InvokeDialog();						// Calls the DoWithParam

	static void SetCurrentDlg(PrintPrefsDlg* pDlg);	// Access to the current open dlg
	static void CloseCurrentDlg();					// Forces the dlg to close

	BOOL HasImages() {return TRUE;}
	TabType GetTabType() {return TABTYPE_TREE/*TABTYPE_LIST*/;}

private:
	// Set up explicit details about this dialog box such as the id and type
	static const CDlgResID IDD;				// Dialog box id
	static const CDlgMode Mode;				// Dialog box mode of operation (mode or modeless)

	// Functions to commit the values on the dialog box and its sections on ok
	BOOL CommitDialogValues();

	String_256 TitleString;					// title of dialog box

	// Parameter passing variables
	static BOOL				OkUsed;					// remember whether ok or cancel used		
	static BOOL				Open;					// Are we open?
	static PrintPrefsDlg*	pCurrentDlg;			// ptr to current print pref dlg that's open
};

/********************************************************************************************

>	class PrintTabPrintPrefsDlg : public PrintPrefsDlg

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>	
	Created:	13/2/96
	Purpose:	To open up the print preference's dialog box with the print tab
				selected. This is used by the units button on the tool bars.

	SeeAlso:	AppPrefsDlg; OptionsTabs; PrintPrefsDlg;
	
********************************************************************************************/

class PrintTabPrintPrefsDlg : public PrintPrefsDlg
{
// Defines the OPTOKEN_PRINTOPTIONSDLG as this now links to the units tab in
// this document options dialog box.
#define OPTOKEN_PRINTOPTIONSDLG	_T("PrintOptionsDlg")

	CC_DECLARE_DYNCREATE( PrintTabPrintPrefsDlg )

public:
	PrintTabPrintPrefsDlg();					// constructor

	// All other functionality comes from the PrintPrefsDlg class.
};

#endif  // INC_PRINTPREFS

#endif //webster
