#pragma once

#include "Libraries.h"

/*

перевіряти чи сервер відєднався від клієнта

*/
void reading(int sock)
{
	while (true)
	{
		if (select(0, (fd_set*)&sock, 0, 0, 0))
		{
			char Buff[20] = { '0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0' };
			recv(sock, Buff, 20, 0);
			if (Buff[0] != '0')
				cout << "my message              " << Buff << endl;
		}
	}

}

void CLIENT()
{

	cout << "enter your IP\n";
	cin >> MY_ADDRESS;

	WSADATA wData;
	//WORD version_request;
	//SOCKET sock, listener;

	//int socket_name_size;
	//int bytes_read;

	//char buff[1024];
	//struct sockaddr_in addr;

	if (WSAStartup(MAKEWORD(1, 1), &wData) != 0)
	{
		std::cout << "socket not initialized\n";
		//return 1;
	}
	std::cout << "socket initialized\n";


	cout << "CLIENT\n";
	int Sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	cout << "created socket\n";
	sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(50000);
	//sockaddr.sin_addr.s_addr = inet_pton(AF_INET,"127.0.0.1",&sockaddr.sin_addr);//htonl(INADDR_LOOPBACK);
	inet_pton(AF_INET, MY_ADDRESS.c_str(), &sockaddr.sin_addr.s_addr);

	DWORD nonBlocking = 1;
	if (ioctlsocket(Sock, FIONBIO, &nonBlocking) != 0)
	{
		cout << "failed to set non-blocking socket\n";
	}


	cout << connect(Sock, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) << endl;
	cout << "connected\n";

	send(Sock, "conn", 4, 0);

	thread th([&]() {reading(Sock); });

	while (true)
	{
		string mess;
		cin >> mess;

		//cout << "my mess  " << mess << "   " << mess.c_str() << endl;

		send(Sock, mess.c_str(), 20, 0);
	}
	cout << "sended\n";
	cout << "all is good\n";
	closesocket(Sock);
	cout << "closed\n";
}
