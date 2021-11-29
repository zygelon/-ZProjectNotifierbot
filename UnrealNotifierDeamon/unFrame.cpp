#include "unFrame.h"
#include "unApp.h"
#include <filesystem>
#include <vector>
#include <cassert>

namespace UnID
{
	enum
	{
		startID = 10000,
	};
}

namespace
{
	const std::string uprojectExtName = ".uproject";
	const wxString logsRelativePath = L"\\Saved\\Logs\\Challange.log";

	bool isUprojectFileName(const std::string& fileName)
	{
		return fileName.find(uprojectExtName) != std::string::npos;
	}

	wxString tryGetProjectName(const wxString& projectPath)
	{
		namespace fs = std::filesystem;
		fs::path path = projectPath.wc_str();
		if (!fs::is_directory(fs::path{ projectPath.wc_str() }))
		{
			return {};
		}
		for (const auto& entry : fs::directory_iterator(projectPath.wc_str()))
		{
			const auto& iterFilename = entry.path().filename().string();
			if (isUprojectFileName(iterFilename))
			{
				return iterFilename;
			}
		}
		return {};
	}

	void showWarningDialog(const wxString& dialogMessage)
	{
		auto* const warningDialog = new wxMessageDialog(NULL,
			dialogMessage, wxT("Info"), wxOK);
		warningDialog->ShowModal();
	}

	wxString getPathToProjectLogFile(const wxString& projectPath)
	{
		return projectPath + logsRelativePath;
	}

	void testParseProject(const wxString& projectPath)
	{
		const auto& logFilePath = getPathToProjectLogFile(projectPath);
		if (!std::filesystem::exists(logFilePath.wc_str()))
		{
			return;
		}
		bool b = true;
	}
}

unFrame::unFrame(unApp* inOwnerApp) : wxFrame(nullptr, wxID_ANY, "Unreal Daemon", wxPoint(400, 500), wxSize(300, 200),
	(wxMINIMIZE_BOX | wxCLOSE_BOX | wxSYSTEM_MENU | wxCAPTION)),
	m_ownerApp(inOwnerApp)
{
	const wxString browseToDescrText = L"Project path";
	const wxPoint browseToDescrPos = { 150, 50 };
	auto* const fileDialogDescrText = new wxStaticText(this, wxID_ANY, browseToDescrText, browseToDescrPos);
	
	const wxString telegrmLoginDescrText = L"Telegram Login";
	const wxPoint telegrmLoginDescrPos = { 10, 50 };
	auto* const telegrmLoginDescrTextObj = new wxStaticText(this, wxID_ANY, telegrmLoginDescrText, telegrmLoginDescrPos);

	const wxPoint telegrmLoginPos = { 10, 70 };
	m_telegrmLoginTextBox = new wxTextCtrl(this, wxID_ANY, wxEmptyString, telegrmLoginPos, wxDefaultSize, wxTE_PROCESS_ENTER);

	const wxPoint browseToPosition = { 150, 70 };
	auto* const browseToButton = new wxButton(this, wxID_ANY, "Browse to...", browseToPosition);
	browseToButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &unFrame::OnBrowseToClicked, this);

	const wxPoint activationButtonPos = { 150, 110 };
	auto* const activateButton = new wxButton(this, wxID_ANY, "Activate", activationButtonPos);
	activateButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &unFrame::onActivateButtonClicked, this);

}

void unFrame::onActivateButtonClicked(wxCommandEvent& event)
{
	const wxString& telegrmLogin = m_telegrmLoginTextBox->GetValue();
	if (telegrmLogin == wxString{})
	{
		showWarningDialog(wxT("Please Enter Your Telegram Login"));
		return;
	}
	if (tryGetProjectName(m_projectPath) != wxString{})
	{
		showWarningDialog(wxT("Cannot find Uproject file on entered path"));
		return;
	}
	testParseProject(m_projectPath);
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
	const wxString& projectPath = openDirDialog.GetPath();
	const bool isValidProjectPath = tryGetProjectName(projectPath) != wxString{};
	if (isValidProjectPath)
	{
		m_projectPath = projectPath;
	}
	else
	{
		showWarningDialog(wxT("Cannot find Uproject file on entered path"));
	}
}
