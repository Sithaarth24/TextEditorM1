#pragma once
#include "TextDocument.h"
#include <iostream>
#include <algorithm>

TextDocument::TextDocument(HANDLE hfile) {
	size = GetFileSize(hfile, 0);
	std::cout << size << "\n";
	buffer = new char[size];
	if(!ReadFile(hfile, buffer, size, 0, 0)) std::cout<<"Error";
	init_linebuffer();
}

TextDocument::TextDocument() {
	HANDLE hfile = CreateFile(
		L"",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		0,
		OPEN_EXISTING,
		0,
		0
	);
	buffer = new char[1024];
}

int TextDocument::getLine(int i,char* line,int len) {
	char* buff = buffer + lineBuffer[i];
	int lineEnd = lineBuffer[i + 1] - lineBuffer[i];
	lineEnd = min(len, lineEnd);
	memcpy(line, buff, lineEnd);
	line[lineEnd + 1] = '\0';
	return lineEnd+1;
}

void TextDocument::init_linebuffer()
{
	int i = 0;
	int lc = 0;
	int len = std::strlen(buffer);
	std::cout << "Len:" << len << std::endl;
	lineBuffer = new int(len);
	lineBuffer[lc] = 0;
	while (i < len) {
		std::cout << buffer[i];
		if (buffer[i] == '\r') {
			lineBuffer[++lc] = i + 1;
		}
		i++;
	}
	std::cout << "\nLineCount:" << lc<<std::endl;
	for (int i = 0; i < lc; i++)
		std::cout << lineBuffer[i]<<" ";
	lineCount = lc;
}

TextDocument::~TextDocument() { delete buffer; }

int TextDocument::getLineCount()
{
	return lineCount;
}
