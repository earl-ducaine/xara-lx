///////////////////////////////////////////////////////////////////////////////
// Name:        src/aui/framemanager.cpp
// Purpose:     wxaui: wx advanced user interface - docking window manager
// Author:      Benjamin I. Williams
// Modified by:
// Created:     2005-05-17
// RCS-ID:      $Id: framemanager.cpp,v 1.17 2006/07/05 16:39:04 BIW Exp $
// Copyright:   (C) Copyright 2005-2006, Kirix Corporation, All Rights Reserved
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "framemanager.h"
#include "dockart.h"
#include "floatpane.h"

#if wxXTRA_AUI

#include <wx/settings.h>
#include <wx/app.h>
#include <wx/dcclient.h>
#include <wx/dcscreen.h>
#include <wx/toolbar.h>
#include <wx/mdi.h>
#include <wx/image.h>

#include <wx/arrimpl.cpp>
WX_DECLARE_OBJARRAY(wxRect, wxAuiRectArray);
WX_DEFINE_OBJARRAY(wxAuiRectArray)
WX_DEFINE_OBJARRAY(wxAuiDockUIPartArray)
WX_DEFINE_OBJARRAY(wxAuiDockInfoArray)
WX_DEFINE_OBJARRAY(wxAuiPaneButtonArray)
WX_DEFINE_OBJARRAY(wxAuiPaneInfoArray)

wxAuiPaneInfo wxAuiNullPaneInfo;
wxAuiDockInfo wxAuiNullDockInfo;
DEFINE_EVENT_TYPE(wxEVT_AUI_PANE_BUTTON)
DEFINE_EVENT_TYPE(wxEVT_AUI_PANE_CLOSE)
DEFINE_EVENT_TYPE(wxEVT_AUI_RENDER)

#ifdef __WXMAC__
    // a few defines to avoid nameclashes
    #define __MAC_OS_X_MEMORY_MANAGER_CLEAN__ 1
    #define __AIFF__
    #include "wx/mac/private.h"
#endif

IMPLEMENT_DYNAMIC_CLASS(wxAuiManagerEvent, wxEvent)

class wxPseudoTransparentFrame : public wxFrame
{
public:
    wxPseudoTransparentFrame(wxWindow* parent = NULL,
                wxWindowID id = wxID_ANY,
                const wxString& title = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString &name = wxT("frame"))
                    : wxFrame(parent, id, title, pos, size, style | wxFRAME_SHAPED, name)
    {
        SetBackgroundStyle(wxBG_STYLE_CUSTOM);
        m_Amount=0;
        m_MaxWidth=0;
        m_MaxHeight=0;
#ifdef __WXGTK__
        m_CanSetShape = false; // have to wait for window create event on GTK
#else
        m_CanSetShape = true;
#endif
        SetTransparent(0);
    }

    virtual bool SetTransparent(wxByte alpha)
    {
        if (m_CanSetShape)
        {
            int w=100; // some defaults
            int h=100;
            GetClientSize(&w, &h);
            if ((alpha != m_Amount) || (m_MaxWidth<w) | (m_MaxHeight<h))
            {
                // Make the region at least double the height and width so we don't have
                // to rebuild if the size changes.
                m_MaxWidth=w*2;
                m_MaxHeight=h*2;
                m_Amount = alpha;
                m_Region.Clear();
//                m_Region.Union(0, 0, 1, m_MaxWidth);
                if (m_Amount)
                {
                    for (int y=0; y<m_MaxHeight; y++)
                    {
                        // Reverse the order of the bottom 4 bits
                        int j=((y&8)?1:0)|((y&4)?2:0)|((y&2)?4:0)|((y&1)?8:0);
                        if ((j*16+8)<m_Amount)
                            m_Region.Union(0, y, m_MaxWidth, 1);
                    }
                }
                SetShape(m_Region);
                Refresh();
            }
        }
        return true;
    }

    void OnPaint(wxPaintEvent& WXUNUSED(event))
    {
        wxPaintDC dc(this);

        dc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
        dc.SetPen(*wxTRANSPARENT_PEN);

        wxRegionIterator upd(GetUpdateRegion()); // get the update rect list

        while (upd)
        {
            wxRect rect(upd.GetRect());
            dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);

            upd++;
        }
    }

#ifdef __WXGTK__
    void OnWindowCreate(wxWindowCreateEvent& WXUNUSED(event)) {m_CanSetShape=true; SetTransparent(0);}
#endif

private:
    int m_Amount;
    int m_MaxWidth;
    int m_MaxHeight;
    bool m_CanSetShape;

    wxRegion m_Region;

    DECLARE_DYNAMIC_CLASS(wxPseudoTransparentFrame)
    DECLARE_EVENT_TABLE()
};


IMPLEMENT_DYNAMIC_CLASS( wxPseudoTransparentFrame, wxFrame )

BEGIN_EVENT_TABLE(wxPseudoTransparentFrame, wxFrame)
    EVT_PAINT(wxPseudoTransparentFrame::OnPaint)
#ifdef __WXGTK__
    EVT_WINDOW_CREATE(wxPseudoTransparentFrame::OnWindowCreate)
#endif
END_EVENT_TABLE()


// -- static utility functions --

static wxBitmap wxPaneCreateStippleBitmap()
{
    unsigned char data[] = { 0,0,0,192,192,192, 192,192,192,0,0,0 };
    wxImage img(2,2,data,true);
    return wxBitmap(img);
}

static void DrawResizeHint(wxDC& dc, const wxRect& rect)
{
    wxBitmap stipple = wxPaneCreateStippleBitmap();
    wxBrush brush(stipple);
    dc.SetBrush(brush);
    dc.SetPen(*wxTRANSPARENT_PEN);

    dc.SetLogicalFunction(wxXOR);
    dc.DrawRectangle(rect);
}



// CopyDocksAndPanes() - this utility function creates copies of
// the dock and pane info.  wxAuiDockInfo's usually contain pointers
// to wxAuiPaneInfo classes, thus this function is necessary to reliably
// reconstruct that relationship in the new dock info and pane info arrays

static void CopyDocksAndPanes(wxAuiDockInfoArray& dest_docks,
                              wxAuiPaneInfoArray& dest_panes,
                              const wxAuiDockInfoArray& src_docks,
                              const wxAuiPaneInfoArray& src_panes)
{
    dest_docks = src_docks;
    dest_panes = src_panes;
    int i, j, k, dock_count, pc1, pc2;
    for (i = 0, dock_count = dest_docks.GetCount(); i < dock_count; ++i)
    {
        wxAuiDockInfo& dock = dest_docks.Item(i);
        for (j = 0, pc1 = dock.panes.GetCount(); j < pc1; ++j)
            for (k = 0, pc2 = src_panes.GetCount(); k < pc2; ++k)
                if (dock.panes.Item(j) == &src_panes.Item(k))
                    dock.panes.Item(j) = &dest_panes.Item(k);
    }
}

// GetMaxLayer() is an internal function which returns
// the highest layer inside the specified dock
static int GetMaxLayer(const wxAuiDockInfoArray& docks, int dock_direction)
{
    int i, dock_count, max_layer = 0;
    for (i = 0, dock_count = docks.GetCount(); i < dock_count; ++i)
    {
        wxAuiDockInfo& dock = docks.Item(i);
        if (dock.dock_direction == dock_direction &&
            dock.dock_layer > max_layer && !dock.fixed)
                max_layer = dock.dock_layer;
    }
    return max_layer;
}


// GetMaxRow() is an internal function which returns
// the highest layer inside the specified dock
static int GetMaxRow(const wxAuiPaneInfoArray& panes, int direction, int layer)
{
    int i, pane_count, max_row = 0;
    for (i = 0, pane_count = panes.GetCount(); i < pane_count; ++i)
    {
        wxAuiPaneInfo& pane = panes.Item(i);
        if (pane.dock_direction == direction &&
            pane.dock_layer == layer &&
            pane.dock_row > max_row)
                max_row = pane.dock_row;
    }
    return max_row;
}



// DoInsertDockLayer() is an internal function that inserts a new dock
// layer by incrementing all existing dock layer values by one
static void DoInsertDockLayer(wxAuiPaneInfoArray& panes,
                              int dock_direction,
                              int dock_layer)
{
    int i, pane_count;
    for (i = 0, pane_count = panes.GetCount(); i < pane_count; ++i)
    {
        wxAuiPaneInfo& pane = panes.Item(i);
        if (!pane.IsFloating() &&
            pane.dock_direction == dock_direction &&
            pane.dock_layer >= dock_layer)
                pane.dock_layer++;
    }
}

// DoInsertDockLayer() is an internal function that inserts a new dock
// row by incrementing all existing dock row values by one
static void DoInsertDockRow(wxAuiPaneInfoArray& panes,
                            int dock_direction,
                            int dock_layer,
                            int dock_row)
{
    int i, pane_count;
    for (i = 0, pane_count = panes.GetCount(); i < pane_count; ++i)
    {
        wxAuiPaneInfo& pane = panes.Item(i);
        if (!pane.IsFloating() &&
            pane.dock_direction == dock_direction &&
            pane.dock_layer == dock_layer &&
            pane.dock_row >= dock_row)
                pane.dock_row++;
    }
}

// DoInsertDockLayer() is an internal function that inserts a space for
// another dock pane by incrementing all existing dock row values by one
static void DoInsertPane(wxAuiPaneInfoArray& panes,
                         int dock_direction,
                         int dock_layer,
                         int dock_row,
                         int dock_pos)
{
    int i, pane_count;
    for (i = 0, pane_count = panes.GetCount(); i < pane_count; ++i)
    {
        wxAuiPaneInfo& pane = panes.Item(i);
        if (!pane.IsFloating() &&
            pane.dock_direction == dock_direction &&
            pane.dock_layer == dock_layer &&
            pane.dock_row == dock_row &&
            pane.dock_pos >= dock_pos)
                pane.dock_pos++;
    }
}

// FindDocks() is an internal function that returns a list of docks which meet
// the specified conditions in the parameters and returns a sorted array
// (sorted by layer and then row)
static void FindDocks(wxAuiDockInfoArray& docks,
                      int dock_direction,
                      int dock_layer,
                      int dock_row,
                      wxAuiDockInfoPtrArray& arr)
{
    int begin_layer = dock_layer;
    int end_layer = dock_layer;
    int begin_row = dock_row;
    int end_row = dock_row;
    int dock_count = docks.GetCount();
    int layer, row, i, max_row = 0, max_layer = 0;

    // discover the maximum dock layer and the max row
    for (i = 0; i < dock_count; ++i)
    {
        max_row = wxMax(max_row, docks.Item(i).dock_row);
        max_layer = wxMax(max_layer, docks.Item(i).dock_layer);
    }

    // if no dock layer was specified, search all dock layers
    if (dock_layer == -1)
    {
        begin_layer = 0;
        end_layer = max_layer;
    }

    // if no dock row was specified, search all dock row
    if (dock_row == -1)
    {
        begin_row = 0;
        end_row = max_row;
    }

    arr.Clear();

    for (layer = begin_layer; layer <= end_layer; ++layer)
        for (row = begin_row; row <= end_row; ++row)
            for (i = 0; i < dock_count; ++i)
            {
                wxAuiDockInfo& d = docks.Item(i);
                if (dock_direction == -1 || dock_direction == d.dock_direction)
                {
                    if (d.dock_layer == layer && d.dock_row == row)
                        arr.Add(&d);
                }
            }
}

// FindPaneInDock() looks up a specified window pointer inside a dock.
// If found, the corresponding wxAuiPaneInfo pointer is returned, otherwise NULL.
static wxAuiPaneInfo* FindPaneInDock(const wxAuiDockInfo& dock, wxWindow* window)
{
    int i, count = dock.panes.GetCount();
    for (i = 0; i < count; ++i)
    {
        wxAuiPaneInfo* p = dock.panes.Item(i);
        if (p->window == window)
            return p;
    }
    return NULL;
}

// RemovePaneFromDocks() removes a pane window from all docks
// with a possible exception specified by parameter "except"
static void RemovePaneFromDocks(wxAuiDockInfoArray& docks,
                                wxAuiPaneInfo& pane,
                                wxAuiDockInfo* except = NULL)
{
    int i, dock_count;
    for (i = 0, dock_count = docks.GetCount(); i < dock_count; ++i)
    {
        wxAuiDockInfo& d = docks.Item(i);
        if (&d == except)
            continue;
        wxAuiPaneInfo* pi = FindPaneInDock(d, pane.window);
        if (pi)
            d.panes.Remove(pi);
    }
}

// RenumberDockRows() takes a dock and assigns sequential numbers
// to existing rows.  Basically it takes out the gaps; so if a
// dock has rows with numbers 0,2,5, they will become 0,1,2
static void RenumberDockRows(wxAuiDockInfoPtrArray& docks)
{
    int i, dock_count, j, pane_count;
    for (i = 0, dock_count = docks.GetCount(); i < dock_count; ++i)
    {
        wxAuiDockInfo& dock = *docks.Item(i);
        dock.dock_row = i;
        for (j = 0, pane_count = dock.panes.GetCount(); j < pane_count; ++j)
            dock.panes.Item(j)->dock_row = i;
    }
}


// SetActivePane() sets the active pane, as well as cycles through
// every other pane and makes sure that all others' active flags
// are turned off
static void SetActivePane(wxAuiPaneInfoArray& panes, wxWindow* active_pane)
{
    int i, pane_count;
    for (i = 0, pane_count = panes.GetCount(); i < pane_count; ++i)
    {
        wxAuiPaneInfo& pane = panes.Item(i);
        pane.state &= ~wxAuiPaneInfo::optionActive;
        if (pane.window == active_pane)
            pane.state |= wxAuiPaneInfo::optionActive;
    }
}


// this function is used to sort panes by dock position
static int PaneSortFunc(wxAuiPaneInfo** p1, wxAuiPaneInfo** p2)
{
    return ((*p1)->dock_pos < (*p2)->dock_pos) ? -1 : 1;
}


// -- wxAuiManager class implementation --


BEGIN_EVENT_TABLE(wxAuiManager, wxEvtHandler)
    EVT_AUI_PANE_BUTTON(wxAuiManager::OnPaneButton)
    EVT_AUI_RENDER(wxAuiManager::OnRender)
    EVT_PAINT(wxAuiManager::OnPaint)
    EVT_ERASE_BACKGROUND(wxAuiManager::OnEraseBackground)
    EVT_SIZE(wxAuiManager::OnSize)
    EVT_SET_CURSOR(wxAuiManager::OnSetCursor)
    EVT_LEFT_DOWN(wxAuiManager::OnLeftDown)
    EVT_LEFT_UP(wxAuiManager::OnLeftUp)
    EVT_MOTION(wxAuiManager::OnMotion)
    EVT_LEAVE_WINDOW(wxAuiManager::OnLeaveWindow)
    EVT_CHILD_FOCUS(wxAuiManager::OnChildFocus)
    EVT_TIMER(101, wxAuiManager::OnHintFadeTimer)
