// $Id: unitres.h 751 2006-03-31 15:43:49Z alex $
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
// Definition of various units related stuff

#ifndef INC_UNITRES
#define INC_UNITRES

/*---------------------------------------------------------------------------------

	The defines below were calculated using the following facts :

		1 mile = 1.60934 kilometres
		1 mile = 1760 yards

	They define the number of millipoints in each of the default unit types supported by Camelot.

	(MarkN 8/3/94 & 28/6/94)
*/

#define MM_MP_VAL 	2834.652715			// Num millipoints in a millimetre
#define CM_MP_VAL	(MM_MP_VAL*10)		// Num millipoints in a centimetre
#define  M_MP_VAL	(CM_MP_VAL*100)		// Num millipoints in a metre
#define KM_MP_VAL	( M_MP_VAL*1000)	// Num millipoints in a kilometre

#define MP_MP_VAL	1.0					// Num millipoints in a millipoint (well, it's consistant)
#define PT_MP_VAL	(MP_MP_VAL*1000)	// Num millipoints in a computer point
#define PI_MP_VAL  	(PT_MP_VAL*12)		// Num millipoints in a pica
#define IN_MP_VAL	(PI_MP_VAL*6)		// Num millipoints in an inch
#define FT_MP_VAL	(IN_MP_VAL*12)		// Num millipoints in a foot
#define YD_MP_VAL	(FT_MP_VAL*3)		// Num millipoints in a yard
#define MI_MP_VAL	(YD_MP_VAL*1760)	// Num millipoints in a mile
#define PX_MP_VAL 	750.0				// Num millipoints in a pixel (1/96 inch)


// The following #defines echo the values defined in the enum specification in unittype.h.
// They are used in the units.ini file to all the units to specified in a more readable and changable way.
//
// It is important that they have the same value as the enums above. 
// (The program won't start up if they don't due to a validity checking function called DocUnitList::CheckUnitTypesValid())

#define UNIT_NOTYPE 		-1
#define UNIT_AUTOMATIC 		-2
#define UNIT_MILLIMETRES 	0
#define UNIT_CENTIMETRES 	1
#define UNIT_METRES		 	2
#define UNIT_INCHES		 	3
#define UNIT_FEET		 	4
#define UNIT_YARDS		 	5
#define UNIT_COMP_POINTS 	6
#define UNIT_PICAS		 	7
#define UNIT_MILLIPOINTS 	8
#define UNIT_MILES		 	9
#define UNIT_KILOMETRES	 	10
#define UNIT_PIXELS		 	11

#define UNIT_NUM_DEFAULT_TYPES 255

#endif // INC_UNITRES
