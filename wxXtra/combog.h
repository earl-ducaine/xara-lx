// $Id: combog.h 1417 2006-07-06 19:56:27Z alex $
/* @@tag:xara-cn-tp@@ THIRD PARTY COPYRIGHT */
// The following line makes normalize.pl skip type fixing
/* SKIPFIXTYPES: START */

/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/combo.h
// Purpose:     Generic wxComboControl
// Author:      Jaakko Salli
// Modified by:
// Created:     Apr-30-2006
// RCS-ID:      
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WXXTRA_GENERIC_COMBOCONTROL_H_
#define _WXXTRA_GENERIC_COMBOCONTROL_H_

#include <wx/wx.h>
#include <wx/xrc/xmlres.h>

#if wxUSE_COMBOCTRL
#undef wxXTRA_COMBOCTRL
#else
#define wxXTRA_COMBOCTRL 1

#include "combo.h"

// Only define generic if native doesn't have all the features
#if !defined(wxCOMBOCONTROL_FULLY_FEATURED)

// ----------------------------------------------------------------------------
// Generic wxComboCtrl
// ----------------------------------------------------------------------------

#if defined(__WXUNIVERSAL__)

// all actions of single line text controls are supported

// popup/dismiss the choice window
#define wxACTION_COMBOBOX_POPUP     _T("popup")
#define wxACTION_COMBOBOX_DISMISS   _T("dismiss")

#endif

extern WXDLLIMPEXP_DATA_CORE(const wxChar *) wxComboBoxNameStr;

class WXDLLEXPORT wxGenericComboControl : public wxComboCtrlBase
{
public:
    // ctors and such
    wxGenericComboControl() : wxComboCtrlBase() { Init(); }

    wxGenericComboControl(wxWindow *parent,
                          wxWindowID id = wxID_ANY,
                          const wxString& value = wxEmptyString,
                          const wxPoint& pos = wxDefaultPosition,
                          const wxSize& size = wxDefaultSize,
                          long style = 0,
                          const wxValidator& validator = wxDefaultValidator,
                          const wxString& name = wxComboBoxNameStr)
        : wxComboCtrlBase()
    {
        Init();

        (void)Create(parent, id, value, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxComboBoxNameStr);

    virtual ~wxGenericComboControl();

    static int GetFeatures() { return wxComboCtrlFeatures::All; }

#if defined(__WXUNIVERSAL__)
    // we have our own input handler and our own actions
    virtual bool PerformAction(const wxControlAction& action,
                               long numArg = 0l,
                               const wxString& strArg = wxEmptyString);
#endif

protected:

    // Mandatory virtuals
    virtual void OnResize();

    // Event handlers
    void OnPaintEvent( wxPaintEvent& event );
    void OnMouseEvent( wxMouseEvent& event );

private:
    void Init();

    DECLARE_EVENT_TABLE()

    DECLARE_DYNAMIC_CLASS(wxGenericComboControl)
};


#ifndef _WX_COMBOCONTROL_H_

// If native wxComboCtrl was not defined, then prepare a simple
// front-end so that wxRTTI works as expected.

class WXDLLEXPORT wxComboCtrl : public wxGenericComboControl
{
public:
    wxComboCtrl() : wxGenericComboControl() {}

    wxComboCtrl(wxWindow *parent,
                   wxWindowID id = wxID_ANY,
                   const wxString& value = wxEmptyString,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxComboBoxNameStr)
        : wxGenericComboControl()
    {
        (void)Create(parent, id, value, pos, size, style, validator, name);
    }

    virtual ~wxComboCtrl() {}

protected:

private:
    DECLARE_DYNAMIC_CLASS(wxComboCtrl)
};

#endif // _WX_COMBOCONTROL_H_

#else

#define wxGenericComboControl   wxComboCtrl

#endif // !defined(wxCOMBOCONTROL_FULLY_FEATURED)

#endif // wxUSE_COMBOCONTROL
#endif
    // _WX_GENERIC_COMBOCONTROL_H_
