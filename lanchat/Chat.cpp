#include "Chat.h"
#include <wchar.h>
#include <vector>
#include <string.h>

#define OFFSET           4
extern HWND mainHWND;

Chat::Chat()
{
	initialize = false;
	pos_last = 0;
	max_buf = 0;
	offsetCarret = 0;
	dchat = 0;
	tchat = 1;
	count = 0;

	for (int i = 0; i < MAX_LINES; i++) {
		memset(chat[i], 0, MAX_BUFFER * 2);
	}

	thread_id = GetWindowThreadProcessId(mainHWND, NULL);
}

void Chat::setRect(RECT* r)
{
	rect.left = r->left;
	rect.right = r->right;
	rect.top = r->top;
	rect.bottom = r->bottom;
	initialize = true;
	for (int i = 0; i < MAX_BUFFER; i++) {
		buffer_input[i] = 0;
	}
	offset = 0;
	max_buf = 0;
}

void Chat::draw(HDC hdc)
{

	HPEN pen = CreatePen(PS_SOLID, 0, RGB(0x00, 0x00, 0x00));
	SelectObject(hdc, pen);

	HBRUSH brush_others = CreateSolidBrush(RGB(0x3e, 0x2e, 0x03));
	HBRUSH brush_line_text = CreateSolidBrush(RGB(0x00, 0x00, 0x00));

	SelectObject(hdc, brush_others);
	RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, 10, 10);

	SelectObject(hdc, brush_line_text);
	
	SIZE size;
	WCHAR text[] = L"TEST";
	GetTextExtentPoint32(hdc, text, wcslen(text), &size);
	int height = size.cy;

	int lineHeight = rect.bottom - height - 4;
	MoveToEx(hdc, rect.left, lineHeight - 4, NULL);
	LineTo(hdc, rect.right, lineHeight - 4);

	textInput.left = rect.left;
	textInput.right = rect.right;
	textInput.top = lineHeight;
	textInput.bottom = rect.bottom;

	SetBkColor(hdc, RGB(0x3e, 0x2e, 0x03));
	SetTextColor(hdc, RGB(0xff, 0xff, 0xff));

	int len = wcslen(&buffer_input[offset]);

	WCHAR ecmd[] = L"> ";
	len_inter_cmd = 2;
	GetTextExtentPoint32(hdc, ecmd, len_inter_cmd, &inter_cmd);

	int p = textInput.left + OFFSET + inter_cmd.cx - offsetCarret;

	int max = 0;
	for (int i = offset; buffer_input[i] != 0; i++) {
		GetTextExtentPoint32(hdc, &buffer_input[i], 1, &size);
		p += size.cx;

		if (p >= textInput.right - OFFSET) {
			break;
		}

		max++;
	}

	TextOut(hdc, textInput.left + OFFSET, textInput.top, ecmd, len_inter_cmd);
	TextOut(hdc, textInput.left + OFFSET + inter_cmd.cx, textInput.top, &buffer_input[offset], max);

	int sx = rect.left + 2;
	int ex = rect.left + 16 + 80;

	int px = ex + 4;

	for (int cur_msg = dchat; cur_msg != tchat; cur_msg++) {
		if (cur_msg == MAX_LINES) {
			cur_msg = -1;
			continue;
		}
		int shsize = 0;

		std::vector<int> poss;
		bool new_line = false;

		poss.push_back(0);

		int pos = cur_msg;

		WCHAR* msg = chat[pos];
		int top = cchat[cur_msg].top;

		for (int mx = px, i = 0; msg[i] != 0; i++) {
			SIZE size;
			GetTextExtentPoint32(hdc, &msg[i], 1, &size);

			if (new_line == false) {
				shsize += size.cy;
				new_line = true;
			}

			mx += size.cx;
			if (mx + OFFSET >= rect.right) {
				poss.push_back(i);
				mx = px;
				new_line = false;
			}
		}

		SetTextColor(hdc, RGB(0xff, 0xff, 0xff));

		len = wcslen(chat[pos]);

		if (len > 0) {


			for (int i = 0, index = 0; index < poss.size() + 1; index++) {
				int end = 0;
				if (poss.size() > index + 1) {
					end = poss[index + 1];
					i = poss[index];
				}
				else {
					if (index == poss.size()) break;
					i = poss[index];
					end = wcslen(&msg[i]);
					end += i;
				}

				int len = end - i;
				GetTextExtentPoint32(hdc, &msg[i], len, &size);

				TextOut(hdc, px, top, &msg[i], len);

				if (index + 1 == poss.size()) break;

				top += size.cy;
			}

			SIZE size_ip;

			GetTextExtentPoint32A(hdc, ip[cur_msg], strlen(ip[cur_msg]) + 1, &size_ip);
			HBRUSH brush = CreateSolidBrush(RGB(0x3e, 0x2e, 0x03));
			SelectObject(hdc, brush);

			HPEN pen = CreatePen(PS_SOLID, 0, RGB(0xd8, 0xb5, 0x15));
			SelectObject(hdc, pen);
			RoundRect(hdc, sx, cchat[cur_msg].top, ex, cchat[cur_msg].top + size_ip.cy + 4, 10, 10);

			int posx = sx + (ex - sx) / 2 - size_ip.cx / 2;
			TextOutA(hdc, posx, cchat[cur_msg].top + 2, ip[cur_msg], strlen(ip[cur_msg]));
		}
	}
	
}

