///////////////////////////////////////////////////////////////////////////////
// Name:        wx/aui/framemanager.h
// Purpose:     wxaui: wx advanced user interface - docking window manager
// Author:      Benjamin I. Williams
// Modified by:
// Created:     2005-05-17
// RCS-ID:      $Id: framemanager.h,v 1.8 2006/07/05 16:37:55 BIW Exp $
// Copyright:   (C) Copyright 2005, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////

#ifndef _WXXTRA_FRAMEMANAGER_H_
#define _WXXTRA_FRAMEMANAGER_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------


// To avoid the -Wunused-local-typedefs warning in GCC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#include <wx/wx.h>
#pragma GCC diagnostic pop


#if wxUSE_AUI
#undef wxXTRA_AUI
#include <wx/aui/framemanager.h>
#else
#define wxXTRA_AUI 1

#include <wx/dynarray.h>
#include <wx/gdicmn.h>
#include <wx/window.h>
#include <wx/timer.h>
#include <wx/sizer.h>

#define WXDLLIMPEXP_AUI WXDLLEXPORT

enum wxAuiManagerDock
{
    wxAUI_DOCK_NONE = 0,
    wxAUI_DOCK_TOP = 1,
    wxAUI_DOCK_RIGHT = 2,
    wxAUI_DOCK_BOTTOM = 3,
    wxAUI_DOCK_LEFT = 4,
    wxAUI_DOCK_CENTER = 5,
    wxAUI_DOCK_CENTRE = wxAUI_DOCK_CENTER
};

enum wxAuiManagerOption
{
    wxAUI_MGR_ALLOW_FLOATING        = 1 << 0,
    wxAUI_MGR_ALLOW_ACTIVE_PANE     = 1 << 1,
    wxAUI_MGR_TRANSPARENT_DRAG      = 1 << 2,
    wxAUI_MGR_TRANSPARENT_HINT      = 1 << 3,
    wxAUI_MGR_TRANSPARENT_HINT_FADE = 1 << 4,
    // The venetian blind effect is ONLY used when the wxAUI_MGR_TRANSPARENT_HINT has been used, but
    // at runtime we determine we cannot use transparency (because, for instance, the OS does not support it).
    // setting this flag drops back in such circumstances (only) to the behaviour without wxAUI_MGR_TRANSPARENT_HINT
    wxAUI_MGR_DISABLE_VENETIAN_BLINDS = 1 << 5,
    wxAUI_MGR_DISABLE_VENETIAN_BLINDS_FADE = 1 << 6,

    wxAUI_MGR_DEFAULT = wxAUI_MGR_ALLOW_FLOATING |
                        wxAUI_MGR_TRANSPARENT_HINT |
                        wxAUI_MGR_TRANSPARENT_HINT_FADE |
                        wxAUI_MGR_DISABLE_VENETIAN_BLINDS_FADE
};

enum wxAuiPaneDockArtSetting
{
    wxAUI_ART_SASH_SIZE = 0,
    wxAUI_ART_CAPTION_SIZE = 1,
    wxAUI_ART_GRIPPER_SIZE = 2,
    wxAUI_ART_PANE_BORDER_SIZE = 3,
    wxAUI_ART_PANE_BUTTON_SIZE = 4,
    wxAUI_ART_BACKGROUND_COLOUR = 5,
    wxAUI_ART_SASH_COLOUR = 6,
    wxAUI_ART_ACTIVE_CAPTION_COLOUR = 7,
    wxAUI_ART_ACTIVE_CAPTION_GRADIENT_COLOUR = 8,
    wxAUI_ART_INACTIVE_CAPTION_COLOUR = 9,
    wxAUI_ART_INACTIVE_CAPTION_GRADIENT_COLOUR = 10,
    wxAUI_ART_ACTIVE_CAPTION_TEXT_COLOUR = 11,
    wxAUI_ART_INACTIVE_CAPTION_TEXT_COLOUR = 12,
    wxAUI_ART_BORDER_COLOUR = 13,
    wxAUI_ART_GRIPPER_COLOUR = 14,
    wxAUI_ART_CAPTION_FONT = 15,
    wxAUI_ART_GRADIENT_TYPE = 16
};

