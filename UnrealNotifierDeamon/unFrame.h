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

	wxStaticText* m_fileDialogDescrText = nullptr;
	wxButton* m_fileDialogActivator = nullptr;

	void OnBrowseToClicked(wxCommandEvent& evt);
	//wxFileDialog* m_fileDialog;
};