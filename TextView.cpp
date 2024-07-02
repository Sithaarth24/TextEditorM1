
#include "TextView.h"


TextView* p_view;
HDC hdc;
PAINTSTRUCT ps;
TCHAR ch;
int nCharWidth;
TEXTMETRIC tm;
int dwCharX;
int dwCharY;
int currIndex;

TextView::TextView(HWND hwnd, HINSTANCE hinst) {
	p_view = this;
	whwnd = hwnd;
	wh_inst = hinst;

	hedit = CreateWindowEx(
		0,
		L"editClass",
		L"",
		WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | WS_VSCROLL | WS_HSCROLL,
		20, 20, 780, 540,
		hwnd,
		(HMENU)1,
		GetModuleHandle(NULL),
		NULL);
	SetParent(hedit, hwnd);
	create_sidebar(whwnd);
	
}

TextView::~TextView() { 
	int n = txtdoc.size();
	for (int i = 0; i < n; i++)
		delete txtdoc[i];
}




void TextView::updateSidebar(int line) {
	wchar_t prev[1000];
	GetWindowTextW(hsidebar, prev, 1000);
	std::string num = "\n " + std::to_string(line);

	int utf16Size = MultiByteToWideChar(CP_UTF8, 0, num.c_str(), -1, NULL, 0);
	wchar_t* utf16Str = new wchar_t[utf16Size];
	MultiByteToWideChar(CP_UTF8, 0, num.c_str(), -1, utf16Str, utf16Size);

	// Access the const wchar_t* representation of the UTF-16 string
	const wchar_t* str = utf16Str;


	size_t len = wcslen(prev);
	wcscpy_s(prev + len, 1000 - len, str);
	SetWindowTextW(hsidebar, prev);
}

void TextView::create_sidebar(HWND hwnd) {
	hsidebar = CreateWindowEx(
		0,
		L"STATIC",
		L"",
		WS_VISIBLE | WS_CHILD | SS_LEFT,
		0, 0, 20, 753,
		hwnd,
		(HMENU)1,
		NULL,
		NULL
	);

}

HWND TextView::getHandle() { return hedit; }

void TextView::display(HDC hdc) {
	std::cout << "Lines:\n";
	int index = TabCtrl_GetCurSel(hedit);
	int l = txtdoc[index]->getLineCount();
	std::cout<<l<<" "<<index<<std::endl;
	char* line = new char(1024);
	for (int i = 0; i < l-1; i++) {
		int len = txtdoc[index]->getLine(i,line,1024);
		std::cout << line;
		TextOutA(hdc, cursorX, cursorY*dwCharY, line, len);
		cursorY += 1;
	}
}

bool TextView::insert(TCHAR ch, int index) {
	return false;
}



LRESULT CALLBACK editWindProc(HWND hedit, UINT umsg, WPARAM wparam, LPARAM lparam) {
	switch (umsg) {
	case WM_CHAR:
		currIndex = TabCtrl_GetCurSel(hedit);
		HideCaret(hedit);
		ch = (TCHAR)wparam;
		hdc = GetDC(hedit);
		GetCharWidth32(hdc, (UINT)wparam, (UINT)wparam,
			&nCharWidth);
		p_view->insert(ch, currIndex);
		TextOut(hdc, cursorX, cursorY*dwCharY, &ch, 1);
		cursorX += nCharWidth;
		ReleaseDC(hedit, hdc);
		ShowCaret(hedit);
		SetCaretPos(cursorX, cursorY*dwCharY);
		
		break;
	case WM_CREATE:
		hdc = GetDC(hedit);
		GetTextMetrics(hdc, &tm);
		ReleaseDC(hedit, hdc); 

		dwCharX = tm.tmAveCharWidth;
		dwCharY = tm.tmHeight;
		CreateCaret(hedit, (HBITMAP) 1, 0, dwCharY);
		ShowCaret(hedit);
		SetCaretPos(cursorX, cursorY);
		break;
	case WM_LBUTTONDOWN:
		CreateCaret(hedit, NULL, 1, 16);
		ShowCaret(hedit);
		SetCaretPos(cursorX, cursorY);
		break;

	case WM_SETFOCUS:
		CreateCaret(hedit, NULL, 1, 16);
		ShowCaret(hedit);
		SetCaretPos(cursorX, cursorY);
		break;
	case WM_KILLFOCUS:
		DestroyCaret();
		break;
	case WM_PAINT:
		hdc = BeginPaint(hedit, &ps);
		ShowCaret(hedit);
		if (p_view->pflag) {
			p_view->display(hdc);
			p_view->pflag = false;
		}
		EndPaint(hedit, &ps);
		break;

	}

	return DefWindowProc(hedit, umsg, wparam, lparam);
}

void TextView::printTxt(int i) {
}



void TextView::newFile(HWND hwnd) {
	txtdoc.push_back(new TextDocument());
	std::cout << "\n";
	for (auto& i : txtdoc)
		std::cout << i << " ";
	std::cout << "\n";
	
}

LPWSTR TextView::openFile(HWND hwnd, int index) {
	OPENFILENAME ofn;
	HANDLE hfile;
	const int MAX_TITLE_SIZE = 64;

	// Dynamically allocate memory for the file name
	LPWSTR fileName = new wchar_t[MAX_TITLE_SIZE];
	ZeroMemory(fileName, MAX_TITLE_SIZE * sizeof(wchar_t));

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_TITLE_SIZE;
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = L"All Files\0*.*\0Text Files\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn)) {
		hfile = CreateFile(
			ofn.lpstrFile,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ,
			0,
			OPEN_EXISTING,
			0,
			0
		);
		if (hfile == INVALID_HANDLE_VALUE) {
			MessageBox(hwnd, L"Failed to open file", L"Error", MB_OK);
			delete[] fileName;
			return nullptr;
		}
		TextDocument* temp = txtdoc[index];
		txtdoc[index] = new TextDocument(hfile);
		delete temp;
		return fileName;
	}
	else {
		MessageBeep(MB_ICONHAND);
		MessageBox(hwnd, L"Cannot Open The File", L"Error", MB_OK);
		delete[] fileName;
		return nullptr;
	}
}