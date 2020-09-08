/////////////////////////////////////////////////////////////////////////////
// Name:        navigating-the-application.h
// Purpose:     Accessing the application context
// Author:      Xoanon Graphics team
// Licence:     MIT
/////////////////////////////////////////////////////////////////////////////

/**

@page navigating-the-application Navigating the application.

@tableofcontents


At times it is convenient to access important objects in the
application directly in a top-down way, rather than through the
current object.


Since most program logic occures within class methods, it's usually
the case that a this is made available. However, it sometimes
inconvenient to access other objects, e.g. by finding the child of a
parent of a parent, etc.

CCamApp inherits from wxApp and therefore is the top-level application
object. CCamApp::OnInit() is responsible for much of the application
initialization. The CCamApp application can be accessed by
GetApplication().


DocView represents the way in which a loaded Document is viewed in the
aplication. To ge the current DocView, DocView::GetSelected() to get
the render window of the current DocView
DocView::GetCurrentRenderWindow() This returns a object of type
CNativeWnd which is a typedef for a wxWindow




**/
