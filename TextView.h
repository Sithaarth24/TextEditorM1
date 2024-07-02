#pragma once
#include <string>
#include <vector>
#include "Window.h"
#include "TextDocument.h"

LRESULT CALLBACK editWindProc(HWND, UINT, WPARAM, LPARAM);
static int cursorX = 0, cursorY = 0;

class TextView {
public:
	TextView(HWND, HINSTANCE);
	~TextView();
	HWND getHandle();
	void create_sidebar(HWND);
	void updateSidebar(int);
	LPWSTR openFile(HWND,int);
	void newFile(HWND);
	void printTxt(int);
	bool insert(TCHAR, int);
	void display(HDC);
	bool pflag = false;
private:
	HWND hedit;
	HINSTANCE wh_inst;
	HWND whwnd;
	HWND hsidebar;
	std::vector<TextDocument*> txtdoc;
};