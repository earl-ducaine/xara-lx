// $Id: odcombo.cpp 1496 2006-07-22 13:09:45Z alex $
/* @@tag:xara-cn-tp@@ THIRD PARTY COPYRIGHT */
// The following line makes normalize.pl skip type fixing
/* SKIPFIXTYPES: START */

/////////////////////////////////////////////////////////////////////////////
// Name:        odcombo.cpp
// Purpose:     wxOwnerDrawnComboBox, wxVListBoxComboPopup
// Author:      Jaakko Salli
// Modified by:
// Created:     Apr-30-2006
// RCS-ID:
// Copyright:   (c) 2005 Jaakko Salli
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <wx/wx.h>

#include "odcombo.h"
// Only compile stuff if this is not already in WX
#if wxXTRA_ODCOMBOBOX

#include <wx/renderer.h>

// ============================================================================
// implementation
// ============================================================================

// time in milliseconds before partial completion buffer drops
#define wxODCB_PARTIAL_COMPLETION_TIME 1000

// ----------------------------------------------------------------------------
// wxVListBoxComboPopup is a wxVListBox customized to act as a popup control
//
// ----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(wxVListBoxComboPopup, wxVListBox)
    EVT_MOTION(wxVListBoxComboPopup::OnMouseMove)
    EVT_KEY_DOWN(wxVListBoxComboPopup::OnKey)
    EVT_LEFT_UP(wxVListBoxComboPopup::OnLeftClick)
END_EVENT_TABLE()


void wxVListBoxComboPopup::Init()
{
    m_widestWidth = 0;
    m_widestItem = -1;
    m_widthsDirty = false;
    m_findWidest = false;
    m_itemHeight = 0;
    m_value = -1;
    m_itemHover = -1;
    m_clientDataItemsType = wxClientData_None;
    m_partialCompletionString = wxEmptyString;
}

bool wxVListBoxComboPopup::Create(wxWindow* parent)
{
    if ( !wxVListBox::Create(parent,
                             wxID_ANY,
                             wxDefaultPosition,
                             wxDefaultSize,
                             wxBORDER_SIMPLE | wxLB_INT_HEIGHT | wxWANTS_CHARS) )
        return false;

    m_useFont = m_combo->GetFont();

    wxVListBox::SetItemCount(m_strings.GetCount());

    // TODO: Move this to SetFont
    m_itemHeight = GetCharHeight() + 0;

    return true;
}

wxVListBoxComboPopup::~wxVListBoxComboPopup()
{
    Clear();
}

bool wxVListBoxComboPopup::LazyCreate()
{
    // NB: There is a bug with wxVListBox that can be avoided by creating
    //     it later (bug causes empty space to be shown if initial selection
    //     is at the end of a list longer than the control can show at once).
    return true;
}

// paint the control itself
void wxVListBoxComboPopup::PaintComboControl( wxDC& dc, const wxRect& rect )
{
    if ( !(m_combo->GetWindowStyle() & wxODCB_STD_CONTROL_PAINT) )
    {
        OnDrawBg(dc,rect,m_value,wxODCB_PAINTING_CONTROL);
        if ( m_value >= 0 )
        {
            OnDrawItem(dc,rect,m_value,wxODCB_PAINTING_CONTROL);
            return;
        }
    }

    wxComboPopup::PaintComboControl(dc,rect);
}

void wxVListBoxComboPopup::OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const
{
    // TODO: Maybe this code could be moved to wxVListBox::OnPaint?
    dc.SetFont(m_useFont);

    // Set correct text colour for selected items
    if ( wxVListBox::GetSelection() == (int) n )
        dc.SetTextForeground( wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT) );
    else
        dc.SetTextForeground( wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT) );

    OnDrawItem(dc,rect,(int)n,0);
}

wxCoord wxVListBoxComboPopup::OnMeasureItem(size_t n) const
{
    wxOwnerDrawnComboBox* combo = (wxOwnerDrawnComboBox*) m_combo;

    wxASSERT_MSG( combo->IsKindOf(CLASSINFO(wxOwnerDrawnComboBox)),
                  wxT("you must subclass wxVListBoxComboPopup for drawing and measuring methods") );

    wxCoord h = combo->OnMeasureItem(n);
    if ( h < 0 )
        h = m_itemHeight;
    return h;
}