enum wxAuiPaneDockArtGradients
{
    wxAUI_GRADIENT_NONE = 0,
    wxAUI_GRADIENT_VERTICAL = 1,
    wxAUI_GRADIENT_HORIZONTAL = 2
};

enum wxAuiPaneButtonState
{
    wxAUI_BUTTON_STATE_NORMAL = 0,
    wxAUI_BUTTON_STATE_HOVER = 1,
    wxAUI_BUTTON_STATE_PRESSED = 2
};

enum wxAuiPaneInsertLevel
{
    wxAUI_INSERT_PANE = 0,
    wxAUI_INSERT_ROW = 1,
    wxAUI_INSERT_DOCK = 2
};



// forwards and array declarations
class wxAuiDockUIPart;
class wxAuiPaneButton;
class wxAuiPaneInfo;
class wxAuiDockInfo;
class wxAuiDockArt;
class wxAuiManagerEvent;

#ifndef SWIG
WX_DECLARE_USER_EXPORTED_OBJARRAY(wxAuiDockInfo, wxAuiDockInfoArray, WXDLLIMPEXP_AUI);
WX_DECLARE_USER_EXPORTED_OBJARRAY(wxAuiDockUIPart, wxAuiDockUIPartArray, WXDLLIMPEXP_AUI);
WX_DECLARE_USER_EXPORTED_OBJARRAY(wxAuiPaneButton, wxAuiPaneButtonArray, WXDLLIMPEXP_AUI);
WX_DECLARE_USER_EXPORTED_OBJARRAY(wxAuiPaneInfo, wxAuiPaneInfoArray, WXDLLIMPEXP_AUI);
WX_DEFINE_ARRAY_PTR(wxAuiPaneInfo*, wxAuiPaneInfoPtrArray);
WX_DEFINE_ARRAY_PTR(wxAuiDockInfo*, wxAuiDockInfoPtrArray);
#endif // SWIG

extern WXDLLIMPEXP_AUI wxAuiDockInfo wxAuiNullDockInfo;
extern WXDLLIMPEXP_AUI wxAuiPaneInfo wxAuiNullPaneInfo;



class WXDLLIMPEXP_AUI wxAuiPaneInfo
{
public:

    wxAuiPaneInfo()
    {
        window = NULL;
        frame = NULL;
        state = 0;
        dock_direction = wxAUI_DOCK_LEFT;
        dock_layer = 0;
        dock_row = 0;
        dock_pos = 0;
        floating_pos = wxDefaultPosition;
        floating_size = wxDefaultSize;
        best_size = wxDefaultSize;
        min_size = wxDefaultSize;
        max_size = wxDefaultSize;
        dock_proportion = 0;

        DefaultPane();
    }

    ~wxAuiPaneInfo() {}

#ifndef SWIG
    wxAuiPaneInfo(const wxAuiPaneInfo& c)
    {
        name = c.name;
        caption = c.caption;
        window = c.window;
        frame = c.frame;
        state = c.state;
        dock_direction = c.dock_direction;
        dock_layer = c.dock_layer;
        dock_row = c.dock_row;
        dock_pos = c.dock_pos;
        best_size = c.best_size;
        min_size = c.min_size;
        max_size = c.max_size;
        floating_pos = c.floating_pos;
        floating_size = c.floating_size;
        dock_proportion = c.dock_proportion;
        buttons = c.buttons;
        rect = c.rect;
    }

    wxAuiPaneInfo& operator=(const wxAuiPaneInfo& c)
    {
        name = c.name;
        caption = c.caption;
        window = c.window;
        frame = c.frame;
        state = c.state;
        dock_direction = c.dock_direction;
        dock_layer = c.dock_layer;
        dock_row = c.dock_row;
        dock_pos = c.dock_pos;
        best_size = c.best_size;
        min_size = c.min_size;
        max_size = c.max_size;
        floating_pos = c.floating_pos;
        floating_size = c.floating_size;
        dock_proportion = c.dock_proportion;
        buttons = c.buttons;
        rect = c.rect;
        return *this;
    }
#endif // SWIG

