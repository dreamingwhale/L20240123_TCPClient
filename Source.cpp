#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include <vector>
#include <set>
#include <map>

#pragma comment(lib, "ws2_32")

std::set<SOCKET> SessionList;

CRITICAL_SECTION SessionCS;

unsigned WINAPI WorkerThread(void* Arg)
{
	SOCKET ClientSocket = *((SOCKET*)Arg);
	while (true)
	{
		char Buffer[1024] = { 0, };
		int RecvLength = recv(ClientSocket, Buffer, sizeof(Buffer), 0);
		if (RecvLength <= 0)
		{
			EnterCriticalSection(&SessionCS);
			SessionList.erase(ClientSocket);
			LeaveCriticalSection(&SessionCS);
			closesocket(ClientSocket);
			//ExitThread(-1);
			break;
		}
		else
		{
			EnterCriticalSection(&SessionCS);
			for (auto ConenctSocket : SessionList)
			{
				int SendLength = send(ConenctSocket, Buffer, RecvLength, 0);
			}
			LeaveCriticalSection(&SessionCS);
		}
	}
	return 0;
}

int main()
{
	InitializeCriticalSection(&SessionCS);


	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ListenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN ListenSockAddr = { 0 , };
	ListenSockAddr.sin_family = AF_INET;
	ListenSockAddr.sin_addr.s_addr = INADDR_ANY;// inet_addr("127.0.0.1");
	ListenSockAddr.sin_port = htons(22222);

	bind(ListenSocket, (SOCKADDR*)&ListenSockAddr, sizeof(ListenSockAddr));

	listen(ListenSocket, 5);

	while (true)
	{
		SOCKADDR_IN ClientSockAddr = { 0 , };
		int ClientSockAddrSize = sizeof(ClientSockAddr);
		SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR*)&ClientSockAddr, &ClientSockAddrSize);
		//thread ½ÇÇà, worker thread
		//CreateThread();
		HANDLE ThreadHandle = (HANDLE)_beginthreadex(0, 0, WorkerThread, (void*)&ClientSocket, 0, 0);
		//TerminateThread(ThreadHandle, -1);
		EnterCriticalSection(&SessionCS);
		SessionList.insert(ClientSocket);
		LeaveCriticalSection(&SessionCS);
	}

	closesocket(ListenSocket);

	WSACleanup();

	DeleteCriticalSection(&SessionCS);

	return 0;
}