void Chat::addChar(HDC hdc, WCHAR c)
{
	int i = carret;
	SIZE size;

	pos_last = offset;
	int m = textInput.left + OFFSET + inter_cmd.cx;

	for (int ii = offset; ii < max_buf; ii++) {
		SIZE size;
		GetTextExtentPoint32(hdc, &buffer_input[ii], 1, &size);
		m += size.cx;
		pos_last++;

		if (m >= carret)
		{
			break;
		}

	}

	GetTextExtentPoint32(hdc, &c, 1, &size);

	carret = m;

	i += size.cx;
	
			
	int ii = 0;
	max_buf = 0;
	for (; buffer_input[ii] != 0; ii++, max_buf++);
	
	if (max_buf + 1 >= MAX_BUFFER) {

		pos_last--;
		max_buf--;

		SIZE temps;
		GetTextExtentPoint32(hdc, &buffer_input[pos_last], 1, &temps);

		carret -= temps.cx;
		carret += size.cx;
		buffer_input[pos_last] = c;

	}
	else {
		for (; ii > pos_last; ii--) {
			buffer_input[ii] = buffer_input[ii - 1];
		}
		carret += size.cx;
		buffer_input[pos_last] = c;
	}
	
	
		
	bool cc = false;
	if (carret >= (textInput.right - OFFSET)) {
		GetTextExtentPoint32(hdc, &buffer_input[offset], 1, &size);
		offsetCarret += size.cx;
		offset++;

		int m = textInput.left + OFFSET + inter_cmd.cx;
		for (int i = offset; buffer_input[i] != 0; i++) {
			GetTextExtentPoint32(hdc, &buffer_input[i], 1, &size);

			m += size.cx;
		}

		carret = m;
	}
	if (cc == false) {
		offsetCarret = 0;
	}
	

	SetCaretPos(carret, textInput.top);
	
	max_buf++;

}

void Chat::backspace(HDC hdc)
{
	if (max_buf == 0) return;

	for (int i = textInput.left + OFFSET + inter_cmd.cx, index = offset; index < max_buf; index++) {
		SIZE size;

		GetTextExtentPoint32(hdc, &buffer_input[index], 1, &size);

		i += size.cx;
		if (i >= carret) {
			carret -= size.cx;
			SetCaretPos(carret, textInput.top);
			if ((index + 1) >= max_buf) {
				buffer_input[index] = 0;
				max_buf--;
				pos_last--;
				int m = 0;
				int mi = 0;
				if (offset) {
					offset--;
					for (m = textInput.left + OFFSET + inter_cmd.cx, index = offset; index < max_buf; index++) {
						SIZE size;

						GetTextExtentPoint32(hdc, &buffer_input[index], 1, &size);

						m += size.cx;
						mi++;
						if (mi >= pos_last + 1) break;
					}
					carret = m;
					SetCaretPos(carret, textInput.top);
				}
				break;
			}
			for (int ii = index; ii < max_buf; ii++) {
				buffer_input[ii] = buffer_input[ii + 1];
			}
			max_buf--;
			pos_last--;
			break;
		}
	}
}