END_EVENT_TABLE()


wxAuiManager::wxAuiManager(wxWindow* managed_wnd, unsigned int flags)
{
    m_action = actionNone;
    m_last_mouse_move = wxPoint();
    m_hover_button = NULL;
    m_art = new wxAuiDefaultDockArt;
    m_hint_wnd = NULL;
    m_flags = flags;

    if (managed_wnd)
    {
        SetManagedWindow(managed_wnd);
    }
}

wxAuiManager::~wxAuiManager()
{
    delete m_art;
}

// GetPane() looks up a wxAuiPaneInfo structure based
// on the supplied window pointer.  Upon failure, GetPane()
// returns an empty wxAuiPaneInfo, a condition which can be checked
// by calling wxAuiPaneInfo::IsOk().
//
// The pane info's structure may then be modified.  Once a pane's
// info is modified, wxAuiManager::Update() must be called to
// realize the changes in the UI.

wxAuiPaneInfo& wxAuiManager::GetPane(wxWindow* window)
{
    int i, pane_count;
    for (i = 0, pane_count = m_panes.GetCount(); i < pane_count; ++i)
    {
        wxAuiPaneInfo& p = m_panes.Item(i);
        if (p.window == window)
            return p;
    }
    return wxAuiNullPaneInfo;
}

// this version of GetPane() looks up a pane based on a
// 'pane name', see above comment for more info
wxAuiPaneInfo& wxAuiManager::GetPane(const wxString& name)
{
    int i, pane_count;
    for (i = 0, pane_count = m_panes.GetCount(); i < pane_count; ++i)
    {
        wxAuiPaneInfo& p = m_panes.Item(i);
        if (p.name == name)
            return p;
    }
    return wxAuiNullPaneInfo;
}

// GetAllPanes() returns a reference to all the pane info structures
wxAuiPaneInfoArray& wxAuiManager::GetAllPanes()
{
    return m_panes;
}

// HitTest() is an internal function which determines which UI item
// the specified coordinates are over (x,y) specify a position in
// client coordinates
wxAuiDockUIPart* wxAuiManager::HitTest(int x, int y) {
  wxAuiDockUIPart* result = NULL;
  int i, part_count;
  for (i = 0, part_count = m_uiparts.GetCount(); i < part_count; ++i) {
    wxAuiDockUIPart* item = &m_uiparts.Item(i);
    // We are not interested in typeDock, because this space isn't
    // used to draw anything, just for measurements; besides, the
    // entire dock area is covered with other rectangles, which we are
    // interested in.
    if (item->type == wxAuiDockUIPart::typeDock) {
      continue;
    }
    // If we already have a hit on a more specific item, we are not
    // interested in a pane hit.  If, however, we don't already have a
    // hit, returning a pane hit is necessary for some operations
    if ((item->type == wxAuiDockUIPart::typePane ||
	 item->type == wxAuiDockUIPart::typePaneBorder) && result) {
      continue;
    }
    // If the point is inside the rectangle, we have a hit
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    if (item->rect.Inside(x,y)) {
      result = item;
    }
#pragma GCC diagnostic pop
  }
  return result;
}

// SetFlags() and GetFlags() allow the owner to set various
// options which are global to wxAuiManager
void wxAuiManager::SetFlags(unsigned int flags)
{
    m_flags = flags;
}

unsigned int wxAuiManager::GetFlags() const
{
    return m_flags;
}


// don't use these anymore as they are deprecated
// use Set/GetManagedFrame() instead
void wxAuiManager::SetFrame(wxFrame* frame)
{
    SetManagedWindow((wxWindow*)frame);
}

wxFrame* wxAuiManager::GetFrame() const
{
    return (wxFrame*)m_frame;
}




// SetManagedWindow() is usually called once when the frame
// manager class is being initialized.  "frame" specifies
// the frame which should be managed by the frame mananger
void wxAuiManager::SetManagedWindow(wxWindow* frame)
{
    wxASSERT_MSG(frame, wxT("specified frame must be non-NULL"));

    m_frame = frame;
    m_frame->PushEventHandler(this);

#if wxUSE_MDI
    // if the owner is going to manage an MDI parent frame,
    // we need to add the MDI client window as the default
    // center pane

    if (frame->IsKindOf(CLASSINFO(wxMDIParentFrame)))
    {
        wxMDIParentFrame* mdi_frame = (wxMDIParentFrame*)frame;
        wxWindow* client_window = mdi_frame->GetClientWindow();

        wxASSERT_MSG(client_window, wxT("Client window is NULL!"));

        AddPane(client_window,
                wxAuiPaneInfo().Name(wxT("mdiclient")).
                CenterPane().PaneBorder(false));
    }
#endif

    // Make a window to use for a transparent hint
#if defined(__WXMSW__) || defined(__WXGTK__)
    m_hint_wnd = new wxFrame(m_frame, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(1,1),
                             wxFRAME_TOOL_WINDOW |
                             wxFRAME_FLOAT_ON_PARENT |
                             wxFRAME_NO_TASKBAR |
                             wxNO_BORDER);

    m_hint_wnd->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));

#elif defined(__WXMAC__)
    // Using a miniframe with float and tool styles keeps the parent
    // frame activated and highlighted as such...
    m_hint_wnd = new wxMiniFrame(m_frame, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(1,1),
                                 wxFRAME_FLOAT_ON_PARENT
                                 | wxFRAME_TOOL_WINDOW
                                 | wxCAPTION );

    // Can't set the bg colour of a Frame in wxMac
    wxPanel* p = new wxPanel(m_hint_wnd);

    // The default wxSYS_COLOUR_ACTIVECAPTION colour is a light silver
    // color that is really hard to see, especially transparent.
    // Until a better system color is decided upon we'll just use
    // blue.
    p->SetBackgroundColour(*wxBLUE);
#endif

    m_hint_fademax=50;

    if (m_hint_wnd
        // CanSetTransparent is only present in the 2.7.0 ABI. To allow this file to be easily used
        // in a backported environment, conditionally compile this in.
#if wxCHECK_VERSION(2,7,0)
       && !m_hint_wnd->CanSetTransparent()
#endif
        )
    {

        m_hint_wnd->Close();
        m_hint_wnd->Destroy();
        m_hint_wnd = NULL;

        // If we can convert it to a PseudoTransparent window, do so
        m_hint_wnd = new wxPseudoTransparentFrame (m_frame, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(1,1),
                                                wxFRAME_TOOL_WINDOW |
                                                wxFRAME_FLOAT_ON_PARENT |
                                                wxFRAME_NO_TASKBAR |
                                                wxNO_BORDER);

        m_hint_fademax = 128;
    }
}


// UnInit() must be called, usually in the destructor
// of the frame class.   If it is not called, usually this
// will result in a crash upon program exit
void wxAuiManager::UnInit()
{
    m_frame->RemoveEventHandler(this);
}

// GetManagedWindow() returns the window pointer being managed
wxWindow* wxAuiManager::GetManagedWindow() const
{
    return m_frame;
}

wxAuiDockArt* wxAuiManager::GetArtProvider() const
{
    return m_art;
}

void wxAuiManager::ProcessMgrEvent(wxAuiManagerEvent& event)
{
    // first, give the owner frame a chance to override
    if (m_frame)
    {
        if (m_frame->ProcessEvent(event))
            return;
    }

    ProcessEvent(event);
}

// SetArtProvider() instructs wxAuiManager to use the
// specified art provider for all drawing calls.  This allows
// plugable look-and-feel features.  The pointer that is
// passed to this method subsequently belongs to wxAuiManager,
// and is deleted in the frame manager destructor
void wxAuiManager::SetArtProvider(wxAuiDockArt* art_provider)
{
    // delete the last art provider, if any
    delete m_art;

    // assign the new art provider
    m_art = art_provider;
}


bool wxAuiManager::AddPane(wxWindow* window, const wxAuiPaneInfo& pane_info)
{
    // check if the pane has a valid window
    if (!window)
        return false;

    // check if the pane already exists
    if (GetPane(pane_info.window).IsOk())
        return false;

    m_panes.Add(pane_info);

    wxAuiPaneInfo& pinfo = m_panes.Last();

    // set the pane window
    pinfo.window = window;

    // if the pane's name identifier is blank, create a random string
    if (pinfo.name.empty())
    {
        pinfo.name.Printf(wxT("%08lx%08x%08x%08lx"),
             ((unsigned long)pinfo.window) & 0xffffffff,
             (unsigned int)time(NULL),
#ifdef __WXWINCE__
             (unsigned int)GetTickCount(),
#else
             (unsigned int)clock(),
#endif
             (unsigned long)m_panes.GetCount());
    }

    // set initial proportion (if not already set)
    if (pinfo.dock_proportion == 0)
        pinfo.dock_proportion = 100000;

    if (pinfo.HasCloseButton() &&
        pinfo.buttons.size() == 0)
    {
        wxAuiPaneButton button;
        button.button_id = wxAuiPaneInfo::buttonClose;
        pinfo.buttons.Add(button);
    }

    if (pinfo.best_size == wxDefaultSize &&
        pinfo.window)
    {
        pinfo.best_size = pinfo.window->GetClientSize();

        if (pinfo.window->IsKindOf(CLASSINFO(wxToolBar)))
        {
            // GetClientSize() doesn't get the best size for
            // a toolbar under some newer versions of wxWidgets,
            // so use GetBestSize()
            pinfo.best_size = pinfo.window->GetBestSize();

            // for some reason, wxToolBar::GetBestSize() is returning
            // a size that is a pixel shy of the correct amount.
            // I believe this to be the correct action, until
            // wxToolBar::GetBestSize() is fixed.  Is this assumption
            // correct?
            pinfo.best_size.y++;
        }

        if (pinfo.min_size != wxDefaultSize)
        {
            if (pinfo.best_size.x < pinfo.min_size.x)
                pinfo.best_size.x = pinfo.min_size.x;
            if (pinfo.best_size.y < pinfo.min_size.y)
                pinfo.best_size.y = pinfo.min_size.y;
        }
    }

    return true;
}

bool wxAuiManager::AddPane(wxWindow* window,
                             int direction,
                             const wxString& caption)
{
    wxAuiPaneInfo pinfo;
    pinfo.Caption(caption);
    switch (direction)
    {
        case wxTOP:    pinfo.Top(); break;
        case wxBOTTOM: pinfo.Bottom(); break;
        case wxLEFT:   pinfo.Left(); break;
        case wxRIGHT:  pinfo.Right(); break;
        case wxCENTER: pinfo.CenterPane(); break;
    }
    return AddPane(window, pinfo);
}

bool wxAuiManager::AddPane(wxWindow* window,
                             const wxAuiPaneInfo& pane_info,
                             const wxPoint& drop_pos)
{
    if (!AddPane(window, pane_info))
        return false;

    wxAuiPaneInfo& pane = GetPane(window);

    DoDrop(m_docks, m_panes, pane, drop_pos, wxPoint(0,0));

    return true;
}

bool wxAuiManager::InsertPane(wxWindow* window, const wxAuiPaneInfo& pane_info,
                                int insert_level)
{
    // shift the panes around, depending on the insert level
    switch (insert_level)
    {
        case wxAUI_INSERT_PANE:
            DoInsertPane(m_panes,
                 pane_info.dock_direction,
                 pane_info.dock_layer,
                 pane_info.dock_row,
                 pane_info.dock_pos);
            break;
        case wxAUI_INSERT_ROW:
            DoInsertDockRow(m_panes,
                 pane_info.dock_direction,
                 pane_info.dock_layer,
                 pane_info.dock_row);
            break;
        case wxAUI_INSERT_DOCK:
            DoInsertDockLayer(m_panes,
                 pane_info.dock_direction,
                 pane_info.dock_layer);
            break;
    }

    // if the window already exists, we are basically just moving/inserting the
    // existing window.  If it doesn't exist, we need to add it and insert it
    wxAuiPaneInfo& existing_pane = GetPane(window);
    if (!existing_pane.IsOk())
    {
        return AddPane(window, pane_info);
    }
    else
    {
        if (pane_info.IsFloating())
        {
            existing_pane.Float();
            if (pane_info.floating_pos != wxDefaultPosition)
                existing_pane.FloatingPosition(pane_info.floating_pos);
            if (pane_info.floating_size != wxDefaultSize)
                existing_pane.FloatingSize(pane_info.floating_size);
        }
         else
        {
            existing_pane.Direction(pane_info.dock_direction);
            existing_pane.Layer(pane_info.dock_layer);
            existing_pane.Row(pane_info.dock_row);
            existing_pane.Position(pane_info.dock_pos);
        }
    }

    return true;
}


// DetachPane() removes a pane from the frame manager.  This
// method will not destroy the window that is removed.
bool wxAuiManager::DetachPane(wxWindow* window)
{
    int i, count;
    for (i = 0, count = m_panes.GetCount(); i < count; ++i)
    {
        wxAuiPaneInfo& p = m_panes.Item(i);
        if (p.window == window)
        {
            if (p.frame)
            {
                // we have a floating frame which is being detached. We need to
                // reparent it to m_frame and destroy the floating frame

                // reduce flicker
                p.window->SetSize(1,1);
                p.frame->Show(false);

                // reparent to m_frame and destroy the pane
                p.window->Reparent(m_frame);
                p.frame->SetSizer(NULL);
                p.frame->Destroy();
                p.frame = NULL;
            }

            // make sure there are no references to this pane in our uiparts,
            // just in case the caller doesn't call Update() immediately after
            // the DetachPane() call.  This prevets obscure crashes which would
            // happen at window repaint if the caller forgets to call Update()
            int pi, part_count;
            for (pi = 0, part_count = (int)m_uiparts.GetCount(); pi < part_count; ++pi)
            {
                wxAuiDockUIPart& part = m_uiparts.Item(pi);
                if (part.pane == &p)
                {
                    m_uiparts.RemoveAt(pi);
                    part_count--;
                    pi--;
                    continue;
                }
            }

            m_panes.RemoveAt(i);
            return true;
        }
    }
    return false;
}


// EscapeDelimiters() changes ";" into "\;" and "|" into "\|"
// in the input string.  This is an internal functions which is
// used for saving perspectives
static wxString EscapeDelimiters(const wxString& s)
{
    wxString result;
    result.Alloc(s.length());
    const wxChar* ch = s.c_str();
    while (*ch)
    {
        if (*ch == wxT(';') || *ch == wxT('|'))
            result += wxT('\\');
        result += *ch;
        ++ch;
    }
    return result;
}

