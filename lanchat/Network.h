#pragma once

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>
#include <string>

#include "Chat.h"
#include "Users.h"

#pragma comment(lib,"ws2_32.lib")

#define MULTICAST_GROUP_NETWORK         "224.0.0.10"

enum MSG_TYPE {
	CHAT_MSG_JOIN,
	CHAT_MSG_MSG,
	CHAT_N_MSGS
};

class NetworkChat
{
public:
	SOCKET sock;
	int init();
	Chat* chat;
	Users* users;
	DWORD ret_thread;
	 sockaddr_in group;
	void operator<< (const std::wstring &msg);
};

