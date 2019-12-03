#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/gdicmn.h>
#include <wx/button.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/frame.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <ffmpeg/FFPlayer.hpp>

class FormMain : public wxFrame {
public:
	FormMain( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
	{
		this->SetSizeHints( wxDefaultSize, wxDefaultSize );

		wxBoxSizer* bSizer1;
		bSizer1 = new wxBoxSizer( wxVERTICAL );

		plTop = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxTAB_TRAVERSAL );
		plTop->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_SCROLLBAR ) );
		plTop->SetMaxSize( wxSize( -1,50 ) );

		wxBoxSizer* bSizer2;
		bSizer2 = new wxBoxSizer( wxHORIZONTAL );

		btOpen = new wxButton( plTop, wxID_ANY, wxT("Open"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizer2->Add( btOpen, 0, wxALL, 5 );

		btClose = new wxButton( plTop, wxID_ANY, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizer2->Add( btClose, 0, wxALL, 5 );

		btPlay = new wxButton( plTop, wxID_ANY, wxT("Play"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizer2->Add( btPlay, 0, wxALL, 5 );

		btPause = new wxButton( plTop, wxID_ANY, wxT("Pause"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizer2->Add( btPause, 0, wxALL, 5 );

		plTop->SetSizer( bSizer2 );
		plTop->Layout();
		bSizer2->Fit( plTop );
		bSizer1->Add( plTop, 1, wxEXPAND | wxALL, 5 );

		plClient = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
		plClient->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNTEXT ) );
		plClient->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNTEXT ) );

		bSizer1->Add( plClient, 1, wxEXPAND | wxALL, 5 );

		this->SetSizer( bSizer1 );
		this->Layout();

		this->Centre( wxBOTH );

		btOpen->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( FormMain::on_btOpen_Click ), NULL, this );
		btClose->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( FormMain::on_btClose_Click ), NULL, this );
		btPlay->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( FormMain::on_btPlay_Click ), NULL, this );
		btPause->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( FormMain::on_btPause_Click ), NULL, this );

		player_.setTargetHandle( plClient->GetHandle() );
		player_.setOnError([](const void* obj, int code, const string msg){
			wxMessageBox(msg, wxT("Error"), wxICON_INFORMATION);
		});
		player_.setOnEOF([](const void* obj){
			wxMessageBox(wxT("끝까지 재생하였습니다."), wxT("Error"), wxICON_INFORMATION);
		});
	}

	~FormMain()
	{
		btOpen->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( FormMain::on_btOpen_Click ), NULL, this );
		btClose->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( FormMain::on_btClose_Click ), NULL, this );
		btPlay->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( FormMain::on_btPlay_Click ), NULL, this );
		btPause->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( FormMain::on_btPause_Click ), NULL, this );

		player_.terminateNow();
	}

private:
	FFPlayer player_;

	void on_btOpen_Click( wxMouseEvent& event ) 
	{
		wxFileDialog* openFileDialog =
			new wxFileDialog(this, _("Open file"), "", "", wxFileSelectorDefaultWildcardStr, wxFD_OPEN, wxDefaultPosition);
		if ( openFileDialog->ShowModal() == wxID_OK ) {
			player_.open( openFileDialog->GetPath() );
		}
	}

	void on_btClose_Click( wxMouseEvent& event ) 
	{ 
		player_.close();
	}

	void on_btPlay_Click( wxMouseEvent& event ) 
	{
		player_.play();
	}

	void on_btPause_Click( wxMouseEvent& event ) 
	{
		player_.pause();
	}

protected:
	wxPanel* plTop;
	wxButton* btOpen;
	wxButton* btClose;
	wxButton* btPlay;
	wxButton* btPause;
	wxPanel* plClient;
};