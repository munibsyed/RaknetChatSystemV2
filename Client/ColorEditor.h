#pragma once
#include "..\bootstrap\imgui_glfw3.h"
#include "imgui_tabs.h"

class ColorEditor
{
public:
	ColorEditor();
	~ColorEditor();

	void Draw();

	bool* GetVisiblePointer();

private:
	bool *m_visible;

	//float col[4] = { 0.65f, 0.65f, 0.68f, 1.00f }; //greyish
	float col[4] = { 0.650f, 0.650f, 1.0f, 1.0f };
	float col1[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float col2[4] = { 0.0f, 0.0f, 0.0f, 0.5f };
};

