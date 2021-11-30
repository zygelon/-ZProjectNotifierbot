#include "unFrame.h"
#include "unApp.h"
#include <filesystem>
#include <fstream>
#include <vector>
#include <cassert>
#define CURL_STATICLIB
#include "curl/curl.h"
using std::wifstream;
using std::wstring;

namespace UnID
{
	enum
	{
		startID = 10000,
	};
}

namespace
{
	const wstring uprojectExtName = L".uproject";
	const wxString logsRelativePath = L"\\Saved\\Logs\\";
	const wxPoint windowPos = wxPoint(400, 500);
	const wxSize windowSize = wxSize(270, 150);

	const wxString checkboxOnImageName = L"CheckBox_Off.png";
	const wxString checkboxOffImageName = L"CheckBox_On.png";

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
			auto iterFilename = entry.path().filename().wstring();
			if (iterFilename.find(uprojectExtName) != wstring::npos)
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

	wifstream getFileToReading(const wxString& filePath)
	{
		wifstream file;
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
		wifstream file = getFileToReading(logFilePath);
		if (file.is_open())
		{
			std::vector<std::wstring> debugLines;
			for (std::wstring line; std::getline(file, line); )
			{
				debugLines.emplace_back(line);
			}
			bool b = true;
		}
	}
}

void unFrame::updateBrowseToCheckbox()
{
	if (m_browseToCheckboxImage)
	{
		m_browseToCheckboxImage->Destroy();
	}
	
	const wxString checkboxImageName = tryGetProjectName(m_projectPath) == wxString{} ? checkboxOffImageName : checkboxOnImageName;
	
	wxPNGHandler* handler = new wxPNGHandler;
	wxImage::AddHandler(handler);

	const wxSize browseToCheckboxSize = { 15, 15 };
	const wxPoint browseToCheckboxPos = { 170, 40 };
	//m_browseToCheckboxImage = new wxStaticBitmap(this, wxID_ANY, wxBitmap(checkboxImageName, wxBITMAP_TYPE_PNG), browseToCheckboxPos);
	//new wxImage(

	//m_browseToCheckboxImage->SetScaleMode(wxStaticBitmapBase::ScaleMode::);
	m_browseToCheckboxImage->Refresh();
}

bool unFrame::isParsingLoopActive(const wxString& telegrmName, const wxString& projectPath) const
{

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

	updateBrowseToCheckbox();

	SetFocus();

	//DEBUG
	curl_global_init(CURL_GLOBAL_ALL);
	const wxPoint telegramMessagebuttonPos = { 160, 70 };
	auto* const sendTelegrmMessage = new wxButton(this, wxID_ANY, "Send Tlgrm", telegramMessagebuttonPos);
	sendTelegrmMessage->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &unFrame::onTelegrmMessageClicked, this);
	

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
	updateBrowseToCheckbox();
}

void unFrame::onTelegrmMessageClicked(wxCommandEvent& event)
{
	if (CURL* curl = curl_easy_init())
	{
		std::string readBuffer;
		const std::string url = "https://api.telegram.org/botToken/getUpdates";
		curl_easy_setopt(curl, CURLOPT_URL, url);
	}
}
/*
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
*/