#include "unFrame.h"
#include "tlgrm.h"
#include "xlog.h"
#include "unApp.h"

#include <wx/debug.h>

#include <filesystem>
#include <fstream>
#include <vector>
#include <utility>

using std::wifstream;
using std::wstring;

namespace EUnID
{
	enum
	{
		startID = 10000,
		parserTimerID
	};
}

namespace
{
	namespace wconst
	{
		const wstring resourcesFolder = L"Resources\\";
		const wstring uprojectExtName = L".uproject";
		const wxString logsRelativePath = L"\\Saved\\Logs\\";

		const wxString iconName = L"small.ico";

		const wxSize windowSize = wxSize(270, 150);

		const wxString checkboxOnImageName = resourcesFolder + L"CheckBox_On.png";
		const wxString checkboxOffImageName = resourcesFolder + L"CheckBox_Off.png";

		const int firstLineY = 10;
		const int secondLineY = 30;
		const int thirdLineY = 60;
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

	void updateImageCheckbox(wxStaticBitmap** checkboxPtr, const wxPoint& pos, const bool checkboxValue, wxPanel* parent)
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
		checkbox = new wxStaticBitmap(parent, wxID_ANY,
			imageBitmap, pos, wxDefaultSize, 0L, checkboxImageName);
		wxImage checkboxImage = imageBitmap.ConvertToImage();
		checkboxImage.Rescale(20, 20);
		checkbox->SetBitmap({ checkboxImage });
		*checkboxPtr = checkbox;
	}

	std::pair<wxCheckBox*, wxStaticBitmap*> addSubscrLine(const int lineY, const wxString& text, const bool checkboxDefaultValue, wxPanel* const parent)
	{
		wxASSERT(parent != nullptr);
		const wxPoint subscrTypeTextPos{ 10, lineY };
		auto* const subscrTypeDescrText = new wxStaticText(parent, wxID_ANY, text, subscrTypeTextPos);
		wxStaticBitmap* imageCheckbox = nullptr;
		updateImageCheckbox(&imageCheckbox, wxPoint{ 100, lineY - 3 }, false, parent);

		const wxPoint subscrDescrTextPos{ 130, lineY };
		auto* const subscrDescrText = new wxStaticText(parent, wxID_ANY, L"subscribe", subscrDescrTextPos);

		const wxPoint checkboxPos{ 183, lineY };
		auto* checkbox = new wxCheckBox(parent, wxID_ANY, L"", checkboxPos);
		checkbox->SetValue(checkboxDefaultValue);

		return std::make_pair(checkbox, imageCheckbox);
	}
}

bool unFrame::isParsingLoopActive(const wxString& telegrmName, const wxString& projectPath) const
{
	return tryGetProjectName(projectPath) != wxString{} && 
		tlgrm::getChatId(telegrmName.ToStdString());
}

