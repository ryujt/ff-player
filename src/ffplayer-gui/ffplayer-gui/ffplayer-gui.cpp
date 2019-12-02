#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <ffmpeg/FFPlayer.hpp>

class MyApp : public wxApp
{
public:
	virtual bool OnInit();
};

class FormMain : public wxFrame
{
private:
	FFPlayer player_;
protected:
	wxButton* bt_hello;
	wxButton* bt_test;

	void on_click(wxMouseEvent& event) 
	{  
		player_.open("D:/Work/test.mp4");
		player_.play();
	}
public:
	FormMain(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(500, 300), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
	~FormMain();
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
	FormMain* frame = new FormMain(NULL, 0, "Hi", wxPoint(50, 50), wxSize(800, 450), wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
	frame->Show(true);
	return true;
}

FormMain::FormMain(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxVERTICAL);

	bt_hello = new wxButton(this, wxID_ANY, wxT("MyButton"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer1->Add(bt_hello, 0, wxALL, 5);

	this->SetSizer(bSizer1);
	this->Layout();
	this->Centre(wxBOTH);

	bt_hello->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(FormMain::on_click), NULL, this);

	player_.setTargetHandle( GetHandle() );
}

FormMain::~FormMain()
{
	player_.terminateNow();
	bt_hello->Disconnect(wxEVT_LEFT_DOWN, wxMouseEventHandler(FormMain::on_click), NULL, this);
}