wxCoord wxVListBoxComboPopup::OnMeasureItemWidth(size_t n) const
{
    wxOwnerDrawnComboBox* combo = (wxOwnerDrawnComboBox*) m_combo;

    wxASSERT_MSG( combo->IsKindOf(CLASSINFO(wxOwnerDrawnComboBox)),
                  wxT("you must subclass wxVListBoxComboPopup for drawing and measuring methods") );

    return combo->OnMeasureItemWidth(n);
}

void wxVListBoxComboPopup::OnDrawBg( wxDC& dc,
                                     const wxRect& rect,
                                     int item,
                                     int flags ) const
{
    wxOwnerDrawnComboBox* combo = (wxOwnerDrawnComboBox*) m_combo;

    wxASSERT_MSG( combo->IsKindOf(CLASSINFO(wxOwnerDrawnComboBox)),
                  wxT("you must subclass wxVListBoxComboPopup for drawing and measuring methods") );

    combo->OnDrawBackground(dc,rect,item,flags);
}

void wxVListBoxComboPopup::OnDrawBackground(wxDC& dc, const wxRect& rect, size_t n) const
{
    OnDrawBg(dc,rect,(int)n,0);
}

// This is called from wxVListBoxComboPopup::OnDrawItem, with text colour and font prepared
void wxVListBoxComboPopup::OnDrawItem( wxDC& dc, const wxRect& rect, int item, int flags ) const
{
    wxOwnerDrawnComboBox* combo = (wxOwnerDrawnComboBox*) m_combo;

    wxASSERT_MSG( combo->IsKindOf(CLASSINFO(wxOwnerDrawnComboBox)),
                  wxT("you must subclass wxVListBoxComboPopup for drawing and measuring methods") );

    combo->OnDrawItem(dc,rect,item,flags);
}

void wxVListBoxComboPopup::DismissWithEvent()
{
    StopPartialCompletion();

    int selection = wxVListBox::GetSelection();

    Dismiss();

    wxString valStr;
    if ( selection != wxNOT_FOUND )
        valStr = m_strings[selection];
    else
        valStr = wxEmptyString;

    m_value = selection;

    if ( valStr != m_combo->GetValue() )
        m_combo->SetValue(valStr);

    SendComboBoxEvent(selection);
}

void wxVListBoxComboPopup::SendComboBoxEvent( int selection )
{
    wxCommandEvent evt(wxEVT_COMMAND_COMBOBOX_SELECTED,m_combo->GetId());

    evt.SetEventObject(m_combo);

    evt.SetInt(selection);

    // Set client data, if any
    if ( selection >= 0 && (int)m_clientDatas.GetCount() > selection )
    {
        void* clientData = m_clientDatas[selection];
        if ( m_clientDataItemsType == wxClientData_Object )
            evt.SetClientObject((wxClientData*)clientData);
        else
            evt.SetClientData(clientData);
    }

    m_combo->GetEventHandler()->AddPendingEvent(evt);
}