void Chat::carretLeft(HDC hdc)
{
	SIZE size;
	int m = textInput.left + OFFSET + inter_cmd.cx;
	bool found = false;
	for (int i = 0; i < max_buf; i++) {
		GetTextExtentPoint32(hdc, &buffer_input[i], 1, &size);
		m += size.cx;
		if (m > carret) {
			if (i > 0) {
				GetTextExtentPoint32(hdc, &buffer_input[i - 1], 1, &size);
				carret -= size.cx;
				SetCaretPos(carret, textInput.top);
				found = true;
			}
			break;
		}
	}

	if (found == false) {
		if (max_buf > 0) {
			GetTextExtentPoint32(hdc, &buffer_input[max_buf - 1], 1, &size);
			if (carret - size.cx < textInput.left + OFFSET + inter_cmd.cx) return;
			carret -= size.cx;
			SetCaretPos(carret, textInput.top);
		}
	}
}

void Chat::carretRight(HDC hdc)
{
	SIZE size;
	int m = textInput.left + OFFSET + inter_cmd.cx;
	bool found = false;
	for (int i = 0; i < max_buf; i++) {
		GetTextExtentPoint32(hdc, &buffer_input[i], 1, &size);
		m += size.cx;
		if (m > carret) {
			carret += size.cx;
			SetCaretPos(carret, textInput.top);
			break;
		}
	}
}

#include "Network.h"
#include <string>
extern NetworkChat* networkChat;


void Chat::fromNetwork(const char* who, WCHAR* msg)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(mainHWND, &ps);

	placeChat(hdc, who, msg);

	EndPaint(mainHWND, &ps);

	for (int i = 0; i < max_buf; i++) {
		buffer_input[i] = 0;
	}
	max_buf = 0;
	pos_last = 0;
	offset = 0;
	carret = rect.left + OFFSET + inter_cmd.cx;
	SetCaretPos(carret, textInput.top);


	if (count >= 1)
		for (int i = dchat; i != tchat; i++) {
			if (i == MAX_LINES) {
				i = -1;
				continue;
			}
			int m = i + 1 >= MAX_LINES ? 0 : i + 1;
			int top0 = cchat[i].top;
			int top1 = cchat[m].size;
			top0 -= top1 + OFFSET;
			cchat[i].top = top0;
		}

	if (count < 2) count++;
	tchat++;
	if (tchat == MAX_LINES) tchat = 0;
	if (tchat == dchat) dchat++;
	if (dchat == MAX_LINES) dchat = 0;

	PostMessage(mainHWND, WM_USER, 0, 0);
	//InvalidateRect(mainHWND, &rect, FALSE);
}

void Chat::enter(HDC hdc)
{
	*networkChat << std::wstring(buffer_input);
}


void Chat::placeChat(HDC hdc, const char* who, WCHAR* msg)
{
	wcsncpy_s(chat[tchat], msg, wcslen(msg) + 1);
	strncpy_s(ip[tchat], who, strlen(who) + 1);

	int sx = rect.left + 16;
	int ex = rect.left + 16 + 80;

	int px = ex + 4;

	int shsize = 0;

	std::vector<int> poss;
	bool new_line = false;

	poss.push_back(0);

	for (int mx = px, i = 0; msg[i] != 0; i++) {
		SIZE size;
		GetTextExtentPoint32(hdc, &msg[i], 1, &size);

		if (new_line == false) {
			shsize += size.cy;
			new_line = true;
		}

		mx += size.cx;
		if (mx + OFFSET >= rect.right) {
			poss.push_back(i);
			mx = px;
			new_line = false;
		}
	}

	cchat[tchat].top = rect.bottom - (textInput.bottom - textInput.top) - shsize - 16;
	cchat[tchat].size = shsize;

}