#pragma once
#include <Windows.h>
#include <Commdlg.h>
#include <tchar.h>

//code taken from https://msdn.microsoft.com/en-us/library/windows/desktop/ms646927(v=vs.85).aspx

class OpenFileDialog
{
public:
	OpenFileDialog();

	~OpenFileDialog();

	TCHAR* DefaultExtension;
	TCHAR* FileName;
	TCHAR* Filter;
	int FilterIndex;
	int Flags;
	TCHAR*InitialDir;
	HWND Owner;
	TCHAR* Title;

	bool ShowDialog();
};