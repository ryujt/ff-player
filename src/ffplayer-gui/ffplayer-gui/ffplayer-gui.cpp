#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "FormMain.hpp"

class MyApp : public wxApp
{
public:
	virtual bool OnInit();
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
	FormMain* frame = new FormMain(NULL, 0, "FF Player", wxPoint(50, 50), wxSize(800, 500), wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
	frame->Show(true);
	return true;
}