    // Write the safe parts of a newly loaded AuiPaneInfo structure "source" into "this"
    // used on loading perspectives etc.
    void SafeSet(wxAuiPaneInfo source)
    {
        // note source is not passed by reference so we can overwrite, to keep the
        // unsafe bits of "dest"
        source.window = window;
        source.frame = frame;
        source.buttons = buttons;
        // now assign
        *this = source;
    }

    bool IsOk() const { return (window != NULL) ? true : false; }
    bool IsFixed() const { return !HasFlag(optionResizable); }
    bool IsResizable() const { return HasFlag(optionResizable); }
    bool IsShown() const { return !HasFlag(optionHidden); }
    bool IsFloating() const { return HasFlag(optionFloating); }
    bool IsDocked() const { return !HasFlag(optionFloating); }
    bool IsToolbar() const { return HasFlag(optionToolbar); }
    bool IsTopDockable() const { return HasFlag(optionTopDockable); }
    bool IsBottomDockable() const { return HasFlag(optionBottomDockable); }
    bool IsLeftDockable() const { return HasFlag(optionLeftDockable); }
    bool IsRightDockable() const { return HasFlag(optionRightDockable); }
    bool IsFloatable() const { return HasFlag(optionFloatable); }
    bool IsMovable() const { return HasFlag(optionMovable); }
    bool HasCaption() const { return HasFlag(optionCaption); }
    bool HasGripper() const { return HasFlag(optionGripper); }
    bool HasBorder() const { return HasFlag(optionPaneBorder); }
    bool HasCloseButton() const { return HasFlag(buttonClose); }
    bool HasMaximizeButton() const { return HasFlag(buttonMaximize); }
    bool HasMinimizeButton() const { return HasFlag(buttonMinimize); }
    bool HasPinButton() const { return HasFlag(buttonPin); }
    bool HasGripperTop() const { return HasFlag(optionGripperTop); }

#ifdef SWIG
    %typemap(out) wxAuiPaneInfo& { $result = $self; Py_INCREF($result); }
#endif
    wxAuiPaneInfo& Window(wxWindow* w) { window = w; return *this; }
    wxAuiPaneInfo& Name(const wxString& n) { name = n; return *this; }
    wxAuiPaneInfo& Caption(const wxString& c) { caption = c; return *this; }
    wxAuiPaneInfo& Left() { dock_direction = wxAUI_DOCK_LEFT; return *this; }
    wxAuiPaneInfo& Right() { dock_direction = wxAUI_DOCK_RIGHT; return *this; }
    wxAuiPaneInfo& Top() { dock_direction = wxAUI_DOCK_TOP; return *this; }
    wxAuiPaneInfo& Bottom() { dock_direction = wxAUI_DOCK_BOTTOM; return *this; }
    wxAuiPaneInfo& Center() { dock_direction = wxAUI_DOCK_CENTER; return *this; }
    wxAuiPaneInfo& Centre() { dock_direction = wxAUI_DOCK_CENTRE; return *this; }
    wxAuiPaneInfo& Direction(int direction) { dock_direction = direction; return *this; }
    wxAuiPaneInfo& Layer(int layer) { dock_layer = layer; return *this; }
    wxAuiPaneInfo& Row(int row) { dock_row = row; return *this; }
    wxAuiPaneInfo& Position(int pos) { dock_pos = pos; return *this; }
    wxAuiPaneInfo& BestSize(const wxSize& size) { best_size = size; return *this; }
    wxAuiPaneInfo& MinSize(const wxSize& size) { min_size = size; return *this; }
    wxAuiPaneInfo& MaxSize(const wxSize& size) { max_size = size; return *this; }
    wxAuiPaneInfo& BestSize(int x, int y) { best_size.Set(x,y); return *this; }
    wxAuiPaneInfo& MinSize(int x, int y) { min_size.Set(x,y); return *this; }
    wxAuiPaneInfo& MaxSize(int x, int y) { max_size.Set(x,y); return *this; }
    wxAuiPaneInfo& FloatingPosition(const wxPoint& pos) { floating_pos = pos; return *this; }
    wxAuiPaneInfo& FloatingPosition(int x, int y) { floating_pos.x = x; floating_pos.y = y; return *this; }
    wxAuiPaneInfo& FloatingSize(const wxSize& size) { floating_size = size; return *this; }
    wxAuiPaneInfo& FloatingSize(int x, int y) { floating_size.Set(x,y); return *this; }
    wxAuiPaneInfo& Fixed() { return SetFlag(optionResizable, false); }
    wxAuiPaneInfo& Resizable(bool resizable = true) { return SetFlag(optionResizable, resizable); }
    wxAuiPaneInfo& Dock() { return SetFlag(optionFloating, false); }
    wxAuiPaneInfo& Float() { return SetFlag(optionFloating, true); }
    wxAuiPaneInfo& Hide() { return SetFlag(optionHidden, true); }
    wxAuiPaneInfo& Show(bool show = true) { return SetFlag(optionHidden, !show); }
    wxAuiPaneInfo& CaptionVisible(bool visible = true) { return SetFlag(optionCaption, visible); }
    wxAuiPaneInfo& PaneBorder(bool visible = true) { return SetFlag(optionPaneBorder, visible); }
    wxAuiPaneInfo& Gripper(bool visible = true) { return SetFlag(optionGripper, visible); }
    wxAuiPaneInfo& GripperTop(bool attop = true) { return SetFlag(optionGripperTop, attop); }
    wxAuiPaneInfo& CloseButton(bool visible = true) { return SetFlag(buttonClose, visible); }
    wxAuiPaneInfo& MaximizeButton(bool visible = true) { return SetFlag(buttonMaximize, visible); }
    wxAuiPaneInfo& MinimizeButton(bool visible = true) { return SetFlag(buttonMinimize, visible); }
    wxAuiPaneInfo& PinButton(bool visible = true) { return SetFlag(buttonPin, visible); }
    wxAuiPaneInfo& DestroyOnClose(bool b = true) { return SetFlag(optionDestroyOnClose, b); }
    wxAuiPaneInfo& TopDockable(bool b = true) { return SetFlag(optionTopDockable, b); }
    wxAuiPaneInfo& BottomDockable(bool b = true) { return SetFlag(optionBottomDockable, b); }
    wxAuiPaneInfo& LeftDockable(bool b = true) { return SetFlag(optionLeftDockable, b); }
    wxAuiPaneInfo& RightDockable(bool b = true) { return SetFlag(optionRightDockable, b); }
    wxAuiPaneInfo& Floatable(bool b = true) { return SetFlag(optionFloatable, b); }
    wxAuiPaneInfo& Movable(bool b = true) { return SetFlag(optionMovable, b); }
    wxAuiPaneInfo& Dockable(bool b = true)
    {
        return TopDockable(b).BottomDockable(b).LeftDockable(b).RightDockable(b);
    }