wxString wxAuiManager::SavePaneInfo(wxAuiPaneInfo& pane)
{
    wxString result = wxT("name=");
    result += EscapeDelimiters(pane.name);
    result += wxT(";");

    result += wxT("caption=");
    result += EscapeDelimiters(pane.caption);
    result += wxT(";");

    result += wxString::Format(wxT("state=%u;"), pane.state);
    result += wxString::Format(wxT("dir=%d;"), pane.dock_direction);
    result += wxString::Format(wxT("layer=%d;"), pane.dock_layer);
    result += wxString::Format(wxT("row=%d;"), pane.dock_row);
    result += wxString::Format(wxT("pos=%d;"), pane.dock_pos);
    result += wxString::Format(wxT("prop=%d;"), pane.dock_proportion);
    result += wxString::Format(wxT("bestw=%d;"), pane.best_size.x);
    result += wxString::Format(wxT("besth=%d;"), pane.best_size.y);
    result += wxString::Format(wxT("minw=%d;"), pane.min_size.x);
    result += wxString::Format(wxT("minh=%d;"), pane.min_size.y);
    result += wxString::Format(wxT("maxw=%d;"), pane.max_size.x);
    result += wxString::Format(wxT("maxh=%d;"), pane.max_size.y);
    result += wxString::Format(wxT("floatx=%d;"), pane.floating_pos.x);
    result += wxString::Format(wxT("floaty=%d;"), pane.floating_pos.y);
    result += wxString::Format(wxT("floatw=%d;"), pane.floating_size.x);
    result += wxString::Format(wxT("floath=%d"), pane.floating_size.y);

    return result;
}

// Load a "pane" with the pane infor settings in pane_part
void wxAuiManager::LoadPaneInfo(wxString pane_part, wxAuiPaneInfo &pane)
{
    // replace escaped characters so we can
    // split up the string easily
    pane_part.Replace(wxT("\\|"), wxT("\a"));
    pane_part.Replace(wxT("\\;"), wxT("\b"));

    while(1)
    {
        wxString val_part = pane_part.BeforeFirst(wxT(';'));
        pane_part = pane_part.AfterFirst(wxT(';'));
        wxString val_name = val_part.BeforeFirst(wxT('='));
        wxString value = val_part.AfterFirst(wxT('='));
        val_name.MakeLower();
        val_name.Trim(true);
        val_name.Trim(false);
        value.Trim(true);
        value.Trim(false);
    
        if (val_name.empty())
            break;
    
        if (val_name == wxT("name"))
            pane.name = value;
        else if (val_name == wxT("caption"))
            pane.caption = value;
        else if (val_name == wxT("state"))
            pane.state = (unsigned int)wxAtoi(value.c_str());
        else if (val_name == wxT("dir"))
            pane.dock_direction = wxAtoi(value.c_str());
        else if (val_name == wxT("layer"))
            pane.dock_layer = wxAtoi(value.c_str());
        else if (val_name == wxT("row"))
            pane.dock_row = wxAtoi(value.c_str());
        else if (val_name == wxT("pos"))
            pane.dock_pos = wxAtoi(value.c_str());
        else if (val_name == wxT("prop"))
            pane.dock_proportion = wxAtoi(value.c_str());
        else if (val_name == wxT("bestw"))
            pane.best_size.x = wxAtoi(value.c_str());
        else if (val_name == wxT("besth"))
            pane.best_size.y = wxAtoi(value.c_str());
        else if (val_name == wxT("minw"))
            pane.min_size.x = wxAtoi(value.c_str());
        else if (val_name == wxT("minh"))
            pane.min_size.y = wxAtoi(value.c_str());
        else if (val_name == wxT("maxw"))
            pane.max_size.x = wxAtoi(value.c_str());
        else if (val_name == wxT("maxh"))
            pane.max_size.y = wxAtoi(value.c_str());
        else if (val_name == wxT("floatx"))
            pane.floating_pos.x = wxAtoi(value.c_str());
        else if (val_name == wxT("floaty"))
            pane.floating_pos.y = wxAtoi(value.c_str());
        else if (val_name == wxT("floatw"))
            pane.floating_size.x = wxAtoi(value.c_str());
        else if (val_name == wxT("floath"))
            pane.floating_size.y = wxAtoi(value.c_str());
        else {
            wxFAIL_MSG(wxT("Bad Perspective String"));
        }
    }

    // replace escaped characters so we can
    // split up the string easily
    pane.name.Replace(wxT("\a"), wxT("|"));
    pane.name.Replace(wxT("\b"), wxT(";"));
    pane.caption.Replace(wxT("\a"), wxT("|"));
    pane.caption.Replace(wxT("\b"), wxT(";"));
    pane_part.Replace(wxT("\a"), wxT("|"));
    pane_part.Replace(wxT("\b"), wxT(";"));

    return;
}


// SavePerspective() saves all pane information as a single string.
// This string may later be fed into LoadPerspective() to restore
// all pane settings.  This save and load mechanism allows an
// exact pane configuration to be saved and restored at a later time

wxString wxAuiManager::SavePerspective()
{
    wxString result;
    result.Alloc(500);
    result = wxT("layout1|");

    int pane_i, pane_count = m_panes.GetCount();
    for (pane_i = 0; pane_i < pane_count; ++pane_i)
    {
        wxAuiPaneInfo& pane = m_panes.Item(pane_i);
        result += SavePaneInfo(pane)+wxT("|");
    }

    int dock_i, dock_count = m_docks.GetCount();
    for (dock_i = 0; dock_i < dock_count; ++dock_i)
    {
        wxAuiDockInfo& dock = m_docks.Item(dock_i);

        result += wxString::Format(wxT("dock_size(%d,%d,%d)=%d|"),
                                   dock.dock_direction, dock.dock_layer,
                                   dock.dock_row, dock.size);
    }

    return result;
}

// LoadPerspective() loads a layout which was saved with SavePerspective()
// If the "update" flag parameter is true, the GUI will immediately be updated

bool wxAuiManager::LoadPerspective(const wxString& layout, bool update)
{
    wxString input = layout;
    wxString part;

    // check layout string version
    part = input.BeforeFirst(wxT('|'));
    input = input.AfterFirst(wxT('|'));
    part.Trim(true);
    part.Trim(false);
    if (part != wxT("layout1"))
        return false;

    // mark all panes currently managed as docked and hidden
    int pane_i, pane_count = m_panes.GetCount();
    for (pane_i = 0; pane_i < pane_count; ++pane_i)
        m_panes.Item(pane_i).Dock().Hide();

    // clear out the dock array; this will be reconstructed
    m_docks.Clear();

    // replace escaped characters so we can
    // split up the string easily
    input.Replace(wxT("\\|"), wxT("\a"));
    input.Replace(wxT("\\;"), wxT("\b"));

    while (1)
    {
        wxAuiPaneInfo pane;

        wxString pane_part = input.BeforeFirst(wxT('|'));
        input = input.AfterFirst(wxT('|'));
        pane_part.Trim(true);

        // if the string is empty, we're done parsing
        if (pane_part.empty())
            break;

        if (pane_part.Left(9) == wxT("dock_size"))
        {
            wxString val_name = pane_part.BeforeFirst(wxT('='));
            wxString value = pane_part.AfterFirst(wxT('='));

            long dir, layer, row, size;
            wxString piece = val_name.AfterFirst(wxT('('));
            piece = piece.BeforeLast(wxT(')'));
            piece.BeforeFirst(wxT(',')).ToLong(&dir);
            piece = piece.AfterFirst(wxT(','));
            piece.BeforeFirst(wxT(',')).ToLong(&layer);
            piece.AfterFirst(wxT(',')).ToLong(&row);
            value.ToLong(&size);

            wxAuiDockInfo dock;
            dock.dock_direction = dir;
            dock.dock_layer = layer;
            dock.dock_row = row;
            dock.size = size;
            m_docks.Add(dock);
            continue;
        }

        // Undo our escaping as LoadPaneInfo needs to take an unescaped
        // name so it can be called by external callers
        pane_part.Replace(wxT("\a"), wxT("|"));
        pane_part.Replace(wxT("\b"), wxT(";"));

        LoadPaneInfo(pane_part, pane);

        wxAuiPaneInfo& p = GetPane(pane.name);
        if (!p.IsOk())
        {
            // the pane window couldn't be found
            // in the existing layout
            return false;
        }

        p.SafeSet(pane);

    }

    if (update)
        Update();

    return true;
}

void wxAuiManager::GetPanePositionsAndSizes(wxAuiDockInfo& dock,
                                              wxArrayInt& positions,
                                              wxArrayInt& sizes)
{
    int caption_size = m_art->GetMetric(wxAUI_ART_CAPTION_SIZE);
    int pane_border_size = m_art->GetMetric(wxAUI_ART_PANE_BORDER_SIZE);
    int gripper_size = m_art->GetMetric(wxAUI_ART_GRIPPER_SIZE);

    positions.Empty();
    sizes.Empty();

    int offset, action_pane = -1;
    int pane_i, pane_count = dock.panes.GetCount();

    // find the pane marked as our action pane
    for (pane_i = 0; pane_i < pane_count; ++pane_i)
    {
        wxAuiPaneInfo& pane = *(dock.panes.Item(pane_i));

        if (pane.state & wxAuiPaneInfo::actionPane)
        {
            wxASSERT_MSG(action_pane==-1, wxT("Too many fixed action panes"));
            action_pane = pane_i;
        }
    }

    // set up each panes default position, and
    // determine the size (width or height, depending
    // on the dock's orientation) of each pane
    for (pane_i = 0; pane_i < pane_count; ++pane_i)
    {
        wxAuiPaneInfo& pane = *(dock.panes.Item(pane_i));
        positions.Add(pane.dock_pos);
        int size = 0;

        if (pane.HasBorder())
            size += (pane_border_size*2);

        if (dock.IsHorizontal())
        {
            if (pane.HasGripper() && !pane.HasGripperTop())
                size += gripper_size;
            size += pane.best_size.x;
        }
         else
        {
            if (pane.HasGripper() && pane.HasGripperTop())
                size += gripper_size;

            if (pane.HasCaption())
                size += caption_size;
            size += pane.best_size.y;
        }

        sizes.Add(size);
    }

    // if there is no action pane, just return the default
    // positions (as specified in pane.pane_pos)
    if (action_pane == -1)
        return;

    offset = 0;
    for (pane_i = action_pane-1; pane_i >= 0; --pane_i)
    {
        int amount = positions[pane_i+1] - (positions[pane_i] + sizes[pane_i]);

        if (amount >= 0)
            offset += amount;
             else
            positions[pane_i] -= -amount;

        offset += sizes[pane_i];
    }

    // if the dock mode is fixed, make sure none of the panes
    // overlap; we will bump panes that overlap
    offset = 0;
    for (pane_i = action_pane; pane_i < pane_count; ++pane_i)
    {
        int amount = positions[pane_i] - offset;
        if (amount >= 0)
            offset += amount;
             else
            positions[pane_i] += -amount;

        offset += sizes[pane_i];
    }
}


void wxAuiManager::LayoutAddPane(wxSizer* cont,
                                   wxAuiDockInfo& dock,
                                   wxAuiPaneInfo& pane,
                                   wxAuiDockUIPartArray& uiparts,
                                   bool spacer_only)
{
    wxAuiDockUIPart part;
    wxSizerItem* sizer_item;

    int caption_size = m_art->GetMetric(wxAUI_ART_CAPTION_SIZE);
    int gripper_size = m_art->GetMetric(wxAUI_ART_GRIPPER_SIZE);
    int pane_border_size = m_art->GetMetric(wxAUI_ART_PANE_BORDER_SIZE);
    int pane_button_size = m_art->GetMetric(wxAUI_ART_PANE_BUTTON_SIZE);

    // find out the orientation of the item (orientation for panes
    // is the same as the dock's orientation)
    int orientation;
    if (dock.IsHorizontal())
        orientation = wxHORIZONTAL;
         else
        orientation = wxVERTICAL;

    // this variable will store the proportion
    // value that the pane will receive
    int pane_proportion = pane.dock_proportion;

    wxBoxSizer* horz_pane_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* vert_pane_sizer = new wxBoxSizer(wxVERTICAL);

    if (pane.HasGripper())
    {
        if (pane.HasGripperTop())
            sizer_item = vert_pane_sizer ->Add(1, gripper_size, 0, wxEXPAND);
        else
            sizer_item = horz_pane_sizer ->Add(gripper_size, 1, 0, wxEXPAND);

        part.type = wxAuiDockUIPart::typeGripper;
        part.dock = &dock;
        part.pane = &pane;
        part.button = NULL;
        part.orientation = orientation;
        part.cont_sizer = horz_pane_sizer;
        part.sizer_item = sizer_item;
        uiparts.Add(part);
    }

    if (pane.HasCaption())
    {
        // create the caption sizer
        wxBoxSizer* caption_sizer = new wxBoxSizer(wxHORIZONTAL);

        sizer_item = caption_sizer->Add(1, caption_size, 1, wxEXPAND);

        part.type = wxAuiDockUIPart::typeCaption;
        part.dock = &dock;
        part.pane = &pane;
        part.button = NULL;
        part.orientation = orientation;
        part.cont_sizer = vert_pane_sizer;
        part.sizer_item = sizer_item;
        int caption_part_idx = uiparts.GetCount();
        uiparts.Add(part);

        // add pane buttons to the caption
        int i, button_count;
        for (i = 0, button_count = pane.buttons.GetCount();
             i < button_count; ++i)
        {
            wxAuiPaneButton& button = pane.buttons.Item(i);

            sizer_item = caption_sizer->Add(pane_button_size,
                                            caption_size,
                                            0, wxEXPAND);

            part.type = wxAuiDockUIPart::typePaneButton;
            part.dock = &dock;
            part.pane = &pane;
            part.button = &button;
            part.orientation = orientation;
            part.cont_sizer = caption_sizer;
            part.sizer_item = sizer_item;
            uiparts.Add(part);
        }

        // add the caption sizer
        sizer_item = vert_pane_sizer->Add(caption_sizer, 0, wxEXPAND);

        uiparts.Item(caption_part_idx).sizer_item = sizer_item;
    }

    // add the pane window itself
    if (spacer_only)
    {
        sizer_item = vert_pane_sizer->Add(1, 1, 1, wxEXPAND);
    }
    else
    {
        sizer_item = vert_pane_sizer->Add(pane.window, 1, wxEXPAND);
        // Don't do this because it breaks the pane size in floating windows
        // BIW: Right now commenting this out is causing problems with
        // an mdi client window as the center pane.
        vert_pane_sizer->SetItemMinSize(pane.window, 1, 1);
    }

    part.type = wxAuiDockUIPart::typePane;
    part.dock = &dock;
    part.pane = &pane;
    part.button = NULL;
    part.orientation = orientation;
    part.cont_sizer = vert_pane_sizer;
    part.sizer_item = sizer_item;
    uiparts.Add(part);


    // determine if the pane should have a minimum size; if the pane is
    // non-resizable (fixed) then we must set a minimum size. Alternitavely,
    // if the pane.min_size is set, we must use that value as well

    wxSize min_size = pane.min_size;
    if (pane.IsFixed())
    {
        if (min_size == wxDefaultSize)
        {
            min_size = pane.best_size;
            pane_proportion = 0;
        }
    }

    if (min_size != wxDefaultSize)
    {
        vert_pane_sizer->SetItemMinSize(
                        vert_pane_sizer->GetChildren().GetCount()-1,
                        min_size.x, min_size.y);
    }


    // add the verticle sizer (caption, pane window) to the
    // horizontal sizer (gripper, verticle sizer)
    horz_pane_sizer->Add(vert_pane_sizer, 1, wxEXPAND);

    // finally, add the pane sizer to the dock sizer

    if (pane.HasBorder())
    {
        // allowing space for the pane's border
        sizer_item = cont->Add(horz_pane_sizer, pane_proportion,
                               wxEXPAND | wxALL, pane_border_size);

        part.type = wxAuiDockUIPart::typePaneBorder;
        part.dock = &dock;
        part.pane = &pane;
        part.button = NULL;
        part.orientation = orientation;
        part.cont_sizer = cont;
        part.sizer_item = sizer_item;
        uiparts.Add(part);
    }
    else
    {
        sizer_item = cont->Add(horz_pane_sizer, pane_proportion, wxEXPAND);
    }
}

