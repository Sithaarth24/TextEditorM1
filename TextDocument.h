#pragma once
#include <Windows.h>
#include <string>


class TextDocument {
public:
	TextDocument();
	TextDocument(HANDLE);
	~TextDocument();
	int getLineCount();
	int getLine(int,char*,int);
private:
	void init_linebuffer();
	char* buffer;
	int* lineBuffer;
	int lineCount;
	int size;
};