    wxAuiPaneInfo& DefaultPane()
    {
        state |= optionTopDockable | optionBottomDockable |
                 optionLeftDockable | optionRightDockable |
                 optionFloatable | optionMovable | optionResizable |
                 optionCaption | optionPaneBorder | buttonClose;
        return *this;
    }

    wxAuiPaneInfo& CentrePane() { return CenterPane(); }
    wxAuiPaneInfo& CenterPane()
    {
        state = 0;
        return Center().PaneBorder().Resizable();
    }

    wxAuiPaneInfo& ToolbarPane()
    {
        DefaultPane();
        state |= (optionToolbar | optionGripper);
        state &= ~(optionResizable | optionCaption);
        if (dock_layer == 0)
            dock_layer = 10;
        return *this;
    }

    wxAuiPaneInfo& SetFlag(unsigned int flag, bool option_state)
    {
        if (option_state)
            state |= flag;
             else
            state &= ~flag;
        return *this;
    }

    bool HasFlag(unsigned int flag) const
    {
        return (state & flag) ? true:false;
    }

#ifdef SWIG
    %typemap(out) wxAuiPaneInfo& ;
#endif

public:

    enum wxAuiPaneState
    {
        optionFloating        = 1 << 0,
        optionHidden          = 1 << 1,
        optionLeftDockable    = 1 << 2,
        optionRightDockable   = 1 << 3,
        optionTopDockable     = 1 << 4,
        optionBottomDockable  = 1 << 5,
        optionFloatable       = 1 << 6,
        optionMovable         = 1 << 7,
        optionResizable       = 1 << 8,
        optionPaneBorder      = 1 << 9,
        optionCaption         = 1 << 10,
        optionGripper         = 1 << 11,
        optionDestroyOnClose  = 1 << 12,
        optionToolbar         = 1 << 13,
        optionActive          = 1 << 14,
        optionGripperTop      = 1 << 15,