void wxAuiManager::LayoutAddDock(wxSizer* cont,
                                   wxAuiDockInfo& dock,
                                   wxAuiDockUIPartArray& uiparts,
                                   bool spacer_only)
{
    wxSizerItem* sizer_item;
    wxAuiDockUIPart part;

    int sash_size = m_art->GetMetric(wxAUI_ART_SASH_SIZE);
    int orientation = dock.IsHorizontal() ? wxHORIZONTAL : wxVERTICAL;

    // resizable bottom and right docks have a sash before them
    if (!dock.fixed && (dock.dock_direction == wxAUI_DOCK_BOTTOM ||
                        dock.dock_direction == wxAUI_DOCK_RIGHT))
    {
        sizer_item = cont->Add(sash_size, sash_size, 0, wxEXPAND);

        part.type = wxAuiDockUIPart::typeDockSizer;
        part.orientation = orientation;
        part.dock = &dock;
        part.pane = NULL;
        part.button = NULL;
        part.cont_sizer = cont;
        part.sizer_item = sizer_item;
        uiparts.Add(part);
    }

    // create the sizer for the dock
    wxSizer* dock_sizer = new wxBoxSizer(orientation);

    // add each pane to the dock
    int pane_i, pane_count = dock.panes.GetCount();

    if (dock.fixed)
    {
        wxArrayInt pane_positions, pane_sizes;

        // figure out the real pane positions we will
        // use, without modifying the each pane's pane_pos member
        GetPanePositionsAndSizes(dock, pane_positions, pane_sizes);

        int offset = 0;
        for (pane_i = 0; pane_i < pane_count; ++pane_i)
        {
            wxAuiPaneInfo& pane = *(dock.panes.Item(pane_i));
            int pane_pos = pane_positions.Item(pane_i);

            int amount = pane_pos - offset;
            if (amount > 0)
            {
                if (dock.IsVertical())
                    sizer_item = dock_sizer->Add(1, amount, 0, wxEXPAND);
                     else
                    sizer_item = dock_sizer->Add(amount, 1, 0, wxEXPAND);

                part.type = wxAuiDockUIPart::typeBackground;
                part.dock = &dock;
                part.pane = NULL;
                part.button = NULL;
                part.orientation = (orientation==wxHORIZONTAL) ? wxVERTICAL:wxHORIZONTAL;
                part.cont_sizer = dock_sizer;
                part.sizer_item = sizer_item;
                uiparts.Add(part);

                offset += amount;
            }

            LayoutAddPane(dock_sizer, dock, pane, uiparts, spacer_only);

            offset += pane_sizes.Item(pane_i);
        }

        // at the end add a very small stretchable background area
        sizer_item = dock_sizer->Add(1,1, 1, wxEXPAND);

        part.type = wxAuiDockUIPart::typeBackground;
        part.dock = &dock;
        part.pane = NULL;
        part.button = NULL;
        part.orientation = orientation;
        part.cont_sizer = dock_sizer;
        part.sizer_item = sizer_item;
        uiparts.Add(part);
    }
    else
    {
        for (pane_i = 0; pane_i < pane_count; ++pane_i)
        {
            wxAuiPaneInfo& pane = *(dock.panes.Item(pane_i));

            // if this is not the first pane being added,
            // we need to add a pane sizer
            if (pane_i > 0)
            {
                sizer_item = dock_sizer->Add(sash_size, sash_size, 0, wxEXPAND);

                part.type = wxAuiDockUIPart::typePaneSizer;
                part.dock = &dock;
                part.pane = dock.panes.Item(pane_i-1);
                part.button = NULL;
                part.orientation = (orientation==wxHORIZONTAL) ? wxVERTICAL:wxHORIZONTAL;
                part.cont_sizer = dock_sizer;
                part.sizer_item = sizer_item;
                uiparts.Add(part);
            }

            LayoutAddPane(dock_sizer, dock, pane, uiparts, spacer_only);
        }
    }

    if (dock.dock_direction == wxAUI_DOCK_CENTER)
        sizer_item = cont->Add(dock_sizer, 1, wxEXPAND);
         else
        sizer_item = cont->Add(dock_sizer, 0, wxEXPAND);

    part.type = wxAuiDockUIPart::typeDock;
    part.dock = &dock;
    part.pane = NULL;
    part.button = NULL;
    part.orientation = orientation;
    part.cont_sizer = cont;
    part.sizer_item = sizer_item;
    uiparts.Add(part);

    if (dock.IsHorizontal())
        cont->SetItemMinSize(dock_sizer, 0, dock.size);
         else
        cont->SetItemMinSize(dock_sizer, dock.size, 0);

    //  top and left docks have a sash after them
    if (!dock.fixed && (dock.dock_direction == wxAUI_DOCK_TOP ||
                        dock.dock_direction == wxAUI_DOCK_LEFT))
    {
        sizer_item = cont->Add(sash_size, sash_size, 0, wxEXPAND);

        part.type = wxAuiDockUIPart::typeDockSizer;
        part.dock = &dock;
        part.pane = NULL;
        part.button = NULL;
        part.orientation = orientation;
        part.cont_sizer = cont;
        part.sizer_item = sizer_item;
        uiparts.Add(part);
    }
}

wxSizer* wxAuiManager::LayoutAll(wxAuiPaneInfoArray& panes,
                                   wxAuiDockInfoArray& docks,
                                   wxAuiDockUIPartArray& uiparts,
                                   bool spacer_only)
{
    wxBoxSizer* container = new wxBoxSizer(wxVERTICAL);

    int pane_border_size = m_art->GetMetric(wxAUI_ART_PANE_BORDER_SIZE);
    int caption_size = m_art->GetMetric(wxAUI_ART_CAPTION_SIZE);
    wxSize cli_size = m_frame->GetClientSize();
    int i, dock_count, pane_count;


    // empty all docks out
    for (i = 0, dock_count = docks.GetCount(); i < dock_count; ++i)
        docks.Item(i).panes.Empty();

    // iterate through all known panes, filing each
    // of them into the appropriate dock. If the
    // pane does not exist in the dock, add it
    for (i = 0, pane_count = panes.GetCount(); i < pane_count; ++i)
    {
        wxAuiPaneInfo& p = panes.Item(i);

        // find any docks in this layer
        wxAuiDockInfo* dock;
        wxAuiDockInfoPtrArray arr;
        FindDocks(docks, p.dock_direction, p.dock_layer, p.dock_row, arr);

        if (arr.GetCount() > 0)
        {
            dock = arr.Item(0);
        }
         else
        {
            // dock was not found, so we need to create a new one
            wxAuiDockInfo d;
            d.dock_direction = p.dock_direction;
            d.dock_layer = p.dock_layer;
            d.dock_row = p.dock_row;
            docks.Add(d);
            dock = &docks.Last();
        }


        if (p.IsDocked() && p.IsShown())
        {
            // remove the pane from any existing docks except this one
            RemovePaneFromDocks(docks, p, dock);

            // pane needs to be added to the dock,
            // if it doesn't already exist
            if (!FindPaneInDock(*dock, p.window))
                dock->panes.Add(&p);
        }
         else
        {
            // remove the pane from any existing docks
            RemovePaneFromDocks(docks, p);
        }

    }

    // remove any empty docks
    for (i = docks.GetCount()-1; i >= 0; --i)
    {
        if (docks.Item(i).panes.GetCount() == 0)
            docks.RemoveAt(i);
    }

    // configure the docks further
    for (i = 0, dock_count = docks.GetCount(); i < dock_count; ++i)
    {
        wxAuiDockInfo& dock = docks.Item(i);
        int j, dock_pane_count = dock.panes.GetCount();

        // sort the dock pane array by the pane's
        // dock position (dock_pos), in ascending order
        dock.panes.Sort(PaneSortFunc);

        // for newly created docks, set up their initial size
        if (dock.size == 0)
        {
            int size = 0;

            for (j = 0; j < dock_pane_count; ++j)
            {
                wxAuiPaneInfo& pane = *dock.panes.Item(j);
                wxSize pane_size = pane.best_size;
                if (pane_size == wxDefaultSize)
                    pane_size = pane.min_size;
                if (pane_size == wxDefaultSize)
                    pane_size = pane.window->GetSize();

                if (dock.IsHorizontal())
                    size = wxMax(pane_size.y, size);
                     else
                    size = wxMax(pane_size.x, size);
            }

            // add space for the border (two times), but only
            // if at least one pane inside the dock has a pane border
            for (j = 0; j < dock_pane_count; ++j)
            {
                if (dock.panes.Item(j)->HasBorder())
                {
                    size += (pane_border_size*2);
                    break;
                }
            }

            // if pane is on the top or bottom, add the caption height,
            // but only if at least one pane inside the dock has a caption
            if (dock.IsHorizontal())
            {
                for (j = 0; j < dock_pane_count; ++j)
                {
                    if (dock.panes.Item(j)->HasCaption())
                    {
                        size += caption_size;
                        break;
                    }
                }
            }

            // new dock's size may not be more than 1/3 of the frame size
            if (dock.IsHorizontal())
                size = wxMin(size, cli_size.y/3);
                 else
                size = wxMin(size, cli_size.x/3);

            if (size < 10)
                size = 10;
            dock.size = size;
        }


        // determine the dock's minimum size
        bool plus_border = false;
        bool plus_caption = false;
        int dock_min_size = 0;
        for (j = 0; j < dock_pane_count; ++j)
        {
            wxAuiPaneInfo& pane = *dock.panes.Item(j);
            if (pane.min_size != wxDefaultSize)
            {
                if (pane.HasBorder())
                    plus_border = true;
                if (pane.HasCaption())
                    plus_caption = true;
                if (dock.IsHorizontal())
                {
                    if (pane.min_size.y > dock_min_size)
                        dock_min_size = pane.min_size.y;
                }
                 else
                {
                    if (pane.min_size.x > dock_min_size)
                        dock_min_size = pane.min_size.x;
                }
            }
        }

        if (plus_border)
            dock_min_size += (pane_border_size*2);
        if (plus_caption && dock.IsHorizontal())
            dock_min_size += (caption_size);

        dock.min_size = dock_min_size;


        // if the pane's current size is less than it's
        // minimum, increase the dock's size to it's minimum
        if (dock.size < dock.min_size)
            dock.size = dock.min_size;


        // determine the dock's mode (fixed or proportional);
        // determine whether the dock has only toolbars
        bool action_pane_marked = false;
        dock.fixed = true;
        dock.toolbar = true;
        for (j = 0; j < dock_pane_count; ++j)
        {
            wxAuiPaneInfo& pane = *dock.panes.Item(j);
            if (!pane.IsFixed())
                dock.fixed = false;
            if (!pane.IsToolbar())
                dock.toolbar = false;
            if (pane.state & wxAuiPaneInfo::actionPane)
                action_pane_marked = true;
        }


        // if the dock mode is proportional and not fixed-pixel,
        // reassign the dock_pos to the sequential 0, 1, 2, 3;
        // e.g. remove gaps like 1, 2, 30, 500
        if (!dock.fixed)
        {
            for (j = 0; j < dock_pane_count; ++j)
            {
                wxAuiPaneInfo& pane = *dock.panes.Item(j);
                pane.dock_pos = j;
            }
        }

        // if the dock mode is fixed, and none of the panes
        // are being moved right now, make sure the panes
        // do not overlap each other.  If they do, we will
        // adjust the panes' positions
        if (dock.fixed && !action_pane_marked)
        {
            wxArrayInt pane_positions, pane_sizes;
            GetPanePositionsAndSizes(dock, pane_positions, pane_sizes);

            int offset = 0;
            for (j = 0; j < dock_pane_count; ++j)
            {
                wxAuiPaneInfo& pane = *(dock.panes.Item(j));
                pane.dock_pos = pane_positions[j];

                int amount = pane.dock_pos - offset;
                if (amount >= 0)
                    offset += amount;
                     else
                    pane.dock_pos += -amount;

                offset += pane_sizes[j];
            }
        }
    }

    // discover the maximum dock layer
    int max_layer = 0;
    for (i = 0; i < dock_count; ++i)
        max_layer = wxMax(max_layer, docks.Item(i).dock_layer);


    // clear out uiparts
    uiparts.Empty();

    // create a bunch of box sizers,
    // from the innermost level outwards.
    wxSizer* cont = NULL;
    wxSizer* middle = NULL;
    int layer = 0;
    int row, row_count;

    for (layer = 0; layer <= max_layer; ++layer)
    {
        wxAuiDockInfoPtrArray arr;

        // find any docks in this layer
        FindDocks(docks, -1, layer, -1, arr);

        // if there aren't any, skip to the next layer
        if (arr.IsEmpty())
            continue;

        wxSizer* old_cont = cont;

        // create a container which will hold this layer's
        // docks (top, bottom, left, right)
        cont = new wxBoxSizer(wxVERTICAL);


        // find any top docks in this layer
        FindDocks(docks, wxAUI_DOCK_TOP, layer, -1, arr);
        RenumberDockRows(arr);
        if (!arr.IsEmpty())
        {
            for (row = 0, row_count = arr.GetCount(); row < row_count; ++row)
                LayoutAddDock(cont, *arr.Item(row), uiparts, spacer_only);
        }


        // fill out the middle layer (which consists
        // of left docks, content area and right docks)

        middle = new wxBoxSizer(wxHORIZONTAL);

        // find any left docks in this layer
        FindDocks(docks, wxAUI_DOCK_LEFT, layer, -1, arr);
        RenumberDockRows(arr);
        if (!arr.IsEmpty())
        {
            for (row = 0, row_count = arr.GetCount(); row < row_count; ++row)
                LayoutAddDock(middle, *arr.Item(row), uiparts, spacer_only);
        }

        // add content dock (or previous layer's sizer
        // to the middle
        if (!old_cont)
        {
            // find any center docks
            FindDocks(docks, wxAUI_DOCK_CENTER, -1, -1, arr);
            if (!arr.IsEmpty())
            {
                for (row = 0,row_count = arr.GetCount(); row<row_count; ++row)
                   LayoutAddDock(middle, *arr.Item(row), uiparts, spacer_only);
            }
             else
            {
                // there are no center docks, add a background area
                wxSizerItem* sizer_item = middle->Add(1,1, 1, wxEXPAND);
                wxAuiDockUIPart part;
                part.type = wxAuiDockUIPart::typeBackground;
                part.pane = NULL;
                part.dock = NULL;
                part.button = NULL;
                part.cont_sizer = middle;
                part.sizer_item = sizer_item;
                uiparts.Add(part);
            }
        }
         else
        {
            middle->Add(old_cont, 1, wxEXPAND);
        }

        // find any right docks in this layer
        FindDocks(docks, wxAUI_DOCK_RIGHT, layer, -1, arr);
        RenumberDockRows(arr);
        if (!arr.IsEmpty())
        {
            for (row = arr.GetCount()-1; row >= 0; --row)
                LayoutAddDock(middle, *arr.Item(row), uiparts, spacer_only);
        }

        cont->Add(middle, 1, wxEXPAND);



        // find any bottom docks in this layer
        FindDocks(docks, wxAUI_DOCK_BOTTOM, layer, -1, arr);
        RenumberDockRows(arr);
        if (!arr.IsEmpty())
        {
            for (row = arr.GetCount()-1; row >= 0; --row)
                LayoutAddDock(cont, *arr.Item(row), uiparts, spacer_only);
        }

    }

    if (!cont)
    {
        // no sizer available, because there are no docks,
        // therefore we will create a simple background area
        cont = new wxBoxSizer(wxVERTICAL);
        wxSizerItem* sizer_item = cont->Add(1,1, 1, wxEXPAND);
        wxAuiDockUIPart part;
        part.type = wxAuiDockUIPart::typeBackground;
        part.pane = NULL;
        part.dock = NULL;
        part.button = NULL;
        part.cont_sizer = middle;
        part.sizer_item = sizer_item;
        uiparts.Add(part);
    }

    container->Add(cont, 1, wxEXPAND);
    return container;
}


