#include "ColorEditor.h"



ColorEditor::ColorEditor()
{
	m_visible = new bool;
	*m_visible = false;
}


ColorEditor::~ColorEditor()
{
	delete m_visible;
}

void ColorEditor::Draw()
{
	if (*m_visible == true)
	{
		ImGui::Begin("Color picker");

		ImGui::ColorEdit4("Tab colour", col);
		ImGui::SetColor(ImGui::ImGuiUserCol_TabNormal, ImVec4(col[0], col[1], col[2], col[3]));
		ImGui::ColorEdit4("Selected colour", col1);
		ImGui::SetColor(ImGui::ImGuiUserCol_TabTitleTextSelected, ImVec4(col1[0], col1[1], col1[2], col1[3]));
		ImGui::ColorEdit4("Not selected colour", col2);
		ImGui::SetColor(ImGui::ImGuiUserCol_TabTitleTextNormal, ImVec4(col2[0], col2[1], col2[2], col2[3]));

		ImGui::End();
	}
}


bool * ColorEditor::GetVisiblePointer()
{
	return m_visible;
}