        buttonClose           = 1 << 24,
        buttonMaximize        = 1 << 25,
        buttonMinimize        = 1 << 26,
        buttonPin             = 1 << 27,
        buttonCustom1         = 1 << 28,
        buttonCustom2         = 1 << 29,
        buttonCustom3         = 1 << 30,
        actionPane            = 1 << 31  // used internally
    };

public:
    wxString name;        // name of the pane
    wxString caption;     // caption displayed on the window

    wxWindow* window;     // window that is in this pane
    wxFrame* frame;       // floating frame window that holds the pane
    unsigned int state;   // a combination of wxAuiPaneState values

    int dock_direction;   // dock direction (top, bottom, left, right, center)
    int dock_layer;       // layer number (0 = innermost layer)
    int dock_row;         // row number on the docking bar (0 = first row)
    int dock_pos;         // position inside the row (0 = first position)

    wxSize best_size;     // size that the layout engine will prefer
    wxSize min_size;      // minimum size the pane window can tolerate
    wxSize max_size;      // maximum size the pane window can tolerate

    wxPoint floating_pos; // position while floating
    wxSize floating_size; // size while floating
    int dock_proportion;  // proportion while docked

    wxAuiPaneButtonArray buttons; // buttons on the pane

    wxRect rect;              // current rectangle (populated by wxAUI)
};





class WXDLLIMPEXP_AUI wxAuiManager : public wxEvtHandler
{
friend class wxAuiFloatingFrame;

public:

    wxAuiManager(wxWindow* managed_wnd = NULL,
                   unsigned int flags = wxAUI_MGR_DEFAULT);
    virtual ~wxAuiManager();
    void UnInit();

    void SetFlags(unsigned int flags);
    unsigned int GetFlags() const;

    void SetManagedWindow(wxWindow* managed_wnd);
    wxWindow* GetManagedWindow() const;

#ifdef SWIG
    %disownarg( wxAuiDockArt* art_provider );
#endif
    void SetArtProvider(wxAuiDockArt* art_provider);
    wxAuiDockArt* GetArtProvider() const;

    wxAuiPaneInfo& GetPane(wxWindow* window);
    wxAuiPaneInfo& GetPane(const wxString& name);
    wxAuiPaneInfoArray& GetAllPanes();

    bool AddPane(wxWindow* window,
                 const wxAuiPaneInfo& pane_info);

    bool AddPane(wxWindow* window,
                 const wxAuiPaneInfo& pane_info,
                 const wxPoint& drop_pos);

    bool AddPane(wxWindow* window,
                 int direction = wxLEFT,
                 const wxString& caption = wxEmptyString);

    bool InsertPane(wxWindow* window,
                 const wxAuiPaneInfo& insert_location,
                 int insert_level = wxAUI_INSERT_PANE);