// Update() updates the layout.  Whenever changes are made to
// one or more panes, this function should be called.  It is the
// external entry point for running the layout engine.

void wxAuiManager::Update()
{
    wxSizer* sizer;
    int i, pane_count = m_panes.GetCount();

    // delete old sizer first
    m_frame->SetSizer(NULL);

    // destroy floating panes which have been
    // redocked or are becoming non-floating
    for (i = 0; i < pane_count; ++i)
    {
        wxAuiPaneInfo& p = m_panes.Item(i);

        if (!p.IsFloating() && p.frame)
        {
            // because the pane is no longer in a floating, we need to
            // reparent it to m_frame and destroy the floating frame

            // reduce flicker
            p.window->SetSize(1,1);
            p.frame->Show(false);

            // reparent to m_frame and destroy the pane
            p.window->Reparent(m_frame);
            p.frame->SetSizer(NULL);
            p.frame->Destroy();
            p.frame = NULL;
        }
    }


    // create a layout for all of the panes
    sizer = LayoutAll(m_panes, m_docks, m_uiparts, false);

    // hide or show panes as necessary,
    // and float panes as necessary
    for (i = 0; i < pane_count; ++i)
    {
        wxAuiPaneInfo& p = m_panes.Item(i);

        if (p.IsFloating())
        {
            if (p.frame == NULL)
            {
                // we need to create a frame for this
                // pane, which has recently been floated
                wxAuiFloatingFrame* frame = new wxAuiFloatingFrame(m_frame,
                                                  this,
                                                  p);

#if wxCHECK_VERSION(2,7,0)
                // on MSW and Mac, if the owner desires transparent dragging, and
                // the dragging is happening right now, then the floating
                // window should have this style by default
                if (m_action == actionDragFloatingPane &&
                    (m_flags & wxAUI_MGR_TRANSPARENT_DRAG))
                        frame->SetTransparent(150);
#endif

                frame->SetPaneWindow(p);
                p.frame = frame;

                if (p.frame->IsShown() != p.IsShown())
                    p.frame->Show(p.IsShown());

            }
             else
            {
                // frame already exists, make sure it's position
                // and size reflect the information in wxAuiPaneInfo
                if (p.frame->GetPosition() != p.floating_pos)
                {
                    p.frame->SetSize(p.floating_pos.x, p.floating_pos.y,
                                     wxDefaultCoord, wxDefaultCoord,
                                     wxSIZE_USE_EXISTING);
                    //p.frame->Move(p.floating_pos.x, p.floating_pos.y);
                }

                if (p.frame->IsShown() != p.IsShown())
                    p.frame->Show(p.IsShown());
            }
        }
         else
        {
            if (p.window->IsShown() != p.IsShown())
                p.window->Show(p.IsShown());
        }

        // if "active panes" are no longer allowed, clear
        // any optionActive values from the pane states
        if ((m_flags & wxAUI_MGR_ALLOW_ACTIVE_PANE) == 0)
        {
            p.state &= ~wxAuiPaneInfo::optionActive;
        }
    }


    // keep track of the old window rectangles so we can
    // refresh those windows whose rect has changed
    wxAuiRectArray old_pane_rects;
    for (i = 0; i < pane_count; ++i)
    {
        wxRect r;
        wxAuiPaneInfo& p = m_panes.Item(i);

        if (p.window && p.IsShown() && p.IsDocked())
            r = p.rect;

        old_pane_rects.Add(r);
    }




    // apply the new sizer
    m_frame->SetSizer(sizer);
    m_frame->SetAutoLayout(false);
    DoFrameLayout();



    // now that the frame layout is done, we need to check
    // the new pane rectangles against the old rectangles that
    // we saved a few lines above here.  If the rectangles have
    // changed, the corresponding panes must also be updated
    for (i = 0; i < pane_count; ++i)
    {
        wxAuiPaneInfo& p = m_panes.Item(i);
        if (p.window && p.window->IsShown() && p.IsDocked())
        {
            if (p.rect != old_pane_rects[i])
            {
                p.window->Refresh();
                p.window->Update();
            }
        }
    }


    Repaint();

    // set frame's minimum size

/*
    // N.B. More work needs to be done on frame minimum sizes;
    // this is some intresting code that imposes the minimum size,
    // but we may want to include a more flexible mechanism or
    // options for multiple minimum-size modes, e.g. strict or lax
    wxSize min_size = sizer->GetMinSize();
    wxSize frame_size = m_frame->GetSize();
    wxSize client_size = m_frame->GetClientSize();

    wxSize minframe_size(min_size.x+frame_size.x-client_size.x,
                         min_size.y+frame_size.y-client_size.y );

    m_frame->SetMinSize(minframe_size);

    if (frame_size.x < minframe_size.x ||
        frame_size.y < minframe_size.y)
            sizer->Fit(m_frame);
*/
}


// DoFrameLayout() is an internal function which invokes wxSizer::Layout
// on the frame's main sizer, then measures all the various UI items
// and updates their internal rectangles.  This should always be called
// instead of calling m_frame->Layout() directly

void wxAuiManager::DoFrameLayout()
{
    m_frame->Layout();

    int i, part_count;
    for (i = 0, part_count = m_uiparts.GetCount(); i < part_count; ++i)
    {
        wxAuiDockUIPart& part = m_uiparts.Item(i);

        // get the rectangle of the UI part
        // originally, this code looked like this:
        //    part.rect = wxRect(part.sizer_item->GetPosition(),
        //                       part.sizer_item->GetSize());
        // this worked quite well, with one exception: the mdi
        // client window had a "deferred" size variable
        // that returned the wrong size.  It looks like
        // a bug in wx, because the former size of the window
        // was being returned.  So, we will retrieve the part's
        // rectangle via other means


        part.rect = part.sizer_item->GetRect();
        int flag = part.sizer_item->GetFlag();
        int border = part.sizer_item->GetBorder();
        if (flag & wxTOP)
        {
            part.rect.y -= border;
            part.rect.height += border;
        }
        if (flag & wxLEFT)
        {
            part.rect.x -= border;
            part.rect.width += border;
        }
        if (flag & wxBOTTOM)
            part.rect.height += border;
        if (flag & wxRIGHT)
            part.rect.width += border;


        if (part.type == wxAuiDockUIPart::typeDock)
            part.dock->rect = part.rect;
        if (part.type == wxAuiDockUIPart::typePane)
            part.pane->rect = part.rect;
    }
}

// GetPanePart() looks up the pane the pane border UI part (or the regular
// pane part if there is no border). This allows the caller to get the exact
// rectangle of the pane in question, including decorations like
// caption and border (if any).

wxAuiDockUIPart* wxAuiManager::GetPanePart(wxWindow* wnd)
{
    int i, part_count;
    for (i = 0, part_count = m_uiparts.GetCount(); i < part_count; ++i)
    {
        wxAuiDockUIPart& part = m_uiparts.Item(i);
        if (part.type == wxAuiDockUIPart::typePaneBorder &&
            part.pane && part.pane->window == wnd)
                return &part;
    }
    for (i = 0, part_count = m_uiparts.GetCount(); i < part_count; ++i)
    {
        wxAuiDockUIPart& part = m_uiparts.Item(i);
        if (part.type == wxAuiDockUIPart::typePane &&
            part.pane && part.pane->window == wnd)
                return &part;
    }
    return NULL;
}



// GetDockPixelOffset() is an internal function which returns
// a dock's offset in pixels from the left side of the window
// (for horizontal docks) or from the top of the window (for
// vertical docks).  This value is necessary for calculating
// fixel-pane/toolbar offsets when they are dragged.

int wxAuiManager::GetDockPixelOffset(wxAuiPaneInfo& test)
{
    // the only way to accurately calculate the dock's
    // offset is to actually run a theoretical layout

    int i, part_count, dock_count;
    wxAuiDockInfoArray docks;
    wxAuiPaneInfoArray panes;
    wxAuiDockUIPartArray uiparts;
    CopyDocksAndPanes(docks, panes, m_docks, m_panes);
    panes.Add(test);

    wxSizer* sizer = LayoutAll(panes, docks, uiparts, true);
    wxSize client_size = m_frame->GetClientSize();
    sizer->SetDimension(0, 0, client_size.x, client_size.y);
    sizer->Layout();

    for (i = 0, part_count = uiparts.GetCount(); i < part_count; ++i)
    {
        wxAuiDockUIPart& part = uiparts.Item(i);
        part.rect = wxRect(part.sizer_item->GetPosition(),
                           part.sizer_item->GetSize());
        if (part.type == wxAuiDockUIPart::typeDock)
            part.dock->rect = part.rect;
    }

    delete sizer;

    for (i = 0, dock_count = docks.GetCount(); i < dock_count; ++i)
    {
        wxAuiDockInfo& dock = docks.Item(i);
        if (test.dock_direction == dock.dock_direction &&
            test.dock_layer==dock.dock_layer && test.dock_row==dock.dock_row)
        {
            if (dock.IsVertical())
                return dock.rect.y;
                 else
                return dock.rect.x;
        }
    }

    return 0;
}



// ProcessDockResult() is a utility function used by DoDrop() - it checks
// if a dock operation is allowed, the new dock position is copied into
// the target info.  If the operation was allowed, the function returns true.

bool wxAuiManager::ProcessDockResult(wxAuiPaneInfo& target,
                              const wxAuiPaneInfo& new_pos)
{
    bool allowed = false;
    switch (new_pos.dock_direction)
    {
        case wxAUI_DOCK_TOP:    allowed = target.IsTopDockable();    break;
        case wxAUI_DOCK_BOTTOM: allowed = target.IsBottomDockable(); break;
        case wxAUI_DOCK_LEFT:   allowed = target.IsLeftDockable();   break;
        case wxAUI_DOCK_RIGHT:  allowed = target.IsRightDockable();  break;
    }

    if (allowed)
        target = new_pos;

    return allowed;
}


// DoDrop() is an important function.  It basically takes a mouse position,
// and determines where the pane's new position would be.  If the pane is to be
// dropped, it performs the drop operation using the specified dock and pane
// arrays.  By specifying copied dock and pane arrays when calling, a "what-if"
// scenario can be performed, giving precise coordinates for drop hints.
// If, however, wxAuiManager:m_docks and wxAuiManager::m_panes are specified
// as parameters, the changes will be made to the main state arrays

const int auiInsertRowPixels = 10;
const int auiNewRowPixels = 40;
const int auiLayerInsertPixels = 40;
const int auiLayerInsertOffset = 5;