// returns true if key was consumed
bool wxVListBoxComboPopup::HandleKey( int keycode, bool saturate, wxChar unicode )
{
    int value = m_value;
    int itemCount = GetCount();
    int comboStyle = m_combo->GetWindowStyle();

    // this is the character equivalent of the code
    wxChar keychar=0;
    if ((keycode >= WXK_SPACE) && (keycode <=255) && (keycode != WXK_DELETE) && wxIsprint(keycode))
    {
        keychar = (wxChar)keycode;
    }
    else if (unicode>0)
    {
        keychar = unicode; 
    }

    if ( keycode == WXK_DOWN || keycode == WXK_RIGHT )
    {
        value++;
        StopPartialCompletion();
    }
    else if ( keycode == WXK_UP || keycode == WXK_LEFT )
    {
        value--;
        StopPartialCompletion();
    }
    else if ( keycode == WXK_PAGEDOWN )
    {
        value+=10;
        StopPartialCompletion();
    }
    else if ( keycode == WXK_PAGEUP )
    {
        value-=10;
        StopPartialCompletion();
    }
    else if ( comboStyle & wxCB_READONLY )
    {
        // Try partial completion

        // find the new partial completion string
#if wxUSE_TIMER
        if (m_partialCompletionTimer.IsRunning())
            m_partialCompletionString+=wxString(keychar);
        else
#endif // wxUSE_TIMER
            m_partialCompletionString=wxString(keychar);

        // now search through the values to see if this is found
        int found = -1;
        unsigned int length=m_partialCompletionString.Length();
        int i;
        for (i=0; i<itemCount; i++)
        {
            wxString item=GetString(i);
            if (( item.Length() >=length) && (!  m_partialCompletionString.CmpNoCase(item.Left(length))))
            {
                found=i;
                break;
            }
        }

        if (found<0)
        {
            StopPartialCompletion();
            ::wxBell();
            return true; // to stop the first value being set
        }
        else
        {
            value=i;
#if wxUSE_TIMER
            m_partialCompletionTimer.Start(wxODCB_PARTIAL_COMPLETION_TIME, true);
#endif // wxUSE_TIMER
        }
    }
    else
        return false;

    if ( saturate )
    {
        if ( value >= itemCount )
            value = itemCount - 1;
        else if ( value < 0 )
            value = 0;
    }
    else
    {
        if ( value >= itemCount )
            value -= itemCount;
        else if ( value < 0 )
            value += itemCount;
    }

    if ( value == m_value )
        // Even if value was same, don't skip the event
        // (good for consistency)
        return true;

    m_value = value;

    if ( value >= 0 )
        m_combo->SetValue(m_strings[value]);

    SendComboBoxEvent(m_value);

    return true;
}

// stop partial completion
void wxVListBoxComboPopup::StopPartialCompletion()
{
    m_partialCompletionString = wxEmptyString;
#if wxUSE_TIMER
    m_partialCompletionTimer.Stop();
#endif // wxUSE_TIMER
}

void wxVListBoxComboPopup::OnComboDoubleClick()
{
    // Cycle on dclick (disable saturation to allow true cycling).
    if ( !::wxGetKeyState(WXK_SHIFT) )
        HandleKey(WXK_DOWN,false);
    else
        HandleKey(WXK_UP,false);
}

void wxVListBoxComboPopup::OnComboKeyEvent( wxKeyEvent& event )
{
    // Saturated key movement on
    if ( !HandleKey(event.GetKeyCode(),true,
#if wxUSE_UNICODE
        event.GetUnicodeKey()
#else
        0
#endif
        ) )
        event.Skip();
}

void wxVListBoxComboPopup::OnPopup()
{
    // *must* set value after size is set (this is because of a vlbox bug)
    wxVListBox::SetSelection(m_value);
}

void wxVListBoxComboPopup::OnMouseMove(wxMouseEvent& event)
{
    event.Skip();

    // Move selection to cursor if it is inside the popup

    int y = event.GetPosition().y;
    int fromBottom = GetClientSize().y - y;

    // Since in any case we need to find out if the last item is only
    // partially visible, we might just as well replicate the HitTest
    // loop here.
    const size_t lineMax = GetVisibleEnd();
    for ( size_t line = GetVisibleBegin(); line < lineMax; line++ )
    {
        y -= OnGetLineHeight(line);
        if ( y < 0 )
        {
            // Only change selection if item is fully visible
            if ( (y + fromBottom) >= 0 )
            {
                wxVListBox::SetSelection((int)line);
                return;
            }
        }
    }
}

void wxVListBoxComboPopup::OnLeftClick(wxMouseEvent& WXUNUSED(event))
{
    DismissWithEvent();
}

void wxVListBoxComboPopup::OnKey(wxKeyEvent& event)
{
    // Select item if ENTER is pressed
    if ( event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER )
    {
        DismissWithEvent();
    }
    // Hide popup if ESC is pressed
    else if ( event.GetKeyCode() == WXK_ESCAPE )
    {
        StopPartialCompletion();
        Dismiss();
    }
    else
    {
        int comboStyle = m_combo->GetWindowStyle();
        int keycode = event.GetKeyCode();
        // Process partial completion key codes here, but not the arrow keys as the base class will do that for us
        if ((comboStyle & wxCB_READONLY) &&
            (keycode >= WXK_SPACE) && (keycode <=255) && (keycode != WXK_DELETE) && wxIsprint(keycode))
        {
            OnComboKeyEvent(event);
            SetSelection(m_value); // ensure the highlight bar moves
        }
        else
            event.Skip();
    }
}

