// $Id: xmatrix.h 1285 2006-06-09 11:10:27Z alex $
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

/*
 * */
/***********************************************************************************************

>	class XMatrix

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/5/1993
	Purpose:	Represents a mathematical transformation matrix containing 64-bit values.
				Use instead of Matrix when larger co-ordinates are needed. Based on the
				1.11 build of matrix.h and the 1.5 build of matrix.cpp.
	Errors:		None.

***********************************************************************************************/


#ifndef INC_XMATRIX
#define INC_XMATRIX

#include "ccmaths.h"
#include "wrkcoord.h"
#include "ccobject.h"

class Matrix;

class XMatrix : public SimpleCCObject
{
friend class OSRenderRegion;

private:
	FIXED16 a;
	FIXED16 b;
	FIXED16 c;
	FIXED16 d;
	XLONG e;
	XLONG f;

public:
	XMatrix();
	XMatrix(const FIXED16&, const FIXED16&);
	XMatrix(const ANGLE& theta);
	XMatrix(const WorkCoord&);
	XMatrix(const XLONG x, const XLONG y);
	XMatrix(const INT32 x, const INT32 y);
	XMatrix(const Matrix& );
	XMatrix( const FIXED16& ca,
			 const FIXED16& cb,
			 const FIXED16& cc,
			 const FIXED16& cd,
			 const XLONG    ce,
			 const XLONG    cf
			);

	void transform(WorkCoord*);
	void transform(WorkCoord[], UINT32 count);
	void transform(WorkCoord[], const WorkCoord[], UINT32);

	XMatrix& operator=(const XMatrix&);
	friend XMatrix operator*(const XMatrix&, const XMatrix&);
	XMatrix& operator*=(const XMatrix&);


};


#endif 
