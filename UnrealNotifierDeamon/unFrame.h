#pragma once

#include "wx/wx.h"
#include "ueLogParser.h"

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

	//virtual void OnOpen(wxCommandEvent& WXUNUSED(event)) override;

private:
	unApp* m_ownerApp = nullptr;

	wxTextCtrl* m_telegrmLoginTextBox = nullptr;
	wxStaticBitmap* m_telegrmCheckboxImage = nullptr;
	wxStaticBitmap* m_browseToCheckboxImage = nullptr;
	wxString m_projectPath = {};

	wxTimer* m_parsingLoopTimer = {};
	EParserMask::type m_parsedValue = 0;
	//parsingLoopStatus m_parsingLoopStatus = parsingLoopStatus::Inactive;

	void onActivateButtonClicked(wxCommandEvent& event);
	void onBrowseToClicked(wxCommandEvent& event);
	void onTelegrmLoginChanged(wxCommandEvent& event);
	void onTelegrmMessageClicked(wxCommandEvent& event);

	void parseDataFromLog(bool IsFirstParsing = false);
	void parsingLoop(wxTimerEvent& evnt);

	bool isParsingLoopActive(const wxString& telegrmName, const wxString& projectPath) const;
	void updateImageCheckbox(wxStaticBitmap** checkboxPtr, const wxPoint& pos, const bool checkboxValue);
	void updateBrowseToImageCheckbox();
	void updateTelegrmImageCheckbox();
};