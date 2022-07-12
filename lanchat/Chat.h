#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

typedef struct Cchat {
	int top;
	int size;
};

#define MAX_LINES         24
#define MAX_BUFFER       512

class Chat
{
public:
	RECT rect;
	RECT textInput;
	WCHAR buffer_input[MAX_BUFFER + 1];
	WCHAR chat[MAX_LINES][MAX_BUFFER];
	
	char ip[MAX_LINES][17];
	Cchat cchat[MAX_LINES];
	int count;
	int dchat;
	int tchat;
	int max_buf;
	int carret;
	long offset;
	int offsetCarret;
	bool initialize;
	int pos_last;
	SIZE inter_cmd;
	int len_inter_cmd;
	DWORD thread_id;
	Chat();
	void addChar(HDC, WCHAR c);
	void backspace(HDC);
	void setRect(RECT* r);
	void draw(HDC hdc);
	void carretLeft(HDC);
	void carretRight(HDC);
	void enter(HDC);
	void placeChat(HDC, const char *who, WCHAR *msg);
	void fromNetwork(const char* who, WCHAR* msg);
};

