
#include "Window.h"
#include <commctrl.h>

#define NEW_BUTTON 1
#define OPEN_BUTTON 2
#define EXIT_BUTTON 3
#define HELP_BUTTON 4
#define SAVE_BUTTON 5

LRESULT CALLBACK TabWinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uid, DWORD_PTR dwRefData);

TextView* txtobj;
HINSTANCE hinst;
HMENU hmenu;
HWND hfooter;
HWND htabwnd;
int lineCount = 1;
int currLine = 1;
int currTab = 0;
RECT winSize;
void openFile(HWND);



Window::Window(HINSTANCE hinstance):h_inst(hinstance)
{
	const wchar_t* CLASS_NAME = L"MainWindowClass";

	WNDCLASS wnd_class = {};
	wnd_class.hInstance = h_inst;
	wnd_class.lpszClassName = CLASS_NAME;
	wnd_class.lpfnWndProc = WindowProc;
	wnd_class.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wnd_class.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClass(&wnd_class);

	wnd_class.lpszClassName = L"editClass";
	wnd_class.lpfnWndProc = editWindProc;

	RegisterClass(&wnd_class);

	DWORD style = WS_OVERLAPPEDWINDOW;

	RECT rect;
	rect.left = 350;
	rect.right = 1150;
	rect.bottom = 700;
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
		this
	);
	ShowWindow(hwnd, SW_SHOW);
	hinst = h_inst;
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



void footer(HWND hwnd) {
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

	int parts[] = { 400,-1 };

	SendMessage(hfooter, SB_SETPARTS, sizeof(parts) / sizeof(int), (LPARAM)parts);

	SendMessage(hfooter, SB_SETTEXT, 0, (LPARAM)L"Ln 1, Col 0");
	SendMessage(hfooter, SB_SETTEXT, 1, (LPARAM)L"0 characters");
}

void menu(HWND hwnd) {
	hmenu = CreateMenu();
	HMENU hfileMenu = CreateMenu();
	
	AppendMenuW(hfileMenu, MF_STRING, NEW_BUTTON, L"New"); 
	AppendMenuW(hfileMenu, MF_STRING, OPEN_BUTTON, L"Open");
	AppendMenuW(hfileMenu, MF_STRING, SAVE_BUTTON, L"Save");
	AppendMenuW(hfileMenu, MF_DISABLED, NULL,NULL);
	AppendMenuW(hfileMenu, MF_STRING, EXIT_BUTTON, L"Exit");

	AppendMenuW(hmenu, MF_POPUP, (UINT_PTR)hfileMenu, L"File");
	AppendMenuW(hmenu, MF_STRING, HELP_BUTTON, L"Help");

	SetMenu(hwnd, hmenu);

}

void resize(HWND hwnd,RECT rect) {
	HWND hedit_t = txtobj->getHandle();
	MoveWindow(hedit_t, 20, 20, rect.right - 20, rect.bottom - 40,true);
	MoveWindow(hfooter, 0, 50, rect.right, 10,true);
	MoveWindow(htabwnd, 20, 0, rect.right - 20, 20, true);
}

void tabwnd(HWND hwnd) {
	htabwnd = CreateWindowEx(
		0,
		WC_TABCONTROL,
		NULL,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | SBT_NOBORDERS,
		20, 0, 1536, 20,
		hwnd,
		NULL,
		GetModuleHandle(NULL),
		NULL
	);
	TCITEM tie;
	tie.mask = TCIF_TEXT;
	LPWSTR tab = new wchar_t[30];
	wcscpy_s(tab, 14, L"  Untitled1  ");
	tie.pszText = tab;

	TabCtrl_InsertItem(htabwnd, currTab++, &tie);
}

void addTab() {
	TCITEM tie;
	tie.mask = TCIF_TEXT;
	LPWSTR tab = new wchar_t[30];
	wcscpy_s(tab, 14, L"  Untitled2  ");
	tie.pszText = tab;

	TabCtrl_InsertItem(htabwnd, currTab++, &tie);
}

void renameTab(LPWSTR newName,int index) {
	TCITEM tie;
	tie.mask = TCIF_TEXT;
	tie.pszText = newName;

	SendMessage(htabwnd, TCM_SETITEM, index, (LPARAM)&tie);
}



LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_KEYDOWN:
		OutputDebugString(L"KeyPressed from parent\n");
		break;
	case WM_COMMAND:
		switch (wParam) {
		case NEW_BUTTON:
			addTab();
			txtobj->newFile(hwnd);
			MessageBeep(MB_OK);
			break;
		case OPEN_BUTTON:
			txtobj->pflag = true;
			MessageBeep(MB_OK);
			int index;
			index = TabCtrl_GetCurSel(htabwnd);
			LPWSTR name;
			if(name = txtobj->openFile(hwnd,index)) renameTab(name,index);
			InvalidateRect(txtobj->getHandle(), NULL, TRUE);
			break;
		case EXIT_BUTTON:
			DestroyWindow(hwnd);
			break;
		case SAVE_BUTTON:
			break;
		}
		break;
	case WM_CREATE:
		txtobj = new TextView(hwnd, hinst);
		txtobj->newFile(hwnd);
		SetFocus(txtobj->getHandle());
		menu(hwnd);
		footer(hwnd);
		tabwnd(hwnd);
		GetClientRect(hwnd,&winSize);

		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		delete(txtobj);
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		PAINTSTRUCT ps;
		RECT rect;
		GetClientRect(hwnd, &rect);
		resize(hwnd, rect);
		HDC hdc;
		hdc = BeginPaint(hwnd, &ps);
		HBRUSH brush;
		brush = CreateSolidBrush(RGB(255, 255, 255));
		FillRect(hdc, &rect, brush);
		DeleteObject(brush);
		EndPaint(hwnd, &ps);
		break;
	case WM_SIZE:
		RECT rect1;
		GetClientRect(hwnd, &rect1);
		resize(hwnd, rect1);
		break;
	case WM_SETFOCUS:
		SetFocus(txtobj->getHandle());
		break;
	case WM_NOTIFY:
		if (((LPNMHDR)lParam)->hwndFrom == htabwnd)
		{
			switch (((LPNMHDR)lParam)->code)
			{
			case TCN_SELCHANGE:
			{
				int index = TabCtrl_GetCurSel(htabwnd);
				txtobj->printTxt(index);
				SendMessageW(txtobj->getHandle(), WM_PAINT, 0, 0);
			}
			break;
			}
		}
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}




