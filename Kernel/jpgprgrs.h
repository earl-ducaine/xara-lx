// $Id: jpgprgrs.h 809 2006-04-13 11:28:42Z phil $
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

#ifndef INC_JPGPRGRES
#define INC_JPGPRGRES

/**************************************************************************************

>	class JPEGProgressMonitor : public jpeg_progress_mgr

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/96
	Purpose:	Provides progress update callback for the IJG library JPEG Filters

**************************************************************************************/
class JPEGProgressMonitor : public libJPEG::jpeg_progress_mgr 
{
	// Declare the class for memory tracking
	//CC_DECLARE_MEMDUMP(JPEGProgressMonitor);
public:
	// Overridden 'C' function
	static void Update(libJPEG::j_common_ptr cinfo);

	BOOL UserAborted() const	{	return !m_bContinueOp;	}

	virtual ~JPEGProgressMonitor() = 0;

protected:

	BOOL				m_bContinueOp;
	libJPEG::j_decompress_ptr	m_pcinfo;

protected:
	// Support functions
	JPEGProgressMonitor(libJPEG::j_decompress_ptr cinfo);

	// Overridable functions

	virtual void	OnUpdate() = 0;
};


/**************************************************************************************

>	class JPEGProgressBySelf : public JPEGProgressMonitor

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/96
	Purpose:	Provides progress update callback for the IJG library JPEG Filters
				using a self-contained Progress object.

**************************************************************************************/
class JPEGProgressBySelf : public JPEGProgressMonitor
{
	// Declare the class for memory tracking
	//CC_DECLARE_MEMDUMP(JPEGProgressBySelf);
public:

	JPEGProgressBySelf(libJPEG::j_decompress_ptr cinfo, const StringBase& BarString);

	~JPEGProgressBySelf();

protected:
	String_64		m_BarString;
	Progress*		m_pProgressBar;
	UINT32			m_uLastPercentDone;

protected:
	// Overridden functions
	virtual void	OnUpdate();

};


/**************************************************************************************

>	class JPEGProgressFilter : public JPEGProgressMonitor

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/96
	Purpose:	Provides progress update callback for the IJG library JPEG Filters
				using the Update member of the Filter class

**************************************************************************************/
class JPEGProgressByFilter : public JPEGProgressMonitor
{
	// Declare the class for memory tracking
	//CC_DECLARE_MEMDUMP(JPEGProgressByFilter);
public:

	JPEGProgressByFilter(libJPEG::j_decompress_ptr cinfo, Filter* pFilterToUse,
		UINT32 ImportSize);

protected:
	Filter*			m_pFilterForUpdate;
	UINT32			m_uLastScanLineDone;
	UINT32			m_uImportSize;

protected:
	// Overridden functions
	virtual void	OnUpdate();
};

#endif	// INC_JPGPRGRES


