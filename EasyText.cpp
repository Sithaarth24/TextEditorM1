#pragma once
#include <iostream>
#include "framework.h"
#include "EasyText.h"
#include "Window.h"

int main(void)
{
	HINSTANCE hInstance = GetModuleHandleA(NULL);
	Window new_window(hInstance);
	new_window.processManager();
}