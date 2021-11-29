#include "unFrame.h"
#include "unApp.h"
#include <filesystem>
#include <fstream>
#include <vector>
#include <cassert>

using std::ifstream;

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
	const wxString logsRelativePath = L"\\Saved\\Logs\\";
	const wxPoint windowPos = wxPoint(400, 500);
	const wxSize windowSize = wxSize(250, 150);

/*	bool isUprojectFileName(const std::string& fileName)
	{
		return fileName.find(uprojectExtName) != std::string::npos;
	}*/

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
			auto iterFilename = entry.path().filename().string();
			if (iterFilename.find(uprojectExtName) != std::string::npos)
			{
				iterFilename.erase(iterFilename.find(uprojectExtName));
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
		const wxString& projectName = tryGetProjectName(projectPath);
		return projectPath + logsRelativePath + projectName + L".log";
	}

	ifstream getFileToReading(const wxString& filePath)
	{
		ifstream file;
		if (!std::filesystem::exists(filePath.wc_str()))
		{
			return file;
		}
		file.open(filePath.wc_str());
		return file;
	}

	void testParseProject(const wxString& projectPath)
	{
		const auto& logFilePath = getPathToProjectLogFile(projectPath);
		auto debug = logFilePath.wc_str();
		ifstream file = getFileToReading(logFilePath);
		if (file.is_open())
		{
			std::vector<std::string> debugLines;
			for (std::string line; std::getline(file, line); )
			{
				debugLines.emplace_back(line);
			}
			bool b = true;
		}
	}

}

using std::string;

bool unFrame::isParsingLoopActive(const wxString& telegrmName, const wxString& projectPath) const
{
/*	if (m_parsingLoopStatus == parsingLoopStatus::Inactive)
	{
		return false;
	}*/
	return tryGetProjectName(projectPath) != wxString{} && telegrmName != wxString{};
}

unFrame::unFrame(unApp* inOwnerApp) : wxFrame(nullptr, wxID_ANY, "Unreal Daemon", windowPos, windowSize,
	(wxMINIMIZE_BOX | wxCLOSE_BOX | wxSYSTEM_MENU | wxCAPTION)),
	m_ownerApp(inOwnerApp)
{
	const wxString browseToDescrText = L"Project path";
	const wxPoint browseToDescrPos = { 150, 10 };
	auto* const fileDialogDescrText = new wxStaticText(this, wxID_ANY, browseToDescrText, browseToDescrPos);
	
	const wxString telegrmLoginDescrText = L"Telegram Login";
	const wxPoint telegrmLoginDescrPos = { 10, 10 };
	auto* const telegrmLoginDescrTextObj = new wxStaticText(this, wxID_ANY, telegrmLoginDescrText, telegrmLoginDescrPos);

	const wxPoint telegrmLoginPos = { 10, 40 };
	m_telegrmLoginTextBox = new wxTextCtrl(this, wxID_ANY, wxEmptyString, telegrmLoginPos, wxDefaultSize, wxTE_PROCESS_ENTER);

	const wxPoint browseToPosition = { 150, 40 };
	auto* const browseToButton = new wxButton(this, wxID_ANY, "Browse to...", browseToPosition);
	browseToButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &unFrame::OnBrowseToClicked, this);

	const wxPoint activationButtonPos = { 75, 70 };
	auto* const activateButton = new wxButton(this, wxID_ANY, "Activate", activationButtonPos);
	activateButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &unFrame::onActivateButtonClicked, this);

	SetFocus();
	//parsingLoop();
}

void unFrame::onActivateButtonClicked(wxCommandEvent& event)
{
	const wxString& telegrmLogin = m_telegrmLoginTextBox->GetValue();
	if (telegrmLogin == wxString{})
	{
		showWarningDialog(wxT("Please Enter Your Telegram Login"));
		return;
	}
	if (tryGetProjectName(m_projectPath) == wxString{})
	{
		showWarningDialog(wxT("Cannot find Uproject file on entered path"));
		return;
	}
	testParseProject(m_projectPath);
	//parsingLoop();
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

void unFrame::parsingLoop()
{
	while (1)
	{
		const wxString& telegrmLogin = m_telegrmLoginTextBox->GetValue();
		if (isParsingLoopActive(telegrmLogin, m_projectPath))
		{

		}
		wxYield();
	}
}