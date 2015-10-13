// $Id: camframe.h 1769 2007-06-17 19:33:43Z alex $
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
#ifndef INC_CAMFRAME
#define INC_CAMFRAME

// Forward definitions
class CCamCanvas;
class wxAuiManager;
class CaptureWnd;

class CCamFrame :
	public wxDocMDIParentFrame
{
public:
	static CCamFrame   *m_pMainFrame;

private:
	DECLARE_CLASS( CCamFrame )
	
	wxMenu			   *m_editMenu;
#if defined(USE_WXAUI)
	wxAuiManager *m_pFrameManager;
#endif
	wxStatusBar*		m_pStatusBar;

	bool				m_ChangingViewMode;

	wxIcon				m_iconAppIcon;

	wxRect m_WndRect;		// Used to remember the unmin/maximised position

public:
	CCamFrame( wxDocManager *manager, wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size,
		   long type ); /* TYPENOTE: Correct */
	~CCamFrame();

	static CCamFrame * GetMainFrame() {return m_pMainFrame;}
	static wxAuiManager * GetFrameManager() { return m_pMainFrame?m_pMainFrame->m_pFrameManager:NULL;}
	
	wxStatusBar* GetStatusBar() const 	{ return m_pStatusBar; }

	BOOL CreateToolbars();

	void UpdateFrameManager();

    /// Creation
    bool Create( wxWindow *parent, wxWindowID id = -1, const wxString &caption = wxT("wxTemplate"), 
		const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
		long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL );

	bool IsChangingViewMode()				{ return m_ChangingViewMode;};
	void SetChangingMode(bool fSetOrClear)	{ m_ChangingViewMode = fSetOrClear;};

    /// Creates the controls and sizers
    void CreateControls();

	void UpdateWndSize();

#if !defined(USE_WXAUI)
    void OnSize(wxSizeEvent &event);
    void OnMove(wxMoveEvent &event);
#endif
	void OnCloseWindow(wxCloseEvent& event);

	void OnMenuCommand			( wxCommandEvent& event );

	void OnZoomIn				( wxCommandEvent& event );
	void OnZoomOut				( wxCommandEvent& event );
	void OnZoomDrawing			( wxCommandEvent& event );
	void OnZoomPage				( wxCommandEvent& event );
	void OnZoomPrevious			( wxCommandEvent& event );
	void OnQualityOutline		( wxCommandEvent& event );
	void OnQualitySimple		( wxCommandEvent& event );
	void OnQualityNormal		( wxCommandEvent& event );
	void OnQualityAntialiased	( wxCommandEvent& event );
	void OnBackgroundRender		( wxCommandEvent& event );
	void OnCaching				( wxCommandEvent& event );
	void OnDoubleBuffer			( wxCommandEvent& event );
	void OnTimeRedraw			( wxCommandEvent& event );
//#if defined(_DEBUG)
	void OnBlobbyDlg			( wxCommandEvent& event );
	void OnBlobbyBar			( wxCommandEvent& event );
	void OnTreeView				( wxCommandEvent& event );
//#endif
	void OnStandardBar			( wxCommandEvent& event );
	
	void OnInitMenuPopup		( wxMenuEvent& event );

	void OnUpdateStandardBar		( wxUpdateUIEvent& event );
	void OnUpdateZoomPrevious		( wxUpdateUIEvent& event );
	void OnUpdateQualityOutline		( wxUpdateUIEvent& event );
	void OnUpdateQualitySimple		( wxUpdateUIEvent& event );
	void OnUpdateQualityNormal		( wxUpdateUIEvent& event );
	void OnUpdateQualityAntialiased	( wxUpdateUIEvent& event );
	void OnUpdateBackgroundRender	( wxUpdateUIEvent& event );
	void OnUpdateCaching			( wxUpdateUIEvent& event );
	void OnUpdateDoubleBuffer		( wxUpdateUIEvent& event );

    /// Should we show tooltips?
    static bool ShowToolTips();

	virtual bool Destroy();

	DECLARE_EVENT_TABLE()
};

extern CCamFrame *GetMainFrame();

#endif
