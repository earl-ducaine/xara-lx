/////////////////////////////////////////////////////////////////////////////
// Name:        rendering.h
// Purpose:     topic overview
// Author:      Xoanon Graphics team
// Licence:     MIT
/////////////////////////////////////////////////////////////////////////////

/**

@page application-framework Application Framework

@tableofcontents

The section provides an overview of Xoanon Graphics, especiallially
the interface between the aplication and the application toolkit
system, wxWidgets, and the relationship between major components of
the system.

The application is built on top of the wxWidgets framework. However,
it differs from most wxWidget-based applications in that it uses many
of its own UI compponents rather than those of the toolkit, and that
there is an abstraction layer that sits between the Kernal application
and the wxWidgets toolkit, wxOil ([wx]Widgets [O]perating system
[i]nterface [l]ayer).

In the words of the wxWidgets project: "wxWidgets is an open source
C++ framework allowing to write cross-platform GUI applications with
native look and feel in C++ and other languages."

wxWidgets provides three basic interface styles: MDI (multiple
document interface), SDI (single document interface) and single-window
(e.g. Microsoft Write). The applecation uses the MDI style. For more
details see; https://docs.wxwidgets.org/trunk/overview_docview.html

The startup process is as follows:

@li Through various macrology, a singleton instance of @ref CCamApp,
    which inherits from wxApp, is instantiated and control is passed
    to @ref CCamApp::OnInit, which sets up various things needed for
    wxWidget's UI components to run, e.g. command line argument
    processing, exception handling policy, initial log messages,
    configuring the debug environment, policy and handling of multiple
    running instances, registering of certain components like image handers, accessing
    file system resources like the user's preference file, create an
    instance of @ref CCamDocTemplate (this inherits from
    wxDocTemplate) and most importantly initializing the Xoanon
    Graphics kernal, especially: @ref InitKernel, @ref
    Application::LateInit

@li Once a new instance of @ref CCamDocTemplate is created, new
    instances of the the graphic object can be instanciated and
    'hooked up' to the view. By default, Camelot has one new document
    opened upon starting up.


@section Event Handling

Xoanon Gra




*/
