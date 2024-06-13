
#include "Window.h"
#include <commctrl.h>
#include <iostream>
#include <string>
#define NEW_BUTTON 1
#define OPEN_BUTTON 2
#define EXIT_BUTTON 3
#define HELP_BUTTON 4


LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK editSubProc(HWND, UINT, WPARAM, LPARAM);
void create_menu(HWND);
void resize(HWND, int, int);
void create_sidebar(HWND);
void textBox(HWND);
void create_footer(HWND);
void updateSidebar(int);

HMENU hmenu;
HWND hfooter;
HWND hedit;
HWND hsidebar;
WNDPROC editDefaultProc;
int lineCount = 1;


Window::Window()
    : h_inst(GetModuleHandle(nullptr))
{
	const wchar_t* CLASS_NAME = L"MainWindowClass";

	WNDCLASS wnd_class = {};
	wnd_class.hInstance = h_inst;
	wnd_class.lpszClassName = CLASS_NAME;
	wnd_class.lpfnWndProc = WindowProc;
	wnd_class.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wnd_class.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClass(&wnd_class);

	DWORD style = WS_OVERLAPPEDWINDOW;

	RECT rect;
	rect.left = 100;
	rect.right = 600;
	rect.bottom = 600;
	rect.top = 100;

	AdjustWindowRect(&rect, style, false);

	hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		L"TextEditorM1",
		style,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL,
		NULL,
		h_inst,
		NULL
	);
	ShowWindow(hwnd, SW_SHOW);
}

bool Window::processManager()
{
	MSG msg = {};
    bool b = false;
	while (GetMessage(&msg,NULL,0,0)) {

        TranslateMessage(&msg);
        DispatchMessage(&msg);
		if (msg.message == WM_QUIT) return false;
	}
	return true;
}

void textBox(HWND hwnd) {
	hedit = CreateWindowEx(
		0,
		L"EDIT",
		L"",
		WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | WS_VSCROLL | WS_HSCROLL,
		20, 0, 480, 460,
		hwnd,
		(HMENU)1,
		GetModuleHandle(NULL),
		NULL);
	editDefaultProc = (WNDPROC)SetWindowLongPtr(hedit, GWLP_WNDPROC, (LONG_PTR)editSubProc);
}

void create_footer(HWND hwnd) {
	hfooter = CreateWindowEx(
		0,
		STATUSCLASSNAME,
		L"",
		WS_VISIBLE | WS_CHILD | SBARS_SIZEGRIP,
		0, 50, 50, 10,
		hwnd,
		(HMENU)1,
		GetModuleHandle(NULL),
		NULL
	);

	if (!hfooter) {
		MessageBox(NULL, L"Status bar creation failed!", L"Error", MB_ICONERROR | MB_OK);
		return;
	}

	int parts[] = { 200,-1 };

	SendMessage(hfooter, SB_SETPARTS, sizeof(parts) / sizeof(int), (LPARAM)parts);

	SendMessage(hfooter, SB_SETTEXT, 0, (LPARAM)L"Ln 1, Col 0");
	SendMessage(hfooter, SB_SETTEXT, 1, (LPARAM)L"0 characters");
}

void create_menu(HWND hwnd) {
	hmenu = CreateMenu();
	HMENU hfileMenu = CreateMenu();
	
	AppendMenuW(hfileMenu, MF_STRING, NEW_BUTTON, L"New"); 
	AppendMenuW(hfileMenu, MF_STRING, OPEN_BUTTON, L"Open");
	AppendMenuW(hfileMenu, MF_DISABLED, NULL,NULL);
	AppendMenuW(hfileMenu, MF_STRING, EXIT_BUTTON, L"Exit");

	AppendMenuW(hmenu, MF_POPUP, (UINT_PTR)hfileMenu, L"File");
	AppendMenuW(hmenu, MF_STRING, HELP_BUTTON, L"Help");

	SetMenu(hwnd, hmenu);

}

void updateSidebar(int line) {
	wchar_t prev[1000];
	GetWindowTextW(hsidebar, prev, 1000);
	std::string num = "\n " + std::to_string(line);
	
	int utf16Size = MultiByteToWideChar(CP_UTF8, 0, num.c_str(), -1, NULL, 0);
	wchar_t* utf16Str = new wchar_t[utf16Size];
	MultiByteToWideChar(CP_UTF8, 0, num.c_str(), -1, utf16Str, utf16Size);

	// Access the const wchar_t* representation of the UTF-16 string
	const wchar_t* str = utf16Str;


	size_t len = wcslen(prev);
	wcscpy_s(prev+len, 1000 - len,str);
	SetWindowTextW(hsidebar, prev);
}

void resize(HWND hwnd, int width, int height) {

}

void create_sidebar(HWND hwnd) {
	hsidebar = CreateWindowEx(
		0,
		L"STATIC",
		L" 1",
		WS_VISIBLE | WS_CHILD | SS_LEFT,
		0, 0, 20, 460,
		hwnd,
		(HMENU)1,
		NULL,
		NULL
	);
	
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_COMMAND:
		switch (wParam) {
		case NEW_BUTTON:
			std::cout << "1" << std::endl;
			MessageBeep(MB_OK);
			break;
		case OPEN_BUTTON:
			MessageBeep(MB_OK);
			std::cout << "2" << std::endl;
			break;
		case EXIT_BUTTON:
			std::cout << "3" << std::endl;
			DestroyWindow(hwnd);
			break;
		}
		break;
	case WM_CREATE:
		create_menu(hwnd);
		textBox(hwnd);
		create_footer(hwnd);
		create_sidebar(hwnd);
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		PAINTSTRUCT ps;
		RECT rect;
		GetClientRect(hwnd, &rect);
		HDC hdc;
		hdc = BeginPaint(hwnd, &ps);
		HBRUSH brush;
		brush = CreateSolidBrush(RGB(255, 255, 255));
		FillRect(hdc, &rect, brush);
		DeleteObject(brush);
		EndPaint(hwnd, &ps);
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


LRESULT CALLBACK editSubProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) {
	switch (umsg) {
	case WM_CHAR:
		if(wparam == VK_RETURN)
			updateSidebar(++lineCount);
		break;
	
	}
	return CallWindowProc(editDefaultProc, hwnd, umsg, wparam, lparam);
}