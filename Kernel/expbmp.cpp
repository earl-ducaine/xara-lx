// $Id: expbmp.cpp 1282 2006-06-09 09:46:49Z alex $
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

// Encapsulates a set of bitmaps exported to a file.

/*
*/

#include "camtypes.h"

#include "expbmp.h"
//#include "bitmap.h" - in camtypes.h [AUTOMATICALLY REMOVED]

DECLARE_SOURCE("$Revision: 1282 $");

CC_IMPLEMENT_MEMDUMP(ExportBitmap, ListItem)
CC_IMPLEMENT_MEMDUMP(ExportBitmapList, List)
CC_IMPLEMENT_MEMDUMP(ExportedBitmaps, CCObject)

#define new CAM_DEBUG_NEW

/********************************************************************************************

>	ExportBitmap::ExportBitmap(KernelBitmap *pNewBmp, INT32 NewRecordNumber)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		pNewBmp			- the Bitmap to put in the list.
				RecordNumber	- the number of the record that this Bitmap definition has been
								  saved in.
	Purpose:	Create a new export Bitmap list item from the given indexed Bitmap.
	SeeAlso:	ExportBitmapList;

********************************************************************************************/

ExportBitmap::ExportBitmap(KernelBitmap *pNewBmp, INT32 NewRecordNumber)
{
	ERROR3IF(pNewBmp->HasBeenDeleted(), "Trying to export a deleted bitmap");

	// Initialise our class variables to these specified values.
	pBmp = pNewBmp;
	RecordNumber = NewRecordNumber;
}

/********************************************************************************************

>	ExportBitmapList::~ExportBitmapList()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroy the list of exported Bitmaps.
	SeeAlso:	ExportedBitmaps

********************************************************************************************/

ExportBitmapList::~ExportBitmapList()
{
	// Destroy the list of Bitmaps which we may have
	//ExportBitmap* pExportedBitmap = (ExportBitmap *)GetHead();
	//while (pExportedBitmap)
	//{
	//	delete pExportedBitmap;

		// Try the next Bitmap in the list
//		pExportedBitmap = (ExportBitmap *)Bitmaps.GetNext(pExportedBitmap);
//	}
	
	DeleteAll();
}

/********************************************************************************************

>	ExportBitmap *ExportBitmapList::AddBitmap(KernelBitmap *pBmp, INT32 NewRecordNumber)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		pNewBmp			- the Bitmap to put in the list.
				RecordNumber	- the number of the record that this Bitmap definition has been
								  saved in.
	Returns:	Pointer to the newly added Bitmap, or 
				NULL if out of memory => ERROR1
	Purpose:	Add a new Bitmap to the list of exported Bitmaps.
	Errors:		Out of memory => ERROR1
	SeeAlso:	ExportBitmap

********************************************************************************************/

ExportBitmap *ExportBitmapList::AddBitmap(KernelBitmap *pBmp, INT32 NewRecordNumber)
{
	ERROR2IF(pBmp->HasBeenDeleted(), NULL, "Trying to add a deleted bitmap to the export list");

	ExportBitmap *pNewBmp = new ExportBitmap(pBmp, NewRecordNumber);

	// Check for out of memory
	if (pNewBmp == NULL)
		return NULL;

	// Add to the list and return success
	AddTail(pNewBmp);
	return pNewBmp;
}

/********************************************************************************************

>	ExportedBitmaps::ExportedBitmaps()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Construct the class which encapsulates a list of exported Bitmaps.
	SeeAlso:	ExportedBitmaps

********************************************************************************************/

ExportedBitmaps::ExportedBitmaps()
{
}

/********************************************************************************************

>	ExportedBitmaps::~ExportedBitmaps()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroy the class which encapsulates a list of exported Bitmaps.
	SeeAlso:	ExportedBitmaps

********************************************************************************************/

ExportedBitmaps::~ExportedBitmaps()
{
}

/********************************************************************************************

>	BOOL ExportedBitmaps::AddBitmap(KernelBitmap *pBmp, INT32 RecordNumber)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		pNewBmp			- the Bitmap to put in the list.
				RecordNumber	- the number of the record that this Bitmap definition has been
								  saved in.
	Returns:	TRUE if the Bitmap could be added;
				FALSE if not
	Purpose:	Add the specified Bitmap to the list of already exported Bitmaps.
	Errors:		-
	SeeAlso:	ExportBitmapList;

********************************************************************************************/

BOOL ExportedBitmaps::AddBitmap(KernelBitmap *pBmp, INT32 RecordNumber)
{
	ERROR2IF(pBmp->HasBeenDeleted(), FALSE, "Trying to add a deleted bitmap to the export list");

	// Add the Bitmap onto our list
	ExportBitmap * pNewBmp = Bitmaps.AddBitmap(pBmp, RecordNumber);
	if (pNewBmp)
		return TRUE;
	
	return FALSE;
}

/********************************************************************************************

>	ExportBitmap *ExportedBitmaps::GetBitmap(KernelBitmap *pSearchBmp)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		pSearchBmp			- the Bitmap to search for in the list.
	Returns:	if the item is found then returns		- Pointer to the exported Bitmap list item
				if the item is not found then returns	- NULL.
	Purpose:	Find an indexed Bitmap in the exported list.
	SeeAlso:	ExportBitmapList

********************************************************************************************/

ExportBitmap *ExportedBitmaps::GetBitmap(KernelBitmap *pSearchBmp)
{
	if (pSearchBmp == NULL)
	{
		ERROR3("ExportedBitmaps::GetBitmap - searching for a null indexed Bitmap!");
	}

	ERROR2IF(pSearchBmp->HasBeenDeleted(), NULL, "Why am I looking for a deleted bitmap ?");

	ExportBitmap* pExportedBitmap = (ExportBitmap *)Bitmaps.GetHead();
	while (pExportedBitmap)
	{
		// Compare the indexed Bitmap pointers and return the match if they are the same
		if (pExportedBitmap->pBmp == pSearchBmp)
			return pExportedBitmap;
		
		// Try the next Bitmap in the list
		pExportedBitmap = (ExportBitmap *)Bitmaps.GetNext(pExportedBitmap);
	}

	// No - return failure.
	return NULL;
}


/********************************************************************************************

>	ExportBitmap* ExportedBitmaps::EnumerateBitmaps(ExportBitmap* pExpBmp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/96
	Inputs:		-
	Returns:	if the item is found then returns		- Pointer to the exported Bitmap list item
				if the item is not found then returns	- NULL.
	Purpose:	-
	SeeAlso:	ExportBitmapList

********************************************************************************************/

ExportBitmap* ExportedBitmaps::EnumerateBitmaps(ExportBitmap* pExpBmp)
{
	if (pExpBmp == NULL)
		return (ExportBitmap *)Bitmaps.GetHead();

	return (ExportBitmap *)Bitmaps.GetNext(pExpBmp);
}
