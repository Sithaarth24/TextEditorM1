#include <iostream>
#include "framework.h"
#include "TextEditorM1.h"
#include "Window.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hpins, 
					  _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) 
{

	Window* new_window = new Window();

	new_window->processManager();
	delete new_window;
}