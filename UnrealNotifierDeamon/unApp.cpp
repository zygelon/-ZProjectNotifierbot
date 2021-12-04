#include "unApp.h"
#include "unFrame.h"
#define MYICON1  101
//#include "wx/msw/wx.rc"
wxIMPLEMENT_APP(unApp);

namespace
{
	const wxString iconPath = L"Resources/logo.png";
}

unApp::unApp()
{

}

bool unApp::OnInit()
{
	m_frame = new unFrame(this);
	m_icon = new wxIcon(iconPath, wxBITMAP_TYPE_PNG);
	//TODO: MB: Memory Leak
	m_frame->SetIcon(*m_icon);

	m_frame->Show();
	return true;
}

unApp::~unApp()
{

}