void wxVListBoxComboPopup::Insert( const wxString& item, int pos )
{
    // Need to change selection?
    wxString strValue;
    if ( !(m_combo->GetWindowStyle() & wxCB_READONLY) &&
         m_combo->GetValue() == item )
    {
        m_value = pos;
    }

    m_strings.Insert(item,pos);
    m_widths.Insert(-1,pos);
    m_widthsDirty = true;

    if ( IsCreated() )
        wxVListBox::SetItemCount( wxVListBox::GetItemCount()+1 );
}

int wxVListBoxComboPopup::Append(const wxString& item)
{
    int pos = (int)m_strings.GetCount();

    if ( m_combo->GetWindowStyle() & wxCB_SORT )
    {
        // Find position
        // TODO: Could be optimized with binary search
        wxArrayString strings = m_strings;
        unsigned int i;

        for ( i=0; i<strings.GetCount(); i++ )
        {
            if ( item.Cmp(strings.Item(i)) < 0 )
            {
                pos = (int)i;
                break;
            }
        }
    }

    Insert(item,pos);

    return pos;
}

void wxVListBoxComboPopup::Clear()
{
    wxASSERT(m_combo);

    m_strings.Empty();
    m_widths.Empty();

    m_widestWidth = 0;
    m_widestItem = -1;

    ClearClientDatas();

    m_value = wxNOT_FOUND;

    if ( IsCreated() )
        wxVListBox::SetItemCount(0);
}

void wxVListBoxComboPopup::ClearClientDatas()
{
    if ( m_clientDataItemsType == wxClientData_Object )
    {
        size_t i;
        for ( i=0; i<m_clientDatas.GetCount(); i++ )
            delete (wxClientData*) m_clientDatas[i];
    }

    m_clientDatas.Empty();
}

void wxVListBoxComboPopup::SetItemClientData( unsigned int n,
                                              void* clientData,
                                              wxClientDataType clientDataItemsType )
{
    // It should be sufficient to update this variable only here
    m_clientDataItemsType = clientDataItemsType;

    m_clientDatas.SetCount(n+1,NULL);
    m_clientDatas[n] = clientData;

    ItemWidthChanged(n);
}

void* wxVListBoxComboPopup::GetItemClientData(unsigned int n) const
{
    if ( m_clientDatas.GetCount() > n )
        return m_clientDatas[n];

    return NULL;
}

void wxVListBoxComboPopup::Delete( unsigned int item )
{
    // Remove client data, if set
    if ( m_clientDatas.GetCount() )
    {
        if ( m_clientDataItemsType == wxClientData_Object )
            delete (wxClientData*) m_clientDatas[item];

        m_clientDatas.RemoveAt(item);
    }

    m_strings.RemoveAt(item);
    m_widths.RemoveAt(item);

    if ( (int)item == m_widestItem )
        m_findWidest = true;

    if ( IsCreated() )
        wxVListBox::SetItemCount( wxVListBox::GetItemCount()-1 );
}

int wxVListBoxComboPopup::FindString(const wxString& s, bool bCase) const
{
    return m_strings.Index(s, bCase);
}

unsigned int wxVListBoxComboPopup::GetCount() const
{
    return m_strings.GetCount();
}

wxString wxVListBoxComboPopup::GetString( int item ) const
{
    return m_strings[item];
}

void wxVListBoxComboPopup::SetString( int item, const wxString& str )
{
    m_strings[item] = str;
    ItemWidthChanged(item);
}

wxString wxVListBoxComboPopup::GetStringValue() const
{
    if ( m_value >= 0 )
        return m_strings[m_value];
    return wxEmptyString;
}

void wxVListBoxComboPopup::SetSelection( int item )
{
    wxCHECK_RET( item == wxNOT_FOUND || ((unsigned int)item < GetCount()),
                 wxT("invalid index in wxVListBoxComboPopup::SetSelection") );

    m_value = item;

    if ( IsCreated() )
        wxVListBox::SetSelection(item);
}

int wxVListBoxComboPopup::GetSelection() const
{
    return m_value;
}

