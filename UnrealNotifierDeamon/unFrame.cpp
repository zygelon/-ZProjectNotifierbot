#include "unFrame.h"
#include "unApp.h"
#include "credentials.h"
#include <filesystem>
#include <fstream>
#include <vector>
#include <cassert>
#define CURL_STATICLIB
#include "curl/curl.h"
#include <nlohmann/json.hpp>

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
	const wstring resourcesFolder = L"Resources\\";
	const wstring uprojectExtName = L".uproject";
	const wxString logsRelativePath = L"\\Saved\\Logs\\";
	const wxPoint windowPos = wxPoint(400, 500);
	const wxSize windowSize = wxSize(270, 150);

	const wxString checkboxOnImageName =  resourcesFolder + L"CheckBox_On.png";
	const wxString checkboxOffImageName = resourcesFolder + L"CheckBox_Off.png";

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

void unFrame::updateImageCheckbox(wxStaticBitmap* checkbox, const wxPoint& pos, const bool checkboxValue)
{
	if (checkbox)
	{
		checkbox->Destroy();
	}
	const wxString checkboxImageName = checkboxValue ? checkboxOnImageName : checkboxOffImageName;
	
	wxPNGHandler* handler = new wxPNGHandler;
	wxImage::AddHandler(handler);
	
	wxBitmap imageBitmap = { checkboxImageName, wxBITMAP_TYPE_PNG };
	checkbox = new wxStaticBitmap(this, wxID_ANY,
		imageBitmap, pos);
	wxImage checkboxImage = imageBitmap.ConvertToImage();
	checkboxImage.Rescale(20, 20);
	checkbox->SetBitmap({ checkboxImage });
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
	const wxSize telegrmLoginSize = { 75, 20 };
	auto debug = wxDefaultSize;
	m_telegrmLoginTextBox = new wxTextCtrl(this, wxID_ANY, wxEmptyString, telegrmLoginPos, telegrmLoginSize, wxTE_PROCESS_ENTER);
	m_telegrmLoginTextBox->Bind(wxEVT_COMMAND_TEXT_UPDATED, &unFrame::onTelegrmLoginChanged, this);

	const wxPoint browseToPosition = { 150, 40 };
	auto* const browseToButton = new wxButton(this, wxID_ANY, "Browse to...", browseToPosition);
	browseToButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &unFrame::onBrowseToClicked, this);

	const wxPoint activationButtonPos = { 75, 70 };
	auto* const activateButton = new wxButton(this, wxID_ANY, "Activate", activationButtonPos);
	activateButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &unFrame::onActivateButtonClicked, this);

	updateBrowseToImageCheckbox();
	updateTelegrmImageCheckbox();
	SetFocus();

	//DEBUG
	curl_global_init(CURL_GLOBAL_ALL);
	const wxPoint telegramMessagebuttonPos = { 160, 70 };
	auto* const sendTelegrmMessage = new wxButton(this, wxID_ANY, "Send Tlgrm", telegramMessagebuttonPos);
	sendTelegrmMessage->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &unFrame::onTelegrmMessageClicked, this);
	

	//parsingLoop();
}
void unFrame::onTelegrmLoginChanged(wxCommandEvent& event)
{
	updateTelegrmImageCheckbox();
}

void unFrame::updateBrowseToImageCheckbox()
{
	const bool checkboxValue = tryGetProjectName(m_projectPath) != wxString{};
	updateImageCheckbox(m_browseToCheckboxImage, wxPoint{ 230, 40 }, checkboxValue);
}

void unFrame::updateTelegrmImageCheckbox()
{
	const bool checkboxValue = m_telegrmLoginTextBox->GetValue() != wxString{};
	updateImageCheckbox(m_browseToCheckboxImage, wxPoint{ 90, 40 }, checkboxValue);
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
void unFrame::onBrowseToClicked(wxCommandEvent& event)
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
	m_projectPath = projectPath;
	if (!isValidProjectPath)
	{
		showWarningDialog(wxT("Cannot find Uproject file on entered path"));
	}
	updateBrowseToImageCheckbox();
}

static size_t WriteCallback(char* data, size_t size, size_t nmemb, void* _)
{
	using namespace nlohmann;
	// just a place for the cast

//	size_t        rc = 0;
//	std::string* stp = reinterpret_cast<std::string*>(userdata);
	  // construct the JSON root object
	json j = json::parse(data);
	auto debugVal = j["ok"].get<bool>();
	auto debugVal2 = j["ok"];

	auto debugVal3 = j["result"];
	auto debugVal4 = j["result"][0];
	//auto debugVal2 = j["result"].get<std::string>();
	return 0;
}


void unFrame::onTelegrmMessageClicked(wxCommandEvent& event)
{
	if (CURL* curl = curl_easy_init())
	{
		curl_easy_setopt(curl, CURLOPT_URL, credentials::telegramUpdateUrl.c_str());
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "http");
		//curl_easy_setopt(curl, CUROPT_READDATA, )
		//struct curl_slist* headers = NULL;
		//curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		//curl_easy_setopt(curl, CURLOPT_HOST, "LocalHost");

		std::string readBuffer;
		char buffer[10000] = {};
		//curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

		CURLcode res = curl_easy_perform(curl);
		wxASSERT(res == CURLcode::CURLE_OK);
		/*
		curl_mime* mime = curl_mime_init(curl);
		curl_mimepart* part = curl_mime_addpart(mime);
		curl_mime_name(part, "chat_id");
		curl_mime_filedata(part, "chat_id")*/
		//curl_mime
		bool b = true;
		curl_easy_cleanup(curl);
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