bool wxAuiManager::DoDrop(wxAuiDockInfoArray& docks,
                            wxAuiPaneInfoArray& panes,
                            wxAuiPaneInfo& target,
                            const wxPoint& pt,
                            const wxPoint& offset)
{
    wxSize cli_size = m_frame->GetClientSize();

    wxAuiPaneInfo drop = target;


    // The result should always be shown
    drop.Show();


    // Check to see if the pane has been dragged outside of the window
    // (or near to the outside of the window), if so, dock it along the edge


    int layer_insert_offset = auiLayerInsertOffset;
    if (target.IsToolbar())
        layer_insert_offset = 0;

    if (pt.x < layer_insert_offset &&
        pt.x > layer_insert_offset-auiLayerInsertPixels)
    {
        int new_layer = wxMax(wxMax(GetMaxLayer(docks, wxAUI_DOCK_LEFT),
                                    GetMaxLayer(docks, wxAUI_DOCK_BOTTOM)),
                                    GetMaxLayer(docks, wxAUI_DOCK_TOP)) + 1;
        drop.Dock().Left().
             Layer(new_layer).
             Row(0).
             Position(pt.y - GetDockPixelOffset(drop) - offset.y);
        return ProcessDockResult(target, drop);
    }
    else if (pt.y < layer_insert_offset &&
             pt.y > layer_insert_offset-auiLayerInsertPixels)
    {
        int new_layer = wxMax(wxMax(GetMaxLayer(docks, wxAUI_DOCK_TOP),
                                    GetMaxLayer(docks, wxAUI_DOCK_LEFT)),
                                    GetMaxLayer(docks, wxAUI_DOCK_RIGHT)) + 1;
        drop.Dock().Top().
             Layer(new_layer).
             Row(0).
             Position(pt.x - GetDockPixelOffset(drop) - offset.x);
        return ProcessDockResult(target, drop);
    }
    else if (pt.x >= cli_size.x - layer_insert_offset &&
             pt.x < cli_size.x - layer_insert_offset + auiLayerInsertPixels)
    {
        int new_layer = wxMax(wxMax(GetMaxLayer(docks, wxAUI_DOCK_RIGHT),
                                    GetMaxLayer(docks, wxAUI_DOCK_TOP)),
                                    GetMaxLayer(docks, wxAUI_DOCK_BOTTOM)) + 1;
        drop.Dock().Right().
             Layer(new_layer).
             Row(0).
             Position(pt.y - GetDockPixelOffset(drop) - offset.y);
        return ProcessDockResult(target, drop);
    }
    else if (pt.y >= cli_size.y - layer_insert_offset &&
             pt.y < cli_size.y - layer_insert_offset + auiLayerInsertPixels)
    {
        int new_layer = wxMax(wxMax(GetMaxLayer(docks, wxAUI_DOCK_BOTTOM),
                                    GetMaxLayer(docks, wxAUI_DOCK_LEFT)),
                                    GetMaxLayer(docks, wxAUI_DOCK_RIGHT)) + 1;
        drop.Dock().Bottom().
             Layer(new_layer).
             Row(0).
             Position(pt.x - GetDockPixelOffset(drop) - offset.x);
        return ProcessDockResult(target, drop);
    }


    wxAuiDockUIPart* part = HitTest(pt.x, pt.y);


    if (drop.IsToolbar())
    {
        if (!part || !part->dock)
            return false;


        // calculate the offset from where the dock begins
        // to the point where the user dropped the pane
        int dock_drop_offset = 0;
        if (part->dock->IsHorizontal())
            dock_drop_offset = pt.x - part->dock->rect.x - offset.x;
             else
            dock_drop_offset = pt.y - part->dock->rect.y - offset.y;


        // toolbars may only be moved in and to fixed-pane docks,
        // otherwise we will try to float the pane.  Also, the pane
        // should float if being dragged over center pane windows
        if (!part->dock->fixed || part->dock->dock_direction == wxAUI_DOCK_CENTER)
        {
            if ((m_flags & wxAUI_MGR_ALLOW_FLOATING) &&
                   (drop.IsFloatable() ||
                    (part->dock->dock_direction != wxAUI_DOCK_CENTER &&
                     part->dock->dock_direction != wxAUI_DOCK_NONE)))
            {
                drop.Float();
            }

            return ProcessDockResult(target, drop);
        }

        drop.Dock().
             Direction(part->dock->dock_direction).
             Layer(part->dock->dock_layer).
             Row(part->dock->dock_row).
             Position(dock_drop_offset);

        if ((
            ((pt.y < part->dock->rect.y + 2) && part->dock->IsHorizontal()) ||
            ((pt.x < part->dock->rect.x + 2) && part->dock->IsVertical())
            ) && part->dock->panes.GetCount() > 1)
        {
            int row = drop.dock_row;
            DoInsertDockRow(panes, part->dock->dock_direction,
                            part->dock->dock_layer,
                            part->dock->dock_row);
            drop.dock_row = row;
        }

        if ((
            ((pt.y > part->dock->rect.y + part->dock->rect.height - 2 ) && part->dock->IsHorizontal()) ||
            ((pt.x > part->dock->rect.x + part->dock->rect.width - 2 ) && part->dock->IsVertical())
            ) && part->dock->panes.GetCount() > 1)
        {
            DoInsertDockRow(panes, part->dock->dock_direction,
                            part->dock->dock_layer,
                            part->dock->dock_row+1);
            drop.dock_row = part->dock->dock_row+1;
        }

        return ProcessDockResult(target, drop);
    }




    if (!part)
        return false;

    if (part->type == wxAuiDockUIPart::typePaneBorder ||
        part->type == wxAuiDockUIPart::typeCaption ||
        part->type == wxAuiDockUIPart::typeGripper ||
        part->type == wxAuiDockUIPart::typePaneButton ||
        part->type == wxAuiDockUIPart::typePane ||
        part->type == wxAuiDockUIPart::typePaneSizer ||
        part->type == wxAuiDockUIPart::typeDockSizer ||
        part->type == wxAuiDockUIPart::typeBackground)
    {
        if (part->type == wxAuiDockUIPart::typeDockSizer)
        {
            if (part->dock->panes.GetCount() != 1)
                return false;
            part = GetPanePart(part->dock->panes.Item(0)->window);
            if (!part)
                return false;
        }



        // If a normal frame is being dragged over a toolbar, insert it
        // along the edge under the toolbar, but over all other panes.
        // (this could be done much better, but somehow factoring this
        // calculation with the one at the beginning of this function)
        if (part->dock && part->dock->toolbar)
        {
            int layer = 0;

            switch (part->dock->dock_direction)
            {
                case wxAUI_DOCK_LEFT:
                    layer = wxMax(wxMax(GetMaxLayer(docks, wxAUI_DOCK_LEFT),
                                      GetMaxLayer(docks, wxAUI_DOCK_BOTTOM)),
                                      GetMaxLayer(docks, wxAUI_DOCK_TOP));
                    break;
                case wxAUI_DOCK_TOP:
                    layer = wxMax(wxMax(GetMaxLayer(docks, wxAUI_DOCK_TOP),
                                      GetMaxLayer(docks, wxAUI_DOCK_LEFT)),
                                      GetMaxLayer(docks, wxAUI_DOCK_RIGHT));
                    break;
                case wxAUI_DOCK_RIGHT:
                    layer = wxMax(wxMax(GetMaxLayer(docks, wxAUI_DOCK_RIGHT),
                                      GetMaxLayer(docks, wxAUI_DOCK_TOP)),
                                      GetMaxLayer(docks, wxAUI_DOCK_BOTTOM));
                    break;
                case wxAUI_DOCK_BOTTOM:
                    layer = wxMax(wxMax(GetMaxLayer(docks, wxAUI_DOCK_BOTTOM),
                                      GetMaxLayer(docks, wxAUI_DOCK_LEFT)),
                                      GetMaxLayer(docks, wxAUI_DOCK_RIGHT));
                    break;
            }

            DoInsertDockRow(panes, part->dock->dock_direction,
                            layer, 0);
            drop.Dock().
                 Direction(part->dock->dock_direction).
                 Layer(layer).Row(0).Position(0);
            return ProcessDockResult(target, drop);
        }


        if (!part->pane)
            return false;

        part = GetPanePart(part->pane->window);
        if (!part)
            return false;

        bool insert_dock_row = false;
        int insert_row = part->pane->dock_row;
        int insert_dir = part->pane->dock_direction;
        int insert_layer = part->pane->dock_layer;

        switch (part->pane->dock_direction)
        {
            case wxAUI_DOCK_TOP:
                if (pt.y >= part->rect.y &&
                    pt.y < part->rect.y+auiInsertRowPixels)
                        insert_dock_row = true;
                break;
            case wxAUI_DOCK_BOTTOM:
                if (pt.y > part->rect.y+part->rect.height-auiInsertRowPixels &&
                    pt.y <= part->rect.y + part->rect.height)
                        insert_dock_row = true;
                break;
            case wxAUI_DOCK_LEFT:
                if (pt.x >= part->rect.x &&
                    pt.x < part->rect.x+auiInsertRowPixels)
                        insert_dock_row = true;
                break;
            case wxAUI_DOCK_RIGHT:
                if (pt.x > part->rect.x+part->rect.width-auiInsertRowPixels &&
                    pt.x <= part->rect.x+part->rect.width)
                        insert_dock_row = true;
                break;
            case wxAUI_DOCK_CENTER:
            {
                // "new row pixels" will be set to the default, but
                // must never exceed 20% of the window size
                int new_row_pixels_x = auiNewRowPixels;
                int new_row_pixels_y = auiNewRowPixels;

                if (new_row_pixels_x > (part->rect.width*20)/100)
                    new_row_pixels_x = (part->rect.width*20)/100;

                if (new_row_pixels_y > (part->rect.height*20)/100)
                    new_row_pixels_y = (part->rect.height*20)/100;


                // determine if the mouse pointer is in a location that
                // will cause a new row to be inserted.  The hot spot positions
                // are along the borders of the center pane

                insert_layer = 0;
                insert_dock_row = true;
                if (pt.x >= part->rect.x &&
                    pt.x < part->rect.x+new_row_pixels_x)
                        insert_dir = wxAUI_DOCK_LEFT;
                 else
                if (pt.y >= part->rect.y &&
                    pt.y < part->rect.y+new_row_pixels_y)
                        insert_dir = wxAUI_DOCK_TOP;
                 else
                if (pt.x >= part->rect.x + part->rect.width-new_row_pixels_x &&
                    pt.x < part->rect.x + part->rect.width)
                        insert_dir = wxAUI_DOCK_RIGHT;
                 else
                if (pt.y >= part->rect.y+ part->rect.height-new_row_pixels_y &&
                    pt.y < part->rect.y + part->rect.height)
                        insert_dir = wxAUI_DOCK_BOTTOM;
                 else
                return false;

                insert_row = GetMaxRow(panes, insert_dir, insert_layer) + 1;
            }
        }

        if (insert_dock_row)
        {
            DoInsertDockRow(panes, insert_dir, insert_layer, insert_row);
            drop.Dock().Direction(insert_dir).
                        Layer(insert_layer).
                        Row(insert_row).
                        Position(0);
            return ProcessDockResult(target, drop);
        }

        // determine the mouse offset and the pane size, both in the
        // direction of the dock itself, and perpendicular to the dock

        int offset, size;

        if (part->orientation == wxVERTICAL)
        {
            offset = pt.y - part->rect.y;
            size = part->rect.GetHeight();
        }
         else
        {
            offset = pt.x - part->rect.x;
            size = part->rect.GetWidth();
        }

        int drop_position = part->pane->dock_pos;

        // if we are in the top/left part of the pane,
        // insert the pane before the pane being hovered over
        if (offset <= size/2)
        {
            drop_position = part->pane->dock_pos;
            DoInsertPane(panes,
                         part->pane->dock_direction,
                         part->pane->dock_layer,
                         part->pane->dock_row,
                         part->pane->dock_pos);
        }

        // if we are in the bottom/right part of the pane,
        // insert the pane before the pane being hovered over
        if (offset > size/2)
        {
            drop_position = part->pane->dock_pos+1;
            DoInsertPane(panes,
                         part->pane->dock_direction,
                         part->pane->dock_layer,
                         part->pane->dock_row,
                         part->pane->dock_pos+1);
        }

        drop.Dock().
             Direction(part->dock->dock_direction).
             Layer(part->dock->dock_layer).
             Row(part->dock->dock_row).
             Position(drop_position);
        return ProcessDockResult(target, drop);
    }

    return false;
}


void wxAuiManager::OnHintFadeTimer(wxTimerEvent& WXUNUSED(event))
{
    if (!m_hint_wnd || m_hint_fadeamt >= m_hint_fademax)
    {
        m_hint_fadetimer.Stop();
        return;
    }

    m_hint_fadeamt += 4;
#if wxCHECK_VERSION(2,7,0)
    m_hint_wnd->SetTransparent(m_hint_fadeamt);
#else
    if (m_hint_wnd->IsKindOf(CLASSINFO(wxPseudoTransparentFrame)))
        ((wxPseudoTransparentFrame *)m_hint_wnd)->SetTransparent(m_hint_fadeamt);
#endif
}

void wxAuiManager::ShowHint(const wxRect& rect)
{
    if ((m_flags & wxAUI_MGR_TRANSPARENT_HINT) != 0
        && m_hint_wnd
        // Finally, don't use a venetian blind effect if it's been specifically disabled
        && !((m_hint_wnd->IsKindOf(CLASSINFO(wxPseudoTransparentFrame))) &&
             (m_flags & wxAUI_MGR_DISABLE_VENETIAN_BLINDS))
       )
    {
        if (m_last_hint == rect)
            return;
        m_last_hint = rect;

        m_hint_fadeamt = m_hint_fademax;
        if ((m_flags & wxAUI_MGR_TRANSPARENT_HINT_FADE)
            && !((m_hint_wnd->IsKindOf(CLASSINFO(wxPseudoTransparentFrame))) &&
                 (m_flags & wxAUI_MGR_DISABLE_VENETIAN_BLINDS_FADE))
            )
            m_hint_fadeamt = 0;

        if (! m_hint_wnd->IsShown())
            m_hint_wnd->Show();

        // if we are dragging a floating pane, set the focus
        // back to that floating pane (otherwise it becomes unfocused)
        if (m_action == actionDragFloatingPane && m_action_window)
            m_action_window->SetFocus();

#if wxCHECK_VERSION(2,7,0)
        m_hint_wnd->SetTransparent(m_hint_fadeamt);
#else
        if (m_hint_wnd->IsKindOf(CLASSINFO(wxPseudoTransparentFrame)))
            ((wxPseudoTransparentFrame *)m_hint_wnd)->SetTransparent(m_hint_fadeamt);
#endif
        m_hint_wnd->SetSize(rect);
        m_hint_wnd->Raise();


        if (m_hint_fadeamt != m_hint_fademax) //  Only fade if we need to
        {
            // start fade in timer
            m_hint_fadetimer.SetOwner(this, 101);
            m_hint_fadetimer.Start(5);
        }
    }

    else  // Not using a transparent hint window...
    {

        if (m_last_hint != rect)
        {
            // remove the last hint rectangle
            m_last_hint = rect;
            m_frame->Refresh();
            m_frame->Update();
        }

        wxScreenDC screendc;
        wxRegion clip(1, 1, 10000, 10000);

        // clip all floating windows, so we don't draw over them
        int i, pane_count;
        for (i = 0, pane_count = m_panes.GetCount(); i < pane_count; ++i)
        {
            wxAuiPaneInfo& pane = m_panes.Item(i);

            if (pane.IsFloating() &&
                pane.frame->IsShown())
            {
                wxRect rect = pane.frame->GetRect();
#ifdef __WXGTK__
                // wxGTK returns the client size, not the whole frame size
                rect.width += 15;
                rect.height += 35;
                rect.Inflate(5);
#endif

                clip.Subtract(rect);
            }
        }

        // As we can only hide the hint by redrawing the managed window, we
        // need to clip the region to the managed window too or we get
        // nasty redrawn problems.
        clip.Intersect(m_frame->GetRect());

        screendc.SetClippingRegion(clip);

        wxBitmap stipple = wxPaneCreateStippleBitmap();
        wxBrush brush(stipple);
        screendc.SetBrush(brush);
        screendc.SetPen(*wxTRANSPARENT_PEN);

        screendc.DrawRectangle(rect.x, rect.y, 5, rect.height);
        screendc.DrawRectangle(rect.x+5, rect.y, rect.width-10, 5);
        screendc.DrawRectangle(rect.x+rect.width-5, rect.y, 5, rect.height);
        screendc.DrawRectangle(rect.x+5, rect.y+rect.height-5, rect.width-10, 5);
    }
}

void wxAuiManager::HideHint()
{
    // hides a transparent window hint, if there is one
    if (m_hint_wnd)
    {
        m_hint_wnd->Show(false);
#if wxCHECK_VERSION(2,7,0)
        m_hint_wnd->SetTransparent(0);
#else
        if (m_hint_wnd->IsKindOf(CLASSINFO(wxPseudoTransparentFrame)))
        ((wxPseudoTransparentFrame *)m_hint_wnd)->SetTransparent(0);
#endif
        m_hint_fadetimer.Stop();
        m_last_hint = wxRect();
        return;
    }

    // hides a painted hint by redrawing the frame window
    if (!m_last_hint.IsEmpty())
    {
        m_frame->Refresh();
        m_frame->Update();
        m_last_hint = wxRect();
    }
}