void wxVListBoxComboPopup::SetStringValue( const wxString& value )
{
    int index = m_strings.Index(value);

    m_value = index;

    if ( index >= -1 && index < (int)wxVListBox::GetItemCount() )
        wxVListBox::SetSelection(index);
}

void wxVListBoxComboPopup::CalcWidths()
{
    bool doFindWidest = m_findWidest;

    // Measure items with dirty width.
    if ( m_widthsDirty )
    {
        unsigned int i;
        unsigned int n = m_widths.GetCount();
        int dirtyHandled = 0;
        wxArrayInt& widths = m_widths;

        // I think using wxDC::GetTextExtent is faster than
        // wxWindow::GetTextExtent (assuming same dc is used
        // for all calls, as we do here).
        wxClientDC dc(m_combo);
        dc.SetFont(m_useFont);

        for ( i=0; i<n; i++ )
        {
            if ( widths[i] < 0 )
            {
                wxCoord x = OnMeasureItemWidth(i);

                if ( x < 0 )
                {
                    const wxString& text = m_strings[i];

                    // To make sure performance won't suck in extreme scenarios,
                    // we'll estimate length after some arbitrary number of items
                    // have been checked precily.
                    if ( dirtyHandled < 1024 )
                    {
                        wxCoord y;
                        dc.GetTextExtent(text, &x, &y, 0, 0);
                        x += 4;
                    }
                    else
                    {
                        x = text.length() * (dc.GetCharWidth()+1);
                    }
                }

                widths[i] = x;

                if ( x >= m_widestWidth )
                {
                    m_widestWidth = x;
                    m_widestItem = (int)i;
                }
                else if ( (int)i == m_widestItem )
                {
                    // Width of previously widest item has been decreased, so
                    // we'll have to check all to find current widest item.
                    doFindWidest = true;
                }

                dirtyHandled++;
            }
        }

        m_widthsDirty = false;
    }

    if ( doFindWidest )
    {
        unsigned int i;
        unsigned int n = m_widths.GetCount();

        int bestWidth = -1;
        int bestIndex = -1;

        for ( i=0; i<n; i++ )
        {
            int w = m_widths[i];
            if ( w > bestWidth )
            {
                bestIndex = (int)i;
                bestWidth = w;
            }
        }

        m_widestWidth = bestWidth;
        m_widestItem = bestIndex;

        m_findWidest = false;
    }
}

wxSize wxVListBoxComboPopup::GetAdjustedSize( int minWidth, int prefHeight, int maxHeight )
{
    int height = 250;

    if ( m_strings.GetCount() )
    {
        if ( prefHeight > 0 )
            height = prefHeight;

        if ( height > maxHeight )
            height = maxHeight;

        int totalHeight = GetTotalHeight(); // + 3;
        if ( height >= totalHeight )
        {
            height = totalHeight;
        }
        else
        {
            // Adjust height to a multiple of the height of the first item
            // NB: Calculations that take variable height into account
            //     are unnecessary.
            int fih = GetLineHeight(0);
            int shown = height/fih;
            height = shown * fih;
        }
    }
    else
        height = 50;

    CalcWidths();

    // Take scrollbar into account in width calculations
    int widestWidth = m_widestWidth + wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);
    return wxSize(minWidth > widestWidth ? minWidth : widestWidth,
                  height+2);
}

//void wxVListBoxComboPopup::Populate( int n, const wxString choices[] )
void wxVListBoxComboPopup::Populate( const wxArrayString& choices )
{
    int i;

    int n = choices.GetCount();

    for ( i=0; i<n; i++ )
    {
        const wxString& item = choices.Item(i);
        m_strings.Add(item);
    }

    m_widths.SetCount(n,-1);
    m_widthsDirty = true;

    if ( IsCreated() )
        wxVListBox::SetItemCount(n);

    // Sort the initial choices
    if ( m_combo->GetWindowStyle() & wxCB_SORT )
        m_strings.Sort();

    // Find initial selection
    wxString strValue = m_combo->GetValue();
    if ( strValue.length() )
        m_value = m_strings.Index(strValue);
}

// ----------------------------------------------------------------------------
// wxOwnerDrawnComboBox
// ----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(wxOwnerDrawnComboBox, wxComboCtrl)
END_EVENT_TABLE()


