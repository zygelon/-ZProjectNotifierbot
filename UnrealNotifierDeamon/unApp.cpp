#include "unApp.h"

unApp::unApp()
{

}

bool unApp::onInit()
{
	m_frame = new unFrame();
	m_frame->Show();

	return true;
}

unApp::~unApp()
{

}