unFrame::unFrame(unApp* inOwnerApp) : wxFrame(nullptr, wxID_ANY, "Unreal Daemon", wxDefaultPosition, wconst::windowSize,
	(wxMINIMIZE_BOX | wxCLOSE_BOX | wxSYSTEM_MENU | wxCAPTION)),
	m_ownerApp(inOwnerApp),
	m_parsingLoopTimer( this, EUnID::parserTimerID )
{
	m_panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wconst::windowSize);

	const wxString telegrmLoginDescrText = L"Telegram Login";
	const wxPoint telegrmLoginDescrPos = { 10, wconst::firstLineY };
	auto* const telegrmLoginDescrTextObj = new wxStaticText(m_panel, wxID_ANY, telegrmLoginDescrText, telegrmLoginDescrPos);

	const wxString browseToDescrText = L"Project path";
	const wxPoint browseToDescrPos = { 150, wconst::firstLineY };
	auto* const fileDialogDescrText = new wxStaticText(m_panel, wxID_ANY, browseToDescrText, browseToDescrPos);

	const wxPoint telegrmLoginPos = { 10, wconst::secondLineY };
	const wxSize telegrmLoginSize = { 75, 20 };

	m_telegrmLoginTextBox = new wxTextCtrl(m_panel, wxID_ANY, wxEmptyString, telegrmLoginPos, telegrmLoginSize, wxTE_PROCESS_ENTER);
	m_telegrmLoginTextBox->Bind(wxEVT_COMMAND_TEXT_UPDATED, &unFrame::onTelegrmLoginChanged, this);

	const wxPoint browseToPosition = { 150, wconst::secondLineY };
	auto* const browseToButton = new wxButton(m_panel, wxID_ANY, "Browse to...", browseToPosition);
	browseToButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &unFrame::onBrowseToClicked, this);

	m_parsingLoopTimer.SetOwner(this);
	this->Connect(m_parsingLoopTimer.GetId(), wxEVT_TIMER,
		wxTimerEventHandler(unFrame::parsingLoop), NULL, this);
	m_parsingLoopTimer.Start(7'000);

	std::pair<wxCheckBox*, wxStaticBitmap*> startEditorCheckboxes = 
		addSubscrLine(wconst::thirdLineY, L"Start Editor", true, m_panel);

	m_inputStartEditorCheckbox = startEditorCheckboxes.first;
	m_startEditorImageCheckbox = startEditorCheckboxes.second;

	updateBrowseToImageCheckbox();
	updateTelegrmImageCheckbox();

	Center();
	SetFocus();
}
/*
void unFrame::addEditorStartLine()
{
	const wxString startEditorText = L"Start Editor";
}
*/
void unFrame::onTelegrmLoginChanged(wxCommandEvent& event)
{
	updateTelegrmImageCheckbox();
}

void unFrame::updateBrowseToImageCheckbox()
{
	const bool checkboxValue = tryGetProjectName(m_projectPath) != wxString{};
	updateImageCheckbox(&m_browseToCheckboxImage, wxPoint{ 230, wconst::secondLineY }, checkboxValue, m_panel);
}

void unFrame::updateTelegrmImageCheckbox()
{
	const bool checkboxValue =
		[this]() {
		const std::string tlgrmLogin = m_telegrmLoginTextBox->GetValue().ToStdString();
		return tlgrmLogin != std::string{} && tlgrm::getChatId(tlgrmLogin).has_value();
	}();
	updateImageCheckbox(&m_telegrmCheckboxImage, wxPoint{ 90, wconst::secondLineY }, checkboxValue, m_panel);
}

void unFrame::updateStartEditorImageCheckbox()
{
	const auto startEditorCheckboxPos{ m_startEditorImageCheckbox->GetPosition() };
	const EParserMask::type parserMask = m_parsedValue.has_value() ? m_parsedValue.value() : 0;
	const bool checkboxValue = isActiveBits(parserMask, EParserMask::startEditor);
	updateImageCheckbox(&m_startEditorImageCheckbox, startEditorCheckboxPos, checkboxValue, m_panel);
}

void unFrame::parsingLoop(wxTimerEvent& evnt)
{
	const wxString& telegrmLogin = m_telegrmLoginTextBox->GetValue();
	if (isParsingLoopActive(telegrmLogin, m_projectPath))
	{
		parseDataFromLog();
	}
}

void unFrame::parseDataFromLog()
{
	wxString tlgrmLogin = m_telegrmLoginTextBox->GetValue();
	const auto chatId = tlgrm::getChatId(tlgrmLogin.ToStdString());
	if (!chatId.has_value())
	{
		wxASSERT(false);
		return;
	}

	const auto& logFilePath = getPathToProjectLogFile(m_projectPath);
	wifstream file = getFileToReading(logFilePath);
	const auto recentParsedValue = parseUELog(file, EParserMask::startEditor);
	const bool IsFirstParsing = !m_parsedValue.has_value();
	if (!IsFirstParsing && isJustChangedBits(m_parsedValue.value(), recentParsedValue, EParserMask::startEditor))
	{
		tlgrm::sendMessage("Start Editor", chatId.value());
	}
	m_parsedValue = recentParsedValue;

	updateStartEditorImageCheckbox();
}


unFrame::~unFrame()
{
}

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

void unFrame::updateSubscribedEvents()
{

}