IMPLEMENT_DYNAMIC_CLASS2(wxOwnerDrawnComboBox, wxComboCtrl, wxControlWithItems)

void wxOwnerDrawnComboBox::Init()
{
}

bool wxOwnerDrawnComboBox::Create(wxWindow *parent,
                                  wxWindowID id,
                                  const wxString& value,
                                  const wxPoint& pos,
                                  const wxSize& size,
                                  long style,
                                  const wxValidator& validator,
                                  const wxString& name)
{
    return wxComboCtrl::Create(parent,id,value,pos,size,style,validator,name);
}

wxOwnerDrawnComboBox::wxOwnerDrawnComboBox(wxWindow *parent,
                                           wxWindowID id,
                                           const wxString& value,
                                           const wxPoint& pos,
                                           const wxSize& size,
                                           const wxArrayString& choices,
                                           long style,
                                           const wxValidator& validator,
                                           const wxString& name)
    : wxComboCtrl()
{
    Init();

    Create(parent,id,value,pos,size,choices,style, validator, name);
}

bool wxOwnerDrawnComboBox::Create(wxWindow *parent,
                                  wxWindowID id,
                                  const wxString& value,
                                  const wxPoint& pos,
                                  const wxSize& size,
                                  const wxArrayString& choices,
                                  long style,
                                  const wxValidator& validator,
                                  const wxString& name)
{
    m_initChs = choices;
    //wxCArrayString chs(choices);

    //return Create(parent, id, value, pos, size, chs.GetCount(),
    //              chs.GetStrings(), style, validator, name);
    return Create(parent, id, value, pos, size, 0,
                  NULL, style, validator, name);
}

bool wxOwnerDrawnComboBox::Create(wxWindow *parent,
                                  wxWindowID id,
                                  const wxString& value,
                                  const wxPoint& pos,
                                  const wxSize& size,
                                  int n,
                                  const wxString choices[],
                                  long style,
                                  const wxValidator& validator,
                                  const wxString& name)
{

    if ( !Create(parent, id, value, pos, size, style,
                 validator, name) )
    {
        return false;
    }

    int i;
    for ( i=0; i<n; i++ )
        m_initChs.Add(choices[i]);

    return true;
}

wxOwnerDrawnComboBox::~wxOwnerDrawnComboBox()
{
    if ( m_popupInterface )
        GetVListBoxComboPopup()->ClearClientDatas();
}

void wxOwnerDrawnComboBox::DoSetPopupControl(wxComboPopup* popup)
{
    if ( !popup )
    {
        popup = new wxVListBoxComboPopup();
    }

    wxComboCtrl::DoSetPopupControl(popup);

    wxASSERT(popup);

    // Add initial choices to the wxVListBox
    if ( !GetVListBoxComboPopup()->GetCount() )
    {
        GetVListBoxComboPopup()->Populate(m_initChs);
        m_initChs.Clear();
    }
}

// ----------------------------------------------------------------------------
// wxOwnerDrawnComboBox item manipulation methods
// ----------------------------------------------------------------------------

void wxOwnerDrawnComboBox::Clear()
{
    EnsurePopupControl();

    GetVListBoxComboPopup()->Clear();

    SetValue(wxEmptyString);
}

void wxOwnerDrawnComboBox::Delete(wxIndex n)
{
    wxCHECK_RET( IsValid(n), _T("invalid index in wxOwnerDrawnComboBox::Delete") );

    if ( GetSelection() == (int) n )
        SetValue(wxEmptyString);

    GetVListBoxComboPopup()->Delete(n);
}

wxIndex wxOwnerDrawnComboBox::GetCount() const
{
    if ( !m_popupInterface )
        return m_initChs.GetCount();

    return GetVListBoxComboPopup()->GetCount();
}

wxString wxOwnerDrawnComboBox::GetString(wxIndex n) const
{
    wxCHECK_MSG( IsValid(n), wxEmptyString, _T("invalid index in wxOwnerDrawnComboBox::GetString") );

    if ( !m_popupInterface )
        return m_initChs.Item(n);

    return GetVListBoxComboPopup()->GetString(n);
}

void wxOwnerDrawnComboBox::SetString(wxIndex n, const wxString& s)
{
    EnsurePopupControl();

    wxCHECK_RET( IsValid(n), _T("invalid index in wxOwnerDrawnComboBox::SetString") );

    GetVListBoxComboPopup()->SetString(n,s);
}

