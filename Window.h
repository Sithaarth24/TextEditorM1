#pragma once
#include<Windows.h>
#include <commctrl.h>
#include <iostream>
#include <string>
#include "TextView.h"

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void menu(HWND);
void textBox(HWND);
void footer(HWND);
void tabwnd(HWND);


class Window
{
public:
	Window(HINSTANCE);
	bool processManager();
	
private:
	HINSTANCE	h_inst;
	HWND hwnd;
};

