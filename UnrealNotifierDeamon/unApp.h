#pragma once

#include "wx/wx.h"

class unApp : public wxApp
{
public:
	unApp();
	~unApp();

	virtual bool OnInit() override;

private:
	class unFrame* m_frame = nullptr;
};