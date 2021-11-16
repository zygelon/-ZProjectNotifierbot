#include "unApp.h"
#include "unFrame.h"

wxIMPLEMENT_APP(unApp);

unApp::unApp()
{

}

bool unApp::OnInit()
{
	m_frame = new unFrame(this);
	m_frame->Show();

	return true;
}

void unApp::SetRootPath(const wxString& InPath)
{
	m_rootProjectPath = InPath;
}

unApp::~unApp()
{

}
