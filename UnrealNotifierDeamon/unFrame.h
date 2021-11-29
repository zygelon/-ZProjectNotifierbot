#pragma once

#include "wx/wx.h"

class unApp;

class unFrame : public wxFrame
{
public:
	unFrame(unApp* inOwnerApp);
	~unFrame();

	//virtual void OnOpen(wxCommandEvent& WXUNUSED(event)) override;

private:
	unApp* m_ownerApp = nullptr;

	wxTextCtrl* m_telegrmLoginTextBox = nullptr;
	wxString m_projectPath = {};
	//wxButton* m_browseToButton = nullptr;

	void onActivateButtonClicked(wxCommandEvent& event);
	void OnBrowseToClicked(wxCommandEvent& event);
	//void OnTelegrmLoginEntered(wxCommandEvent& event);
};