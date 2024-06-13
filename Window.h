#pragma once
#include<Windows.h>

class Window
{
public:
	Window();
	bool processManager();
	
private:
	HINSTANCE	h_inst;
	HWND hwnd;
};