    bool DetachPane(wxWindow* window);

    wxString SavePaneInfo(wxAuiPaneInfo& pane);
    void LoadPaneInfo(wxString pane_part, wxAuiPaneInfo &pane);

    wxString SavePerspective();

    bool LoadPerspective(const wxString& perspective,
                 bool update = true);

    void Update();


public:

    void DrawHintRect(wxWindow* pane_window,
                       const wxPoint& pt,
                       const wxPoint& offset);
    virtual void ShowHint(const wxRect& rect);
    virtual void HideHint();

public:

    // deprecated -- please use SetManagedWindow() and
    // and GetManagedWindow() instead

    wxDEPRECATED( void SetFrame(wxFrame* frame) );
    wxDEPRECATED( wxFrame* GetFrame() const );

protected:



    void DoFrameLayout();

    void LayoutAddPane(wxSizer* container,
                       wxAuiDockInfo& dock,
                       wxAuiPaneInfo& pane,
                       wxAuiDockUIPartArray& uiparts,
                       bool spacer_only);

    void LayoutAddDock(wxSizer* container,
                       wxAuiDockInfo& dock,
                       wxAuiDockUIPartArray& uiparts,
                       bool spacer_only);

    wxSizer* LayoutAll(wxAuiPaneInfoArray& panes,
                       wxAuiDockInfoArray& docks,
                       wxAuiDockUIPartArray& uiparts,
                       bool spacer_only = false);

    virtual bool ProcessDockResult(wxAuiPaneInfo& target,
                                   const wxAuiPaneInfo& new_pos);

    bool DoDrop(wxAuiDockInfoArray& docks,
                wxAuiPaneInfoArray& panes,
                wxAuiPaneInfo& drop,
                const wxPoint& pt,
                const wxPoint& action_offset = wxPoint(0,0));

    wxAuiPaneInfo& LookupPane(wxWindow* window);
    wxAuiPaneInfo& LookupPane(const wxString& name);
    wxAuiDockUIPart* HitTest(int x, int y);
    wxAuiDockUIPart* GetPanePart(wxWindow* pane);
    int GetDockPixelOffset(wxAuiPaneInfo& test);
    void OnFloatingPaneMoveStart(wxWindow* window);
    void OnFloatingPaneMoving(wxWindow* window);
    void OnFloatingPaneMoved(wxWindow* window);
    void OnFloatingPaneActivated(wxWindow* window);
    void OnFloatingPaneClosed(wxWindow* window, wxCloseEvent& evt);
    void OnFloatingPaneResized(wxWindow* window, const wxSize& size);
    void Render(wxDC* dc);
    void Repaint(wxDC* dc = NULL);
    void ProcessMgrEvent(wxAuiManagerEvent& event);
    void UpdateButtonOnScreen(wxAuiDockUIPart* button_ui_part,
                              const wxMouseEvent& event);
    void GetPanePositionsAndSizes(wxAuiDockInfo& dock,
                              wxArrayInt& positions,
                              wxArrayInt& sizes);


public:

    // public events (which can be invoked externally)
    void OnRender(wxAuiManagerEvent& evt);
    void OnPaneButton(wxAuiManagerEvent& evt);

protected:

    // protected events
    void OnPaint(wxPaintEvent& evt);
    void OnEraseBackground(wxEraseEvent& evt);
    void OnSize(wxSizeEvent& evt);
    void OnSetCursor(wxSetCursorEvent& evt);
    void OnLeftDown(wxMouseEvent& evt);
    void OnLeftUp(wxMouseEvent& evt);
    void OnMotion(wxMouseEvent& evt);
    void OnLeaveWindow(wxMouseEvent& evt);
    void OnChildFocus(wxChildFocusEvent& evt);
    void OnHintFadeTimer(wxTimerEvent& evt);

protected:

    enum
    {
        actionNone = 0,
        actionResize,
        actionClickButton,
        actionClickCaption,
        actionDragToolbarPane,
        actionDragFloatingPane
    };

protected:

    wxWindow* m_frame;           // the window being managed
    wxAuiDockArt* m_art;            // dock art object which does all drawing
    unsigned int m_flags;        // manager flags wxAUI_MGR_*

    wxAuiPaneInfoArray m_panes;     // array of panes structures
    wxAuiDockInfoArray m_docks;     // array of docks structures
    wxAuiDockUIPartArray m_uiparts; // array of UI parts (captions, buttons, etc)

    int m_action;                // current mouse action
    wxPoint m_action_start;      // position where the action click started
    wxPoint m_action_offset;     // offset from upper left of the item clicked
    wxAuiDockUIPart* m_action_part; // ptr to the part the action happened to
    wxWindow* m_action_window;   // action frame or window (NULL if none)
    wxRect m_action_hintrect;    // hint rectangle for the action
    wxAuiDockUIPart* m_hover_button;// button uipart being hovered over
    wxRect m_last_hint;          // last hint rectangle
    wxPoint m_last_mouse_move;   // last mouse move position (see OnMotion)

    wxFrame* m_hint_wnd;         // transparent hint window, if supported by platform
    wxTimer m_hint_fadetimer;    // transparent fade timer
    wxByte m_hint_fadeamt;       // transparent fade amount
    wxByte m_hint_fademax;       // maximum value of hint fade

#ifndef SWIG
    DECLARE_EVENT_TABLE()
#endif // SWIG
};



// event declarations/classes

class WXDLLIMPEXP_AUI wxAuiManagerEvent : public wxEvent
{
public:
    wxAuiManagerEvent(wxEventType type=wxEVT_NULL) : wxEvent(0, type)
    {
        pane = NULL;
        button = 0;
        veto_flag = false;
        canveto_flag = true;
        dc = NULL;
    }
#ifndef SWIG
    wxAuiManagerEvent(const wxAuiManagerEvent& c) : wxEvent(c)
    {
        pane = c.pane;
        button = c.button;
        veto_flag = c.veto_flag;
        canveto_flag = c.canveto_flag;
        dc = c.dc;
    }
#endif
    wxEvent *Clone() const { return new wxAuiManagerEvent(*this); }

    void SetPane(wxAuiPaneInfo* p) { pane = p; }
    void SetButton(int b) { button = b; }
    void SetDC(wxDC* pdc) { dc = pdc; }

    wxAuiPaneInfo* GetPane() { return pane; }
    int GetButton() { return button; }
    wxDC* GetDC() { return dc; }

    void Veto(bool veto = true) { veto_flag = veto; }
    bool GetVeto() const { return veto_flag; }
    void SetCanVeto(bool can_veto) { canveto_flag = can_veto; }
    bool CanVeto() const { return  canveto_flag && veto_flag; }

public:
    wxAuiPaneInfo* pane;
    int button;
    bool veto_flag;
    bool canveto_flag;
    wxDC* dc;

#ifndef SWIG
private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxAuiManagerEvent)
#endif
};


class WXDLLIMPEXP_AUI wxAuiDockInfo
{
public:
    wxAuiDockInfo()
    {
        dock_direction = 0;
        dock_layer = 0;
        dock_row = 0;
        size = 0;
        min_size = 0;
        resizable = true;
        fixed = false;
        toolbar = false;
    }

#ifndef SWIG
    wxAuiDockInfo(const wxAuiDockInfo& c)
    {
        dock_direction = c.dock_direction;
        dock_layer = c.dock_layer;
        dock_row = c.dock_row;
        size = c.size;
        min_size = c.min_size;
        resizable = c.resizable;
        fixed = c.fixed;
        toolbar = c.toolbar;
        panes = c.panes;
        rect = c.rect;
    }

    wxAuiDockInfo& operator=(const wxAuiDockInfo& c)
    {
        dock_direction = c.dock_direction;
        dock_layer = c.dock_layer;
        dock_row = c.dock_row;
        size = c.size;
        min_size = c.min_size;
        resizable = c.resizable;
        fixed = c.fixed;
        toolbar = c.toolbar;
        panes = c.panes;
        rect = c.rect;
        return *this;
    }
#endif // SWIG

