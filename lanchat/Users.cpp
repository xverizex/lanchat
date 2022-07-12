#include "Users.h"
#include <string.h>

Users::Users()
{
	rect.left = 16;
	rect.top = 16;
	rect.right = 256;
	rect.bottom = 512;

	title.left = 16;
	title.top = 24;
	title.right = 256;
	title.bottom = title.top;

	char *user0 = _strdup("192.168.1.2");
	char *user1 = _strdup("192.168.1.3");
	char *user2 = _strdup("192.168.1.4");

#if 1
	users.push_back(user0);
	users.push_back(user1);
	users.push_back(user2);
#endif
}

void Users::DrawUsersLists(HDC hdc)
{
	HBRUSH brush_others = CreateSolidBrush(RGB(0x3e, 0x2e, 0x03));
	SelectObject(hdc, brush_others);

	RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, 10, 10);

	HBRUSH hbr = CreateSolidBrush(RGB(0x8d, 0x7c, 0x4e));
	SelectObject(hdc, hbr);


	WCHAR textTitle[] = L"LANCHAT";
	size_t len = wcslen(textTitle);

	SIZE size;
	GetTextExtentPoint32(hdc, textTitle, len, &size);
	SetBkColor(hdc, RGB(0x8d, 0x7c, 0x4e));
	SetTextColor(hdc, RGB(0xff, 0xff, 0xff));

	title.bottom += size.cy;

	FillRect(hdc, &title, hbr);

	TextOut(hdc, (256 / 2) - size.cx / 2, title.top, textTitle, len);

	SIZE size_max;
	WCHAR broadcast[] = L"255.255.255.255";
	GetTextExtentPoint32(hdc, broadcast, 15, &size_max);

	int top_users = title.bottom + 4;
	int posx = rect.right - size_max.cx;

	SetBkColor(hdc, RGB(0x3e, 0x2e, 0x03));

	int pos_line = 90;

	MoveToEx(hdc, pos_line, title.bottom, 0);
	HPEN pen = CreatePen(PS_SOLID, 0, RGB(0xff, 0xff, 0xff));
	SelectObject(hdc, pen);
	
	for (int i = 0; i < users.size(); i++) {
		int len = strlen(users[i]);

		GetTextExtentPoint32A(hdc, users[i], len, &size);

		int newy = top_users + size.cy / 2;
		LineTo(hdc, pos_line, newy);
		LineTo(hdc, posx - 4, newy);
		MoveToEx(hdc, pos_line, newy, 0);

		TextOutA(hdc, posx, top_users, users[i], len);

		top_users += size.cy + 4;
	}
}