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

	
	ip_mreq_source mreq;

	mreq.imr_multiaddr.S_un.S_addr = 0x010100e0;
	mreq.imr_sourceaddr.S_un.S_addr = 0x0701a8c0;

	mreq.imr_interface.S_un.S_addr = 0x0701a8c0;

	group.sin_addr.S_un.S_addr = 0x010100e0;
	group.sin_family = AF_INET;
	group.sin_port = htons(0);
	

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
	int ret = sendto(sock, (const char*)msg.c_str(), wcslen(msg.c_str()), 0, (sockaddr *)&group, sizeof(group));
	if (ret == -1) {
		perror("sendto");
	}
}

DWORD WINAPI threadc(LPVOID param)
{
	NetworkChat* networkChat = static_cast<NetworkChat*>(param);

	char buf[512];

	
	while (1) {
		sockaddr_in s;

		socklen_t len_s = sizeof(s);
		int len = recvfrom(networkChat->sock, buf, 512, 0, (sockaddr*)&s, &len_s);
		if (len <= 0) {
			continue;
		}

		WCHAR b[512];
		memcpy(b, buf, len);

		networkChat->chat->fromNetwork("192.168.1.14", b);
	}
}