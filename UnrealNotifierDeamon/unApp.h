#pragma once

#include "wx/wx.h"
#include "unFrame.h"

class unApp : public wxApp
{
public:
	unApp();
	~unApp();

	virtual bool onInit();

private:
	unFrame* m_frame = nullptr;
};