// DrawHintRect() draws a drop hint rectangle. First calls DoDrop() to
// determine the exact position the pane would be at were if dropped.  If
// the pame would indeed become docked at the specified drop point,
// DrawHintRect() then calls ShowHint() to indicate this drop rectangle.
// "pane_window" is the window pointer of the pane being dragged, pt is
// the mouse position, in client coordinates
void wxAuiManager::DrawHintRect(wxWindow* pane_window,
                                  const wxPoint& pt,
                                  const wxPoint& offset)
{
    wxRect rect;

    // we need to paint a hint rectangle; to find out the exact hint rectangle,
    // we will create a new temporary layout and then measure the resulting
    // rectangle; we will create a copy of the docking structures (m_dock)
    // so that we don't modify the real thing on screen

    int i, pane_count, part_count;
    wxAuiDockInfoArray docks;
    wxAuiPaneInfoArray panes;
    wxAuiDockUIPartArray uiparts;
    wxAuiPaneInfo hint = GetPane(pane_window);
    hint.name = wxT("__HINT__");
    hint.Show();

    if (!hint.IsOk())
        return;

    CopyDocksAndPanes(docks, panes, m_docks, m_panes);

    // remove any pane already there which bears the same window;
    // this happens when you are moving a pane around in a dock
    for (i = 0, pane_count = panes.GetCount(); i < pane_count; ++i)
    {
        if (panes.Item(i).window == pane_window)
        {
            RemovePaneFromDocks(docks, panes.Item(i));
            panes.RemoveAt(i);
            break;
        }
    }

    // find out where the new pane would be
    if (!DoDrop(docks, panes, hint, pt, offset))
    {
        HideHint();
        return;
    }

    panes.Add(hint);

    wxSizer* sizer = LayoutAll(panes, docks, uiparts, true);
    wxSize client_size = m_frame->GetClientSize();
    sizer->SetDimension(0, 0, client_size.x, client_size.y);
    sizer->Layout();

    for (i = 0, part_count = uiparts.GetCount();
         i < part_count; ++i)
    {
        wxAuiDockUIPart& part = uiparts.Item(i);

        if (part.type == wxAuiDockUIPart::typePaneBorder &&
            part.pane && part.pane->name == wxT("__HINT__"))
        {
            rect = wxRect(part.sizer_item->GetPosition(),
                          part.sizer_item->GetSize());
            break;
        }
    }

    delete sizer;

    if (rect.IsEmpty())
    {
        HideHint();
        return;
    }

    // actually show the hint rectangle on the screen
    m_frame->ClientToScreen(&rect.x, &rect.y);
    ShowHint(rect);
}

void wxAuiManager::OnFloatingPaneMoveStart(wxWindow* wnd)
{
    // try to find the pane
    wxAuiPaneInfo& pane = GetPane(wnd);
    wxASSERT_MSG(pane.IsOk(), wxT("Pane window not found"));

#if wxCHECK_VERSION(2,7,0)
    if (m_flags & wxAUI_MGR_TRANSPARENT_DRAG)
        pane.frame->SetTransparent(150);
#endif
}

void wxAuiManager::OnFloatingPaneMoving(wxWindow* wnd)
{
    // try to find the pane
    wxAuiPaneInfo& pane = GetPane(wnd);
    wxASSERT_MSG(pane.IsOk(), wxT("Pane window not found"));

    wxPoint pt = ::wxGetMousePosition();
    wxPoint client_pt = m_frame->ScreenToClient(pt);

    // calculate the offset from the upper left-hand corner
    // of the frame to the mouse pointer
    wxPoint frame_pos = pane.frame->GetPosition();
    wxPoint action_offset(pt.x-frame_pos.x, pt.y-frame_pos.y);

    // no hint for toolbar floating windows
    if (pane.IsToolbar() && m_action == actionDragFloatingPane)
    {
        if (m_action == actionDragFloatingPane)
        {
            wxAuiDockInfoArray docks;
            wxAuiPaneInfoArray panes;
            wxAuiDockUIPartArray uiparts;
            wxAuiPaneInfo hint = pane;

            CopyDocksAndPanes(docks, panes, m_docks, m_panes);

            // find out where the new pane would be
            if (!DoDrop(docks, panes, hint, client_pt))
                return;
            if (hint.IsFloating())
                return;

            pane = hint;
            m_action = actionDragToolbarPane;
            m_action_window = pane.window;

            Update();
        }

        return;
    }


    // if a key modifier is pressed while dragging the frame,
    // don't dock the window
    if (wxGetKeyState(WXK_CONTROL) || wxGetKeyState(WXK_ALT))
    {
        HideHint();
        return;
    }


    DrawHintRect(wnd, client_pt, action_offset);

#ifdef __WXGTK__
    // this cleans up some screen artifacts that are caused on GTK because
    // we aren't getting the exact size of the window (see comment
    // in DrawHintRect)
    //Refresh();
#endif


    // reduces flicker
    m_frame->Update();
}

void wxAuiManager::OnFloatingPaneMoved(wxWindow* wnd)
{
    // try to find the pane
    wxAuiPaneInfo& pane = GetPane(wnd);
    wxASSERT_MSG(pane.IsOk(), wxT("Pane window not found"));

    wxPoint pt = ::wxGetMousePosition();
    wxPoint client_pt = m_frame->ScreenToClient(pt);

    // calculate the offset from the upper left-hand corner
    // of the frame to the mouse pointer
    wxPoint frame_pos = pane.frame->GetPosition();
    wxPoint action_offset(pt.x-frame_pos.x, pt.y-frame_pos.y);


    // if a key modifier is pressed while dragging the frame,
    // don't dock the window
    if (!wxGetKeyState(WXK_CONTROL) && !wxGetKeyState(WXK_ALT))
    {
        // do the drop calculation
        DoDrop(m_docks, m_panes, pane, client_pt, action_offset);
    }

    // if the pane is still floating, update it's floating
    // position (that we store)
    if (pane.IsFloating())
    {
        pane.floating_pos = pane.frame->GetPosition();

#if wxCHECK_VERSION(2,7,0)
        if (m_flags & wxAUI_MGR_TRANSPARENT_DRAG)
            pane.frame->SetTransparent(255);
#endif
    }

    Update();

    HideHint();
}

void wxAuiManager::OnFloatingPaneResized(wxWindow* wnd, const wxSize& size)
{
    // try to find the pane
    wxAuiPaneInfo& pane = GetPane(wnd);
    wxASSERT_MSG(pane.IsOk(), wxT("Pane window not found"));

    pane.floating_size = size;
}


void wxAuiManager::OnFloatingPaneClosed(wxWindow* wnd, wxCloseEvent& evt)
{
    // try to find the pane
    wxAuiPaneInfo& pane = GetPane(wnd);
    wxASSERT_MSG(pane.IsOk(), wxT("Pane window not found"));


    // fire pane close event
    wxAuiManagerEvent e(wxEVT_AUI_PANE_CLOSE);
    e.SetPane(&pane);
    e.SetCanVeto(evt.CanVeto());
    ProcessMgrEvent(e);

    if (e.GetVeto())
    {
        evt.Veto();
        return;
    }
     else
    {
        // reparent the pane window back to us and
        // prepare the frame window for destruction
        pane.window->Show(false);
        pane.window->Reparent(m_frame);
        pane.frame = NULL;
        pane.Hide();
    }
}



void wxAuiManager::OnFloatingPaneActivated(wxWindow* wnd)
{
    if (GetFlags() & wxAUI_MGR_ALLOW_ACTIVE_PANE)
    {
        // try to find the pane
        wxASSERT_MSG(GetPane(wnd).IsOk(), wxT("Pane window not found"));

        SetActivePane(m_panes, wnd);
        Repaint();
    }
}

// OnRender() draws all of the pane captions, sashes,
// backgrounds, captions, grippers, pane borders and buttons.
// It renders the entire user interface.

void wxAuiManager::OnRender(wxAuiManagerEvent& evt)
{
    wxDC* dc = evt.GetDC();

#ifdef __WXMAC__
    dc->Clear() ;
#endif
    int i, part_count;
    for (i = 0, part_count = m_uiparts.GetCount();
         i < part_count; ++i)
    {
        wxAuiDockUIPart& part = m_uiparts.Item(i);

        // don't draw hidden pane items
        if (part.sizer_item && !part.sizer_item->IsShown())
            continue;

        switch (part.type)
        {
            case wxAuiDockUIPart::typeDockSizer:
            case wxAuiDockUIPart::typePaneSizer:
                m_art->DrawSash(*dc, part.orientation, part.rect);
                break;
            case wxAuiDockUIPart::typeBackground:
                m_art->DrawBackground(*dc, part.orientation, part.rect);
                break;
            case wxAuiDockUIPart::typeCaption:
                m_art->DrawCaption(*dc, part.pane->caption, part.rect, *part.pane);
                break;
            case wxAuiDockUIPart::typeGripper:
                m_art->DrawGripper(*dc, part.rect, *part.pane);
                break;
            case wxAuiDockUIPart::typePaneBorder:
                m_art->DrawBorder(*dc, part.rect, *part.pane);
                break;
            case wxAuiDockUIPart::typePaneButton:
                m_art->DrawPaneButton(*dc, part.button->button_id,
                        wxAUI_BUTTON_STATE_NORMAL, part.rect, *part.pane);
                break;
        }
    }
}


// Render() fire a render event, which is normally handled by
// wxAuiManager::OnRender().  This allows the render function to
// be overridden via the render event.  This can be useful for paintin
// custom graphics in the main window. Default behavior can be
// invoked in the overridden function by calling OnRender()

void wxAuiManager::Render(wxDC* dc)
{
    wxAuiManagerEvent e(wxEVT_AUI_RENDER);
    e.SetDC(dc);
    ProcessMgrEvent(e);
}

void wxAuiManager::Repaint(wxDC* dc)
{
#ifdef __WXMAC__
    if ( dc == NULL )
    {
        m_frame->Refresh() ;
        m_frame->Update() ;
        return ;
    }
#endif
    int w, h;
    m_frame->GetClientSize(&w, &h);

    // figure out which dc to use; if one
    // has been specified, use it, otherwise
    // make a client dc
    wxClientDC* client_dc = NULL;
    if (!dc)
    {
        client_dc = new wxClientDC(m_frame);
        dc = client_dc;
    }

    // if the frame has a toolbar, the client area
    // origin will not be (0,0).
    wxPoint pt = m_frame->GetClientAreaOrigin();
    if (pt.x != 0 || pt.y != 0)
        dc->SetDeviceOrigin(pt.x, pt.y);

    // render all the items
    Render(dc);

    // if we created a client_dc, delete it
    if (client_dc)
        delete client_dc;
}

void wxAuiManager::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(m_frame);
    Repaint(&dc);
}

void wxAuiManager::OnEraseBackground(wxEraseEvent& event)
{
#ifdef __WXMAC__
    event.Skip() ;
#else
    wxUnusedVar(event);
#endif
}

void wxAuiManager::OnSize(wxSizeEvent& WXUNUSED(event))
{
    if (m_frame)
    {
        DoFrameLayout();
        Repaint();
    }
}


void wxAuiManager::OnSetCursor(wxSetCursorEvent& event)
{
    // determine cursor
    wxAuiDockUIPart* part = HitTest(event.GetX(), event.GetY());
    wxCursor cursor = wxNullCursor;

    if (part)
    {
        if (part->type == wxAuiDockUIPart::typeDockSizer ||
            part->type == wxAuiDockUIPart::typePaneSizer)
        {
            // a dock may not be resized if it has a single
            // pane which is not resizable
            if (part->type == wxAuiDockUIPart::typeDockSizer && part->dock &&
                part->dock->panes.GetCount() == 1 &&
                part->dock->panes.Item(0)->IsFixed())
                    return;

            // panes that may not be resized do not get a sizing cursor
            if (part->pane && part->pane->IsFixed())
                return;

            if (part->orientation == wxVERTICAL)
                cursor = wxCursor(wxCURSOR_SIZEWE);
                 else
                cursor = wxCursor(wxCURSOR_SIZENS);
        }
         else if (part->type == wxAuiDockUIPart::typeGripper)
        {
            cursor = wxCursor(wxCURSOR_SIZING);
        }
    }

    event.SetCursor(cursor);
}



void wxAuiManager::UpdateButtonOnScreen(wxAuiDockUIPart* button_ui_part,
                                          const wxMouseEvent& event)
{
    wxAuiDockUIPart* hit_test = HitTest(event.GetX(), event.GetY());

    int state = wxAUI_BUTTON_STATE_NORMAL;

    if (hit_test == button_ui_part)
    {
        if (event.LeftDown())
            state = wxAUI_BUTTON_STATE_PRESSED;
             else
            state = wxAUI_BUTTON_STATE_HOVER;
    }
    else
    {
        if (event.LeftDown())
            state = wxAUI_BUTTON_STATE_HOVER;
    }

    // now repaint the button with hover state
    wxClientDC cdc(m_frame);

    // if the frame has a toolbar, the client area
    // origin will not be (0,0).
    wxPoint pt = m_frame->GetClientAreaOrigin();
    if (pt.x != 0 || pt.y != 0)
        cdc.SetDeviceOrigin(pt.x, pt.y);

    m_art->DrawPaneButton(cdc,
              button_ui_part->button->button_id,
              state,
              button_ui_part->rect,
              *hit_test->pane);
}

void wxAuiManager::OnLeftDown(wxMouseEvent& event)
{
    wxAuiDockUIPart* part = HitTest(event.GetX(), event.GetY());
    if (part)
    {
        if (part->dock && part->dock->dock_direction == wxAUI_DOCK_CENTER)
            return;

        if (part->type == wxAuiDockUIPart::typeDockSizer ||
            part->type == wxAuiDockUIPart::typePaneSizer)
        {
            // a dock may not be resized if it has a single
            // pane which is not resizable
            if (part->type == wxAuiDockUIPart::typeDockSizer && part->dock &&
                part->dock->panes.GetCount() == 1 &&
                part->dock->panes.Item(0)->IsFixed())
                    return;

            // panes that may not be resized should be ignored here
            if (part->pane && part->pane->IsFixed())
                return;

            m_action = actionResize;
            m_action_part = part;
            m_action_hintrect = wxRect();
            m_action_start = wxPoint(event.m_x, event.m_y);
            m_action_offset = wxPoint(event.m_x - part->rect.x,
                                      event.m_y - part->rect.y);
            m_frame->CaptureMouse();
        }
         else if (part->type == wxAuiDockUIPart::typePaneButton)
        {
            m_action = actionClickButton;
            m_action_part = part;
            m_action_start = wxPoint(event.m_x, event.m_y);
            m_frame->CaptureMouse();

            UpdateButtonOnScreen(part, event);
        }
         else if (part->type == wxAuiDockUIPart::typeCaption ||
                  part->type == wxAuiDockUIPart::typeGripper)
        {
            if (GetFlags() & wxAUI_MGR_ALLOW_ACTIVE_PANE)
            {
                // set the caption as active
                SetActivePane(m_panes, part->pane->window);
                Repaint();
            }

            m_action = actionClickCaption;
            m_action_part = part;
            m_action_start = wxPoint(event.m_x, event.m_y);
            m_action_offset = wxPoint(event.m_x - part->rect.x,
                                      event.m_y - part->rect.y);
            m_frame->CaptureMouse();
        }
#ifdef __WXMAC__
        else
        {
            event.Skip();
        }
#endif
    }
#ifdef __WXMAC__
    else
    {
        event.Skip();
    }
#else
    event.Skip();
#endif
}


