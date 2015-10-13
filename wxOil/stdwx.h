// $Id: stdwx.h 1589 2006-07-28 20:53:48Z alex $
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

// First the main include file
#include <wx/wx.h>

// Now specialized bits we might not have got through the above
#include <wx/bitmap.h>
#include <wx/cmdline.h>
#include <wx/cmdproc.h>
#include <wx/config.h>
#include <wx/cshelp.h>
#include <wx/dcbuffer.h>
#include <wx/debugrpt.h>
#include <wx/dcps.h>
#include <wx/display.h>
#include <wx/dir.h>
#include <wx/docmdi.h>
#include <wx/docview.h>
#include <wx/event.h>
#include <wx/except.h>
#include <wx/fileconf.h>
#include <wx/file.h>
#include <wx/filedlg.h>
#include <wx/filesys.h>
#include <wx/fontenum.h>
#include <wx/fs_mem.h>
#include <wx/fs_zip.h>
#include <wx/hashmap.h>
#include <wx/image.h>
#include <wx/imagpng.h>
#include <wx/ipc.h>
#include <wx/log.h>
#include <wx/rawbmp.h>
#include <wx/snglinst.h>
#include <wx/splash.h>
#include <wx/stdpaths.h>
#include <wx/sysopt.h>
#include <wx/tokenzr.h>
#include <wx/tooltip.h>
#include <wx/treectrl.h>
#include <wx/wxchar.h>
#include <wx/wxprec.h>
#include <wx/propdlg.h>
#include <wx/process.h>
#include <wx/xrc/xh_all.h>
#include <wx/xrc/xmlres.h>
#if wxUSE_LISTBOOK
#include <wx/listbook.h>
#endif
#if wxUSE_CHOICEBOOK
#include <wx/choicebk.h>
#endif
#if wxUSE_TREEBOOK
#include <wx/treebook.h>
#endif
#if wxUSE_TOOLBOOK
#include <wx/toolbook.h>
#endif
#if wxUSE_LIBGNOMEPRINT
#include <wx/gtk/gnome/gprint.h>
#endif

// Now platform dependent stuff
// FreeBSD does not provide backtrace()
#if !defined(__WXMAC__) && !defined(__FreeBSD__)
#include <wx/stackwalk.h>
#endif

#if defined(__WXMSW__)
#include <wx/msw/regconf.h>
#endif

#if !defined(EXCLUDE_FROM_XARLIB)
#include "wxXtra.h"
#endif

#include <memory>
#include <map>
#include <list>
#include <vector>
#include <set>
#include <iostream>
#include <sstream>
#include <new>
#if !defined(__WXMAC__)
#include <malloc.h>
#else
#include <malloc/malloc.h>
#endif

#if !defined(EXCLUDE_FROM_XARLIB)
#include <libxml/tree.h>
#include <libxml/parser.h>
#endif

#if wxUSE_DEBUGREPORT && wxUSE_XML
#define HAVE_DEBUGREPORT
#else
#undef HAVE_DEBUGREPORT
#endif

template< typename T >
T Abs( T val )
{
	return val < 0 ? -val : val;
}

#ifndef ABS
#define ABS(a) ( ((a) < 0) ? -(a) : (a) )
#endif

#if defined(__WXMSW__)
// min and max macros - only MSW because they blow up some of the headers
#define max(a,b)  (((a) > (b)) ? (a) : (b))
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif

#define camMax(a,b)  (((a) > (b)) ? (a) : (b))
#define camMin(a,b)  (((a) < (b)) ? (a) : (b))

// Defines and types moved from awindows.h

#define nobreak

// Defines and types moved from oiltypes.h

#define CAMAPI
#define CCAPI

#define DECLARE_SOURCE(String)
#define BASED_CODE

#include "compatdef.h"
#include "ensure.h"
#include "errors.h"
#include "memdebug.h"
#include "exception.h"
#include "mfccontainer.h"

#if !defined(EXCLUDE_FROM_XARLIB)
#include "binreloc.h"
#endif
