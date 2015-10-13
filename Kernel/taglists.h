// $Id: taglists.h 1282 2006-06-09 09:46:49Z alex $
// Defines the lists and list items used to manage tags
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

#ifndef INC_TAGLISTS
#define INC_TAGLISTS

#include "cxftags.h"
//#include "list.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "listitem.h" - in camtypes.h [AUTOMATICALLY REMOVED]

/***********************************************************************************************

>	class AtomicTagListItem : public ListItem

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/96
	Purpose:	An item that appears in the AtomicTagList

***********************************************************************************************/

class AtomicTagListItem : public ListItem
{
CC_DECLARE_MEMDUMP(AtomicTagListItem)

public:
	AtomicTagListItem()				{ Tag = TAG_UNDEFINED; }
	AtomicTagListItem(UINT32 ThisTag) { Tag = ThisTag; }

	UINT32 GetTag() { return Tag; }
private:
	UINT32 Tag;
};

/***********************************************************************************************

>	class EssentialTagListItem : public ListItem

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/96
	Purpose:	An item that appears in the EssentialTagList

***********************************************************************************************/

class EssentialTagListItem : public ListItem
{
CC_DECLARE_MEMDUMP(EssentialTagListItem)

public:
	EssentialTagListItem()				{ Tag = TAG_UNDEFINED; }
	EssentialTagListItem(UINT32 ThisTag)	{ Tag = ThisTag; }

	UINT32 GetTag() { return Tag; }
private:
	UINT32 Tag;
};

/***********************************************************************************************

>	class TagDescriptionListItem : public ListItem

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/96
	Purpose:	An item that appears in the TagDescriptionList

***********************************************************************************************/

class TagDescriptionListItem : public ListItem
{
CC_DECLARE_MEMDUMP(TagDescriptionListItem)

public:
	TagDescriptionListItem()									{ Tag = TAG_UNDEFINED; pStr = NULL; }
	TagDescriptionListItem(UINT32 ThisTag, String_256* pThisStr)	{ Tag = ThisTag; pStr = pThisStr; }

	~TagDescriptionListItem()	{ if (pStr != NULL) delete pStr; }
	
	UINT32 GetTag()			{ return Tag; }
	String_256* GetString()	{ return pStr; }

private:
	UINT32		Tag;
	String_256* pStr;
};

/***********************************************************************************************

>	class AtomicTagList : public List

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/96
	Purpose:	Keeps a list of all the atomic tags present in the imported file.

				This only relates to new format files

				An atomic tag is a tag that represents the head of a sub-tree that should be
				taken as a single entity.
				E.g. a TextStory tag is an atomic tag because it, and all it's children, represent
				one object (same applies for mould & blend tags).

				If an importer reads a record that it doesn't understand, and it's been defined as
				an atomic record, then it should ignore the record AND ALL ITS CHILD RECORDS!

***********************************************************************************************/

class AtomicTagList : public List
{
CC_DECLARE_MEMDUMP(AtomicTagList)

public:
	AtomicTagList() {}

	void AddTail(AtomicTagListItem* pItem)				{ List::AddTail(pItem); };
	AtomicTagListItem* GetHead()						{ return (AtomicTagListItem*)List::GetHead(); }
	AtomicTagListItem* GetNext(AtomicTagListItem* pItem){ return (AtomicTagListItem*)List::GetNext(pItem); }
};


/***********************************************************************************************

>	class EssentialTagList : public List

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/96
	Purpose:	Keeps a list of all the essential tags present in the imported file.

				This only relates to new format files

				An essential tag is a tag that the importer must understand in order to successfully
				load the file.

				If the importer does not recognise the tag, and it's defined to be essential, the importer
				should abort the import.

				We have no examples of such a tag.  This mechanism is in place in case we wish to prevent
				old importers loading in newer versions of the file format.

***********************************************************************************************/

class EssentialTagList : public List
{
CC_DECLARE_MEMDUMP(EssentialTagList)

public:
	EssentialTagList() {}

	void AddTail(EssentialTagListItem* pItem)					{ List::AddTail(pItem); };
	EssentialTagListItem* GetHead()								{ return (EssentialTagListItem*)List::GetHead(); }
	EssentialTagListItem* GetNext(EssentialTagListItem* pItem)	{ return (EssentialTagListItem*)List::GetNext(pItem); }
};

/***********************************************************************************************

>	class TagDescriptionList : public List

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/96
	Purpose:	Keeps a list of all the tag descriptions present in the imported file.

				This only relates to new format files

				This will only come into use in newer versions of the file format.  It allows old importers
				to describe an unknown tag to the user.

***********************************************************************************************/

class TagDescriptionList : public List
{
CC_DECLARE_MEMDUMP(TagDescriptionList)

public:
	TagDescriptionList() {}

	void AddTail(TagDescriptionListItem* pItem)						{ List::AddTail(pItem); };
	TagDescriptionListItem* GetHead()								{ return (TagDescriptionListItem*)List::GetHead(); }
	TagDescriptionListItem* GetNext(TagDescriptionListItem* pItem)	{ return (TagDescriptionListItem*)List::GetNext(pItem); }
};

#endif	// INC_TAGLISTS