    bool IsOk() const { return (dock_direction != 0) ? true : false; }
    bool IsHorizontal() const { return (dock_direction == wxAUI_DOCK_TOP ||
                             dock_direction == wxAUI_DOCK_BOTTOM) ? true:false; }
    bool IsVertical() const { return (dock_direction == wxAUI_DOCK_LEFT ||
                             dock_direction == wxAUI_DOCK_RIGHT ||
                             dock_direction == wxAUI_DOCK_CENTER) ? true:false; }
public:
    wxAuiPaneInfoPtrArray panes; // array of panes
    wxRect rect;              // current rectangle
    int dock_direction;       // dock direction (top, bottom, left, right, center)
    int dock_layer;           // layer number (0 = innermost layer)
    int dock_row;             // row number on the docking bar (0 = first row)
    int size;                 // size of the dock
    int min_size;             // minimum size of a dock (0 if there is no min)
    bool resizable;           // flag indicating whether the dock is resizable
    bool toolbar;             // flag indicating dock contains only toolbars
    bool fixed;               // flag indicating that the dock operates on
                              // absolute coordinates as opposed to proportional
};


class WXDLLIMPEXP_AUI wxAuiDockUIPart
{
public:
    enum
    {
        typeCaption,
        typeGripper,
        typeDock,
        typeDockSizer,
        typePane,
        typePaneSizer,
        typeBackground,
        typePaneBorder,
        typePaneButton
    };

    int type;                // ui part type (see enum above)
    int orientation;         // orientation (either wxHORIZONTAL or wxVERTICAL)
    wxAuiDockInfo* dock;        // which dock the item is associated with
    wxAuiPaneInfo* pane;        // which pane the item is associated with
    wxAuiPaneButton* button;    // which pane button the item is associated with
    wxSizer* cont_sizer;     // the part's containing sizer
    wxSizerItem* sizer_item; // the sizer item of the part
    wxRect rect;             // client coord rectangle of the part itself
};


class WXDLLIMPEXP_AUI wxAuiPaneButton
{
public:
    int button_id;        // id of the button (e.g. buttonClose)
};



#ifndef SWIG
// wx event machinery

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_AUI_PANE_BUTTON, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_AUI_PANE_CLOSE, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_AUI_RENDER, 0)
END_DECLARE_EVENT_TYPES()

typedef void (wxEvtHandler::*wxAuiManagerEventFunction)(wxAuiManagerEvent&);

#define wxAuiManagerEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxAuiManagerEventFunction, &func)

#define EVT_AUI_PANE_BUTTON(func) \
   wx__DECLARE_EVT0(wxEVT_AUI_PANE_BUTTON, wxAuiManagerEventHandler(func))
#define EVT_AUI_PANE_CLOSE(func) \
   wx__DECLARE_EVT0(wxEVT_AUI_PANE_CLOSE, wxAuiManagerEventHandler(func))
#define EVT_AUI_RENDER(func) \
   wx__DECLARE_EVT0(wxEVT_AUI_RENDER, wxAuiManagerEventHandler(func))

#else

%constant wxEventType wxEVT_AUI_PANE_BUTTON;
%constant wxEventType wxEVT_AUI_PANE_CLOSE;
%constant wxEventType wxEVT_AUI_RENDER;

%pythoncode {
    EVT_AUI_PANE_BUTTON = wx.PyEventBinder( wxEVT_AUI_PANE_BUTTON )
    EVT_AUI_PANE_CLOSE = wx.PyEventBinder( wxEVT_AUI_PANE_CLOSE )
    EVT_AUI_RENDER = wx.PyEventBinder( wxEVT_AUI_RENDER )
}
#endif // SWIG


#endif // wxUSE_AUI
#endif //_WX_FRAMEMANAGER_H_