int wxOwnerDrawnComboBox::FindString(const wxString& s, bool bCase) const
{
    if ( !m_popupInterface )
        return m_initChs.Index(s, bCase);

    return GetVListBoxComboPopup()->FindString(s, bCase);
}

void wxOwnerDrawnComboBox::Select(int n)
{
    EnsurePopupControl();

    wxCHECK_RET( (n == wxNOT_FOUND) || IsValid(n), _T("invalid index in wxOwnerDrawnComboBox::Select") );

    GetVListBoxComboPopup()->SetSelection(n);

    wxString str;
    if ( n >= 0 )
        str = GetVListBoxComboPopup()->GetString(n);

    // Refresh text portion in control
    if ( m_text )
        m_text->SetValue( str );
    else
        m_valueString = str;

    Refresh();
}

int wxOwnerDrawnComboBox::GetSelection() const
{
    if ( !m_popupInterface )
        return m_initChs.Index(m_valueString);

    return GetVListBoxComboPopup()->GetSelection();
}

int wxOwnerDrawnComboBox::DoAppend(const wxString& item)
{
    EnsurePopupControl();
    wxASSERT(m_popupInterface);

    return GetVListBoxComboPopup()->Append(item);
}

int wxOwnerDrawnComboBox::DoInsert(const wxString& item, wxIndex pos)
{
    EnsurePopupControl();

    wxCHECK_MSG(!(GetWindowStyle() & wxCB_SORT), -1, wxT("can't insert into sorted list"));
#if 0
    wxCHECK_MSG(IsValidInsert(pos), -1, wxT("invalid index"));
#endif

    GetVListBoxComboPopup()->Insert(item,pos);

    return pos;
}

void wxOwnerDrawnComboBox::DoSetItemClientData(wxIndex n, void* clientData)
{
    EnsurePopupControl();

    GetVListBoxComboPopup()->SetItemClientData(n,clientData,m_clientDataItemsType);
}

void* wxOwnerDrawnComboBox::DoGetItemClientData(wxIndex n) const
{
    if ( !m_popupInterface )
        return NULL;

    return GetVListBoxComboPopup()->GetItemClientData(n);
}

void wxOwnerDrawnComboBox::DoSetItemClientObject(wxIndex n, wxClientData* clientData)
{
    DoSetItemClientData(n, (void*) clientData);
}

wxClientData* wxOwnerDrawnComboBox::DoGetItemClientObject(wxIndex n) const
{
    return (wxClientData*) DoGetItemClientData(n);
}

// ----------------------------------------------------------------------------
// wxOwnerDrawnComboBox item drawing and measuring default implementations
// ----------------------------------------------------------------------------

void wxOwnerDrawnComboBox::OnDrawItem( wxDC& dc,
                                       const wxRect& rect,
                                       int item,
                                       int flags ) const
{
    if ( flags & wxODCB_PAINTING_CONTROL )
    {
        dc.DrawText( GetValue(),
                     rect.x + GetTextIndent(),
                     (rect.height-dc.GetCharHeight())/2 + rect.y );
    }
    else
    {
        dc.DrawText( GetVListBoxComboPopup()->GetString(item), rect.x + 2, rect.y );
    }
}

wxCoord wxOwnerDrawnComboBox::OnMeasureItem( size_t WXUNUSED(item) ) const
{
    return -1;
}

wxCoord wxOwnerDrawnComboBox::OnMeasureItemWidth( size_t WXUNUSED(item) ) const
{
    return -1;
}

void wxOwnerDrawnComboBox::OnDrawBackground(wxDC& dc, const wxRect& rect, int item, int flags) const
{
    // we need to render selected and current items differently
    if ( GetVListBoxComboPopup()->IsCurrent((size_t)item) ||
         (flags & wxODCB_PAINTING_CONTROL) )
    {
        DrawFocusBackground(dc,
                            rect,
                            (flags&wxODCB_PAINTING_CONTROL?0:wxCONTROL_ISSUBMENU) |
                            wxCONTROL_SELECTED);
    }
    //else: do nothing for the normal items
}

#endif // wxXTRA_OWNERDRAWNCOMBOBOX
