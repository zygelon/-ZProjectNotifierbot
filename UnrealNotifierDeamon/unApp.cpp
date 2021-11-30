#include "unApp.h"
#include "unFrame.h"
#define MYICON1  101
//#include "wx/msw/wx.rc"
wxIMPLEMENT_APP(unApp);

namespace
{
	const wxString iconName = L"small.ico";
}

unApp::unApp()
{

}

bool unApp::OnInit()
{
	m_frame = new unFrame(this);
	//TODO: MB:
	wxIcon* icon = new wxIcon(iconName, wxICON_DEFAULT_TYPE);
	m_frame->SetIcon(*icon);

	m_frame->Show();
	return true;
}

unApp::~unApp()
{

}
