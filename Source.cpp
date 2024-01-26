#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include <iostream>

#pragma comment(lib, "ws2_32")

SOCKET ServerSocket;

unsigned WINAPI SendThread(void* Arg)
{
	char Message[1024] = { 0, };

	while (true)
	{
		std::cin.getline(Message, sizeof(Message));

		int SendLength = send(ServerSocket, Message, (int)strlen(Message), 0);
	}


	return 0;
}

unsigned WINAPI RecvThread(void* Arg)
{
	char Buffer[1024] = { 0, };

	while (true)
	{
		int RecvLength = recv(ServerSocket, Buffer, sizeof(Buffer), 0);

		std::cout << Buffer << std::endl;
	}

	return 0;
}

int main()
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	ServerSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN ServerSockAddr = { 0 , };
	ServerSockAddr.sin_family = AF_INET;
	ServerSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	ServerSockAddr.sin_port = htons(22222);

	connect(ServerSocket, (SOCKADDR*)&ServerSockAddr, sizeof(ServerSockAddr));

	HANDLE ThreadHandles[2];

	ThreadHandles[0] = (HANDLE)_beginthreadex(0, 0, SendThread, 0, 0, 0);
	ThreadHandles[1] = (HANDLE)_beginthreadex(0, 0, RecvThread, 0, 0, 0);

	while (true)
	{
		Sleep(1);
	}

	closesocket(ServerSocket);

	WSACleanup();

	return 0;
}