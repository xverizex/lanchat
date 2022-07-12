#include "Network.h"

static DWORD WINAPI threadc(LPVOID param);

int NetworkChat::init()
{
	WSADATA wsaData;

	int res;

	res = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (res != NO_ERROR) {
		return -1;
	}

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (sock == INVALID_SOCKET) {
		WSACleanup();
		return -1;
	}

	int flag_on = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&flag_on, sizeof(flag_on));
	ip_mreq mreq;

	inet_pton(AF_INET, "224.1.50.15", &mreq.imr_multiaddr.S_un.S_addr);
	//inet_pton(AF_INET, "0.0.0.0", &mreq.imr_interface.S_un.S_addr);
	mreq.imr_interface.S_un.S_addr = htonl(INADDR_ANY);

	inet_pton(AF_INET, "0.0.0.0", &group.sin_addr.S_un.S_addr);
	group.sin_family = AF_INET;
	group.sin_port = htons(1234);
	
	int ret = bind(sock, (sockaddr*)&group, sizeof(group));

	inet_pton(AF_INET, "224.1.50.15", &group.sin_addr.S_un.S_addr);

	if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char *) & mreq, sizeof(mreq)) < 0) {
		return -1;
	}


	CreateThread(
		NULL,
		0,
		threadc,
		this,
		0,
		&ret_thread);

	return 0;
}

void NetworkChat::operator<< (const std::wstring &msg)
{
	size_t len = wcslen(msg.c_str());
	wchar_t* m = new wchar_t[len + 2];

	m[0] = CHAT_MSG_MSG;
	
	memcpy(&m[1], msg.c_str(), len * 2);
	
	
	int ret = sendto(sock, (const char*)m, len * 2 + 2, 0, (sockaddr*)&group, sizeof(group));
	if (ret == -1) {
		perror("sendto");
	}
}


DWORD WINAPI threadc(LPVOID param)
{
	NetworkChat* networkChat = static_cast<NetworkChat*>(param);

	WCHAR join[2];
	join[0] = CHAT_MSG_JOIN;
	join[1] = 0;
	sendto(networkChat->sock, (const char*)join, 4, 0, (sockaddr*)&networkChat->group, sizeof(networkChat->group));

	while (1) {
		sockaddr_in s;
		WCHAR b[512];

		socklen_t len_s = sizeof(s);
		int len = recvfrom(networkChat->sock, (char *) b, 512, 0, (sockaddr*)&s, &len_s);
		if (len <= 0) {
			continue;
		}

		char* a = (char*)b;
		a[len] = 0;
		a[len + 1] = 0;

		
		char who[17];
		who[16] = 0;

		InetNtopA(AF_INET, (const void*)&s.sin_addr, who, 16);
		
		switch (b[0]) {
		case CHAT_MSG_JOIN:
			networkChat->users->addUser(_strdup(who));
			break;
		case CHAT_MSG_MSG:
			networkChat->chat->fromNetwork(who, &b[1]);
			break;
		}
		
	}
}