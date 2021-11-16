#include "unFrame.h"
#include "unApp.h"

namespace UnID
{
	enum
	{
		startID = 10000,
		fileDialogDescr,
		fileDialogActivator,
		boxSizer
	};
}

unFrame::unFrame(unApp* inOwnerApp) : wxFrame(nullptr, wxID_ANY, "Unreal Daemon", wxPoint(500, 500), wxSize(400, 200),
	(wxMINIMIZE_BOX | wxCLOSE_BOX | wxSYSTEM_MENU | wxCAPTION)),
	m_ownerApp(inOwnerApp)
{
	const wxString descrText = "Subscribe daemon to your project.\n\Browse to your root UE Project folder";
	const wxPoint DescrTextPosition = { 100, 20 };
	m_fileDialogDescrText = new wxStaticText(this, UnID::fileDialogDescr, descrText, DescrTextPosition);
	const wxPoint activatorPosition = { 150, 70 };
	m_fileDialogActivator = new wxButton(this, UnID::fileDialogActivator, "Browse to...", activatorPosition);
	m_fileDialogActivator->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &unFrame::OnBrowseToClicked, this);

	//wxStaticBox* boxSizer = new wxStaticBox(this, wxID_ANY);
	//wxGridSizer* grid = new wxGridSizer(5, 10, 100, 100);
	//grid->Add(m_fileDialogDescrText);
	//grid->Add(m_fileDialogActivator);

	//SetSizer(grid);
	//grid->Layout();


}

unFrame::~unFrame()
{
}

void unFrame::OnBrowseToClicked(wxCommandEvent& evt)
{
	wxDirDialog openDirDialog(this, _("Open folder with your project"), 
		"", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
	if (openDirDialog.ShowModal() == wxID_CANCEL)
	{
		return;     // the user changed idea...
	}
	m_ownerApp->SetRootPath(openDirDialog.GetPath());
	evt.Skip();
}

//void unFrame::OnOpen(wxCommandEvent& WXUNUSED)
//{
//}
