#include "unFrame.h"
#include "unApp.h"
#include <filesystem>
#include <fstream>
#include <vector>
#include <cassert>
#include "tlgrm.h"

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
	namespace wconst
	{
		const wstring resourcesFolder = L"Resources\\";
		const wstring uprojectExtName = L".uproject";
		const wxString logsRelativePath = L"\\Saved\\Logs\\";
		const wxPoint windowPos = wxPoint(400, 500);
		const wxSize windowSize = wxSize(270, 150);

		const wxString checkboxOnImageName = resourcesFolder + L"CheckBox_On.png";
		const wxString checkboxOffImageName = resourcesFolder + L"CheckBox_Off.png";
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
			auto iterFilename = entry.path().filename().wstring();
			if (iterFilename.find(wconst::uprojectExtName) != wstring::npos)
			{
				iterFilename.erase(iterFilename.find(wconst::uprojectExtName));
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
		return projectPath + wconst::logsRelativePath + projectName + L".log";
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

void unFrame::updateImageCheckbox(wxStaticBitmap** checkboxPtr, const wxPoint& pos, const bool checkboxValue)
{
	wxStaticBitmap* checkbox{ *checkboxPtr };
	if (checkbox)
	{
		const auto& checkboxOldValue = [&checkbox]() -> std::optional<bool> {
			const wxString& checkboxImageName = checkbox->GetName();
			if (checkboxImageName == wconst::checkboxOnImageName)
			{
				return true;
			}
			else if (checkboxImageName == wconst::checkboxOffImageName)
			{
				return false;
			}
			return std::optional<bool>{};
		} ();

		if (checkboxOldValue.has_value() && checkboxOldValue.value() == checkboxValue)
		{
			return;
		}
		checkbox->Destroy();
	}
	const wxString checkboxImageName = checkboxValue ? wconst::checkboxOnImageName 
		: wconst::checkboxOffImageName;
	
	wxPNGHandler* handler = new wxPNGHandler;
	wxImage::AddHandler(handler);
	
	wxBitmap imageBitmap = { checkboxImageName, wxBITMAP_TYPE_PNG };
	checkbox = new wxStaticBitmap(this, wxID_ANY,
		imageBitmap, pos, wxDefaultSize, 0L, checkboxImageName);
	wxImage checkboxImage = imageBitmap.ConvertToImage();
	checkboxImage.Rescale(20, 20);
	checkbox->SetBitmap({ checkboxImage });
	*checkboxPtr = checkbox;
}

bool unFrame::isParsingLoopActive(const wxString& telegrmName, const wxString& projectPath) const
{
	return tryGetProjectName(projectPath) != wxString{} && telegrmName != wxString{};
}

unFrame::unFrame(unApp* inOwnerApp) : wxFrame(nullptr, wxID_ANY, "Unreal Daemon", wconst::windowPos, wconst::windowSize,
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
	updateImageCheckbox(&m_browseToCheckboxImage, wxPoint{ 230, 40 }, checkboxValue);
}

void unFrame::updateTelegrmImageCheckbox()
{
	const bool checkboxValue =
		[this]() {
		const std::string tlgrmLogin = m_telegrmLoginTextBox->GetValue().ToStdString();
		return tlgrmLogin != std::string{} && tlgrm::getChatId(tlgrmLogin).has_value();
	}();
	updateImageCheckbox(&m_telegrmCheckboxImage, wxPoint{ 90, 40 }, checkboxValue);
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

void unFrame::onTelegrmMessageClicked(wxCommandEvent& event)
{
	const wxString& tlgrmLogin = m_telegrmLoginTextBox->GetValue();
	const auto& chatId = tlgrm::getChatId(tlgrmLogin.ToStdString());
	if (chatId.has_value())
	{
		tlgrm::sendMessage("Test message from zygelon", chatId.value());
	}
	bool debugval = true;
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