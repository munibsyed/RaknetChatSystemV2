#include "PopupWindow.h"



PopupWindow::PopupWindow(std::string title, std::string message)
{
	m_title = title;
	m_message = message;
	m_open = new bool;
	*m_open = true;
	m_windowCount++;
	ss << title << " " << m_windowCount;
}


PopupWindow::~PopupWindow()
{
	delete m_open;
}


void PopupWindow::Draw()
{
	ImGuiWindowFlags flags = 0;	

	ImGui::Begin(ss.str().c_str(), m_open, flags);
	ImGui::TextColored(ImVec4(1,0,0,1), m_message.c_str());
	ImGui::End();

	/*if (ImGui::Begin(m_title.c_str(), m_open, flags) == false)
	{
		ImGui::End();
	}
	else
	{
		ImGui::Text(m_message.c_str());
		ImGui::End();
	}*/

}

bool PopupWindow::IsOpen()
{
	return *m_open;
}

int PopupWindow::m_windowCount = 0;