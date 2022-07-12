#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <vector>

class Users
{
public:
	RECT rect;
	RECT title;
	bool one;
	std::vector<char *>users;
	Users();
	void DrawUsersLists(HDC hdc);
	void addUser(char* who);
};

