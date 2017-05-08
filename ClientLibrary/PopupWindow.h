#pragma once
#include "..\bootstrap\imgui_glfw3.h"
#include <string>
#include <sstream>

class PopupWindow
{
public:
	PopupWindow(std::string title, std::string message);
	~PopupWindow();

	void Draw();

	bool IsOpen();

private:
	std::string m_message;
	std::string m_title;

	std::stringstream ss;

	bool *m_open;
	static int m_windowCount;
};

