#pragma once

#include "wx/wx.h"

class unApp : public wxApp
{
public:
	unApp();
	~unApp();

	virtual bool OnInit() override;
	void SetRootPath(const wxString& inPath);

private:
	class unFrame* m_frame = nullptr;
	wxString m_rootProjectPath = {};
};