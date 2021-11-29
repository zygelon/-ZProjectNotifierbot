#include "unFrame.h"
#include "unApp.h"

namespace UnID
{
	enum
	{
		startID = 10000,
		fileDialogDescr,
		fileDialogActivator,
		telegrmLogin
	};
}

namespace
{
	bool validateProjectPath(const wxString& projectPath)
	{
		return true;
	}
}

unFrame::unFrame(unApp* inOwnerApp) : wxFrame(nullptr, wxID_ANY, "Unreal Daemon", wxPoint(400, 500), wxSize(300, 200),
	(wxMINIMIZE_BOX | wxCLOSE_BOX | wxSYSTEM_MENU | wxCAPTION)),
	m_ownerApp(inOwnerApp)
{
	const wxString browseToDescrText = L"Project folder";
	const wxPoint browseToDescrPos = { 150, 50 };
	auto* const fileDialogDescrText = new wxStaticText(this, wxID_ANY, browseToDescrText, browseToDescrPos);
	
	const wxString telegrmLoginDescrText = L"Telegram Login";
	const wxPoint telegrmLoginDescrPos = { 10, 50 };
	auto* const telegrmLoginDescrTextObj = new wxStaticText(this, wxID_ANY, telegrmLoginDescrText, telegrmLoginDescrPos);

	const wxPoint telegrmLoginPos = { 10, 70 };
	m_telegrmLoginTextBox = new wxTextCtrl(this, wxID_ANY, wxEmptyString, telegrmLoginPos, wxDefaultSize, wxTE_PROCESS_ENTER);
	//m_telegrmLoginTextBox->Bind(wxEVT_TEXT_ENTER, &unFrame::OnTelegrmLoginEntered, this);

	const wxPoint browseToPosition = { 150, 70 };
	m_browseToButton = new wxButton(this, wxID_ANY, "Browse to...", browseToPosition);
	m_browseToButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &unFrame::OnBrowseToClicked, this);

	const wxPoint activationButtonPos = { 150, 110 };
	auto* const activateButton = new wxButton(this, wxID_ANY, "Activate", activationButtonPos);
	activateButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &unFrame::onActivateButtonClicked, this);

}

void unFrame::onActivateButtonClicked(wxCommandEvent& event)
{
	const wxString telegrmLogin = m_telegrmLoginTextBox->GetValue();
}

unFrame::~unFrame()
{
}
/* TODO: validate telegram login
void unFrame::OnTelegrmLoginEntered(wxCommandEvent& event)
{
}
*/
void unFrame::OnBrowseToClicked(wxCommandEvent& event)
{
	wxDirDialog openDirDialog(this, _("Open folder with your project"), 
		"", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
	if (openDirDialog.ShowModal() == wxID_CANCEL)
	{
		return;     // the user changed idea...
	}
	event.Skip();
	const bool isValidProjectPath = validateProjectPath(openDirDialog.GetPath());
}
