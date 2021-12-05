#pragma once

#include "wx/wx.h"
#include "ueLogParser.h"
#include <optional>

class unApp;

enum class parsingLoopStatus
{
	AllowedActive,
	Inactive,
};

class unFrame : public wxFrame
{
public:
	unFrame(unApp* inOwnerApp);
	~unFrame();

private:
	unApp* m_ownerApp = nullptr;

	wxPanel* m_panel = nullptr;
	wxTextCtrl* m_telegrmLoginTextBox = nullptr;
	wxStaticBitmap* m_telegrmCheckboxImage = nullptr;
	wxStaticBitmap* m_browseToCheckboxImage = nullptr;
	wxString m_projectPath = {};

	wxTimer m_parsingLoopTimer = {};
	std::optional<EParserMask::type> m_parsedValue = {};

	void onBrowseToClicked(wxCommandEvent& event);
	void onTelegrmLoginChanged(wxCommandEvent& event);

	void parseDataFromLog();
	void parsingLoop(wxTimerEvent& evnt);

	bool isParsingLoopActive(const wxString& telegrmName, const wxString& projectPath) const;
	void updateImageCheckbox(wxStaticBitmap** checkboxPtr, const wxPoint& pos, const bool checkboxValue);
	void updateBrowseToImageCheckbox();
	void updateTelegrmImageCheckbox();
};