void wxAuiManager::OnLeftUp(wxMouseEvent& event)
{
    if (m_action == actionResize)
    {
        m_frame->ReleaseMouse();

        // get rid of the hint rectangle
        wxScreenDC dc;
        DrawResizeHint(dc, m_action_hintrect);

        // resize the dock or the pane
        if (m_action_part && m_action_part->type==wxAuiDockUIPart::typeDockSizer)
        {
            wxRect& rect = m_action_part->dock->rect;

            wxPoint new_pos(event.m_x - m_action_offset.x,
                            event.m_y - m_action_offset.y);

            switch (m_action_part->dock->dock_direction)
            {
                case wxAUI_DOCK_LEFT:
                    m_action_part->dock->size = new_pos.x - rect.x;
                    break;
                case wxAUI_DOCK_TOP:
                    m_action_part->dock->size = new_pos.y - rect.y;
                    break;
                case wxAUI_DOCK_RIGHT:
                    m_action_part->dock->size = rect.x + rect.width -
                                  new_pos.x - m_action_part->rect.GetWidth();
                    break;
                case wxAUI_DOCK_BOTTOM:
                    m_action_part->dock->size = rect.y + rect.height -
                                  new_pos.y - m_action_part->rect.GetHeight();
                    break;
            }

            Update();
            Repaint(NULL);
        }
         else if (m_action_part &&
                  m_action_part->type == wxAuiDockUIPart::typePaneSizer)
        {
            wxAuiDockInfo& dock = *m_action_part->dock;
            wxAuiPaneInfo& pane = *m_action_part->pane;

            int total_proportion = 0;
            int dock_pixels = 0;
            int new_pixsize = 0;

            int caption_size = m_art->GetMetric(wxAUI_ART_CAPTION_SIZE);
            int pane_border_size = m_art->GetMetric(wxAUI_ART_PANE_BORDER_SIZE);
            int sash_size = m_art->GetMetric(wxAUI_ART_SASH_SIZE);

            wxPoint new_pos(event.m_x - m_action_offset.x,
                            event.m_y - m_action_offset.y);

            // determine the pane rectangle by getting the pane part
            wxAuiDockUIPart* pane_part = GetPanePart(pane.window);
            wxASSERT_MSG(pane_part,
                       wxT("Pane border part not found -- shouldn't happen"));

            // determine the new pixel size that the user wants;
            // this will help us recalculate the pane's proportion
            if (dock.IsHorizontal())
                new_pixsize = new_pos.x - pane_part->rect.x;
                 else
                new_pixsize = new_pos.y - pane_part->rect.y;

            // determine the size of the dock, based on orientation
            if (dock.IsHorizontal())
                dock_pixels = dock.rect.GetWidth();
                 else
                dock_pixels = dock.rect.GetHeight();

            // determine the total proportion of all resizable panes,
            // and the total size of the dock minus the size of all
            // the fixed panes
            int i, dock_pane_count = dock.panes.GetCount();
            int pane_position = -1;
            for (i = 0; i < dock_pane_count; ++i)
            {
                wxAuiPaneInfo& p = *dock.panes.Item(i);
                if (p.window == pane.window)
                    pane_position = i;

                // while we're at it, subtract the pane sash
                // width from the dock width, because this would
                // skew our proportion calculations
                if (i > 0)
                    dock_pixels -= sash_size;

                // also, the whole size (including decorations) of
                // all fixed panes must also be subtracted, because they
                // are not part of the proportion calculation
                if (p.IsFixed())
                {
                    if (dock.IsHorizontal())
                        dock_pixels -= p.best_size.x;
                         else
                        dock_pixels -= p.best_size.y;
                }
                 else
                {
                    total_proportion += p.dock_proportion;
                }
            }

            // find a pane in our dock to 'steal' space from or to 'give'
            // space to -- this is essentially what is done when a pane is
            // resized; the pane should usually be the first non-fixed pane
            // to the right of the action pane
            int borrow_pane = -1;
            for (i = pane_position+1; i < dock_pane_count; ++i)
            {
                wxAuiPaneInfo& p = *dock.panes.Item(i);
                if (!p.IsFixed())
                {
                    borrow_pane = i;
                    break;
                }
            }


            // demand that the pane being resized is found in this dock
            // (this assert really never should be raised)
            wxASSERT_MSG(pane_position != -1, wxT("Pane not found in dock"));

            // prevent division by zero
            if (dock_pixels == 0 || total_proportion == 0 || borrow_pane == -1)
            {
                m_action = actionNone;
                return;
            }

            // calculate the new proportion of the pane
            int new_proportion = (new_pixsize*total_proportion)/dock_pixels;

            // default minimum size
            int min_size = 0;

            // check against the pane's minimum size, if specified. please note
            // that this is not enough to ensure that the minimum size will
            // not be violated, because the whole frame might later be shrunk,
            // causing the size of the pane to violate it's minimum size
            if (pane.min_size.IsFullySpecified())
            {
                min_size = 0;

                if (pane.HasBorder())
                    min_size += (pane_border_size*2);

                // calculate minimum size with decorations (border,caption)
                if (pane_part->orientation == wxVERTICAL)
                {
                    min_size += pane.min_size.y;
                    if (pane.HasCaption())
                        min_size += caption_size;
                }
                 else
                {
                    min_size += pane.min_size.x;
                }
            }


            // for some reason, an arithmatic error somewhere is causing
            // the proportion calculations to always be off by 1 pixel;
            // for now we will add the 1 pixel on, but we really should
            // determine what's causing this.
            min_size++;

            int min_proportion = (min_size*total_proportion)/dock_pixels;

            if (new_proportion < min_proportion)
                new_proportion = min_proportion;



            int prop_diff = new_proportion - pane.dock_proportion;

            // borrow the space from our neighbor pane to the
            // right or bottom (depending on orientation)
            dock.panes.Item(borrow_pane)->dock_proportion -= prop_diff;
            pane.dock_proportion = new_proportion;

            // repaint
            Update();
            Repaint(NULL);
        }
    }
    else if (m_action == actionClickButton)
    {
        m_hover_button = NULL;
        m_frame->ReleaseMouse();
        UpdateButtonOnScreen(m_action_part, event);

        // make sure we're still over the item that was originally clicked
        if (m_action_part == HitTest(event.GetX(), event.GetY()))
        {
            // fire button-click event
            wxAuiManagerEvent e(wxEVT_AUI_PANE_BUTTON);
            e.SetPane(m_action_part->pane);
            e.SetButton(m_action_part->button->button_id);
            ProcessMgrEvent(e);
        }
    }
    else if (m_action == actionClickCaption)
    {
        m_frame->ReleaseMouse();
    }
    else if (m_action == actionDragFloatingPane)
    {
        m_frame->ReleaseMouse();
    }
    else if (m_action == actionDragToolbarPane)
    {
        m_frame->ReleaseMouse();

        wxAuiPaneInfo& pane = GetPane(m_action_window);
        wxASSERT_MSG(pane.IsOk(), wxT("Pane window not found"));

        // save the new positions
        wxAuiDockInfoPtrArray docks;
        FindDocks(m_docks, pane.dock_direction,
                  pane.dock_layer, pane.dock_row, docks);
        if (docks.GetCount() == 1)
        {
            wxAuiDockInfo& dock = *docks.Item(0);

            wxArrayInt pane_positions, pane_sizes;
            GetPanePositionsAndSizes(dock, pane_positions, pane_sizes);

            int i, dock_pane_count = dock.panes.GetCount();
            for (i = 0; i < dock_pane_count; ++i)
                dock.panes.Item(i)->dock_pos = pane_positions[i];
        }

        pane.state &= ~wxAuiPaneInfo::actionPane;
        Update();
    }
    else
    {
        event.Skip();
    }

    m_action = actionNone;
    m_last_mouse_move = wxPoint(); // see comment in OnMotion()
}


void wxAuiManager::OnMotion(wxMouseEvent& event)
{
    // sometimes when Update() is called from inside this method,
    // a spurious mouse move event is generated; this check will make
    // sure that only real mouse moves will get anywhere in this method;
    // this appears to be a bug somewhere, and I don't know where the
    // mouse move event is being generated.  only verified on MSW

    wxPoint mouse_pos = event.GetPosition();
    if (m_last_mouse_move == mouse_pos)
        return;
    m_last_mouse_move = mouse_pos;


    if (m_action == actionResize)
    {
        wxPoint pos = m_action_part->rect.GetPosition();
        if (m_action_part->orientation == wxHORIZONTAL)
            pos.y = wxMax(0, event.m_y - m_action_offset.y);
             else
            pos.x = wxMax(0, event.m_x - m_action_offset.x);

        wxRect rect(m_frame->ClientToScreen(pos),
                    m_action_part->rect.GetSize());

        wxScreenDC dc;
        if (!m_action_hintrect.IsEmpty())
            DrawResizeHint(dc, m_action_hintrect);
        DrawResizeHint(dc, rect);
        m_action_hintrect = rect;
    }
    else if (m_action == actionClickCaption)
    {
        int drag_x_threshold = wxSystemSettings::GetMetric(wxSYS_DRAG_X);
        int drag_y_threshold = wxSystemSettings::GetMetric(wxSYS_DRAG_Y);

        // caption has been clicked.  we need to check if the mouse
        // is now being dragged. if it is, we need to change the
        // mouse action to 'drag'
        if (abs(event.m_x - m_action_start.x) > drag_x_threshold ||
            abs(event.m_y - m_action_start.y) > drag_y_threshold)
        {
            wxAuiPaneInfo* pane_info = m_action_part->pane;

            if (!pane_info->IsToolbar())
            {
                if ((m_flags & wxAUI_MGR_ALLOW_FLOATING) &&
                    pane_info->IsFloatable())
                {
                    m_action = actionDragFloatingPane;

                    // set initial float position
                    wxPoint pt = m_frame->ClientToScreen(event.GetPosition());
                    pane_info->floating_pos = wxPoint(pt.x - m_action_offset.x,
                                                      pt.y - m_action_offset.y);
                    // float the window
                    pane_info->Float();
                    Update();

                    m_action_window = pane_info->frame;

                    // action offset is used here to make it feel "natural" to the user
                    // to drag a docked pane and suddenly have it become a floating frame.
                    // Sometimes, however, the offset where the user clicked on the docked
                    // caption is bigger than the width of the floating frame itself, so
                    // in that case we need to set the action offset to a sensible value
                    wxSize frame_size = m_action_window->GetSize();
                    if (frame_size.x <= m_action_offset.x)
                        m_action_offset.x = 30;
                }
            }
             else
            {
                m_action = actionDragToolbarPane;
                m_action_window = pane_info->window;
            }
        }
    }
    else if (m_action == actionDragFloatingPane)
    {
        wxPoint pt = m_frame->ClientToScreen(event.GetPosition());
        m_action_window->Move(pt.x - m_action_offset.x,
                             pt.y - m_action_offset.y);
    }
    else if (m_action == actionDragToolbarPane)
    {
        wxAuiPaneInfo& pane = GetPane(m_action_window);
        wxASSERT_MSG(pane.IsOk(), wxT("Pane window not found"));

        pane.state |= wxAuiPaneInfo::actionPane;

        wxPoint pt = event.GetPosition();
        DoDrop(m_docks, m_panes, pane, pt, m_action_offset);

        // if DoDrop() decided to float the pane, set up
        // the floating pane's initial position
        if (pane.IsFloating())
        {
            wxPoint pt = m_frame->ClientToScreen(event.GetPosition());
            pane.floating_pos = wxPoint(pt.x - m_action_offset.x,
                                        pt.y - m_action_offset.y);
        }

        // this will do the actiual move operation;
        // in the case that the pane has been floated,
        // this call will create the floating pane
        // and do the reparenting
        Update();

        // if the pane has been floated, change the mouse
        // action actionDragFloatingPane so that subsequent
        // EVT_MOTION() events will move the floating pane
        if (pane.IsFloating())
        {
            pane.state &= ~wxAuiPaneInfo::actionPane;
            m_action = actionDragFloatingPane;
            m_action_window = pane.frame;
        }
    }
    else
    {
        wxAuiDockUIPart* part = HitTest(event.GetX(), event.GetY());
        if (part && part->type == wxAuiDockUIPart::typePaneButton)
        {
            if (part != m_hover_button)
            {
                // make the old button normal
                if (m_hover_button)
                    UpdateButtonOnScreen(m_hover_button, event);

                // mouse is over a button, so repaint the
                // button in hover mode
                UpdateButtonOnScreen(part, event);
                m_hover_button = part;
            }
        }
         else
        {
            if (m_hover_button)
            {
                m_hover_button = NULL;
                Repaint();
            }
             else
            {
                event.Skip();
            }
        }
    }
}

void wxAuiManager::OnLeaveWindow(wxMouseEvent& WXUNUSED(event))
{
    if (m_hover_button)
    {
        m_hover_button = NULL;
        Repaint();
    }
}

void wxAuiManager::OnChildFocus(wxChildFocusEvent& event)
{
    // when a child pane has it's focus set, we should change the
    // pane's active state to reflect this. (this is only true if
    // active panes are allowed by the owner)
    if (GetFlags() & wxAUI_MGR_ALLOW_ACTIVE_PANE)
    {
        if (GetPane(event.GetWindow()).IsOk())
        {
            SetActivePane(m_panes, event.GetWindow());
            m_frame->Refresh();
        }
    }

    event.Skip();
}


// OnPaneButton() is an event handler that is called
// when a pane button has been pressed.
void wxAuiManager::OnPaneButton(wxAuiManagerEvent& evt)
{
    wxASSERT_MSG(evt.pane, wxT("Pane Info passed to wxAuiManager::OnPaneButton must be non-null"));

    wxAuiPaneInfo& pane = *(evt.pane);

    if (evt.button == wxAuiPaneInfo::buttonClose)
    {
        // fire pane close event
        wxAuiManagerEvent e(wxEVT_AUI_PANE_CLOSE);
        e.SetPane(evt.pane);
        ProcessMgrEvent(e);

        if (!e.GetVeto())
        {
            pane.Hide();
            Update();
        }
    }
     else if (evt.button == wxAuiPaneInfo::buttonPin)
    {
        if ((m_flags & wxAUI_MGR_ALLOW_FLOATING) &&
            pane.IsFloatable())
                pane.Float();
        Update();
    }
}

#endif // wxUSE_AUI
