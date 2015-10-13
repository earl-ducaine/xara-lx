// $Id: impstr.cpp 1282 2006-06-09 09:46:49Z alex $
// Implements a class used when importing string records in the v2 file format
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

#include "camtypes.h"
#include "impstr.h"

#include "cxftags.h"
//#include "cxfrec.h" - in camtypes.h [AUTOMATICALLY REMOVED]

CC_IMPLEMENT_DYNCREATE(ImportedString,	ListItem);
CC_IMPLEMENT_DYNAMIC(ImportedStringList,List);

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

/********************************************************************************************

>	ImportedString::ImportedString(TextChar* pChar,CXaraFileRecord* pRecord)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Inputs:		pChar   = ptr to a char node in the tree
				pRecord = ptr to string record that's been imported
	Returns:	-
	Purpose:	The constructor.

				pChar should point to a TextChar that's been inserted into the tree that contains
				the first character in the string.

				pRecord should point to the string record imported from the file.

				This constructor keeps a copy of the pChar address.
				It makes a copy of pRecord, so it's safe to delete pRecord after this call

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ImportedString::ImportedString(TextChar* pChar,CXaraFileRecord* pRecord)
{
	ERROR3IF(pChar   == NULL,"pChar is NULL");
	ERROR3IF(pRecord == NULL,"pRecord is NULL");

	pFirstChar = pChar;
	pCopyOfStringRecord = NULL;

	if (pRecord != NULL)
	{
		pCopyOfStringRecord = pRecord->GetCopy();

		ERROR3IF(pRecord->GetTag() != TAG_TEXT_STRING,"The record is not a string record");
	}	
}

/********************************************************************************************

>	ImportedString::~ImportedString()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Inputs:		-
	Returns:	-
	Purpose:	The destructor
				It frees up the copy of the record it created at construction time.

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ImportedString::~ImportedString()
{
	if (pCopyOfStringRecord != NULL)
		delete pCopyOfStringRecord;
}

/********************************************************************************************

>	CXaraFileRecord* ImportedString::GetRecord()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Inputs:		-
	Returns:	ptr to the string record
	Purpose:	Returns a ptr to the copy of the string record passed into the constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

CXaraFileRecord* ImportedString::GetRecord()
{
	return pCopyOfStringRecord;
}

/********************************************************************************************

>	TextChar* ImportedString::GetFirstChar()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Inputs:		-
	Returns:	ptr to the text char
	Purpose:	Returns a ptr to the text char passed into the constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

TextChar* ImportedString::GetFirstChar()
{
	return pFirstChar;
}

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------

/********************************************************************************************

>	void ImportedStringList::AddTail(ImportedString* pItem)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Inputs:		pItem = ptr to the ImportedString object
	Returns:	-
	Purpose:	Adds the string item to the list
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void ImportedStringList::AddTail(ImportedString* pItem)
{
	List::AddTail(pItem);
}

/********************************************************************************************

>	ImportedString* ImportedStringList::GetHead()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Inputs:		-
	Returns:	ptr to the head of the list
	Purpose:	Returns a ptr to the head of the list
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ImportedString* ImportedStringList::GetHead()
{
	return (ImportedString*)List::GetHead();
}

/********************************************************************************************

>	ImportedString* ImportedStringList::GetNext(ImportedString* pItem)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Inputs:		pItem = ptr to a ImportedString object in the list
	Returns:	ptr to the next itme in the list
	Purpose:	Returns a ptr to the next itme in the list
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ImportedString* ImportedStringList::GetNext(ImportedString* pItem)
{
	return (ImportedString*)List::GetNext(pItem);
}
