#pragma once

#include "Libraries.h"

/*

перевіряти чи сервер відєднався від клієнта

*/
bool contin = true;
int MyID;

char userName[20];


void reading(int sock)
{
	while (contin)
	{
		fd_set Set;
		FD_ZERO(&Set);
		FD_SET(sock, &Set);
		if (select(1, &Set, 0, 0, 0))//переробити?
		{
			//char Buff[MTU];
			BUFFER * buffer = (BUFFER *)malloc(MTU);
			int RecvSize = recv(sock, (char *)buffer, MTU, 0);
			cout << "flags: " << (int)(buffer->flags) << endl;
			cout << "receiver: " << buffer->receiver << endl;
			cout << "sender: " << buffer->sender << endl;
			cout << "messageSize: " << buffer->messageSize << endl;
			cout << "recv size: " << RecvSize << endl;

			//system("pause");
			if (RecvSize != 0)
			{
				if (buffer->flags == (char)REGISTRATION)
				{
					MyID = buffer->receiver;
					strcpy((char *)&buffer->beginOfMess, userName);
					
					//memcpy((void *)&(buffer->beginOfMess), (void *)&userName, strlen(userName));
					//memcpy(Buff, &userName, strlen(userName));
					buffer->flags = REGISTRATION;
					buffer->sender = MyID;
					buffer->receiver = SERVER_ID;
					buffer->messageSize = sizeof(BUFFER) + strlen(userName) + 1;
					send(sock, (char *)buffer, buffer->messageSize, 0);
				}
				if (buffer->flags == (char)MESSAGE)
				{
					cout << "user:"<<buffer->sender<<" " << (char *)buffer->beginOfMess<< endl;
				}

			}
			if ((RecvSize == 0) && (errno != EAGAIN))
			{
				/* ЛАЖА ВИРПАВИТИ ПЯСЛЯ ФІКСУ БАГІВ!!!!!!!!

				*/
				shutdown(sock, 2);
				closesocket(sock);
				contin = false;
			}
		}
	}

}

void CLIENT()
{

	cout << "enter server IP\n";
	//cin >> MY_ADDRESS;
	cout << "enter your username\n";
	cin >> userName;

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
	

	thread th([&]() {reading(Sock); });

	
	while (contin)
	{
		if (contin)
		{
			BUFFER * buffer = (BUFFER *)malloc(MTU);
			string mess;
			cin >> mess;
			if (mess == "false") contin = false;
			else
			{
				strcpy((char *)&buffer->beginOfMess, mess.c_str());

				//memcpy((void *)&(buffer->beginOfMess), (void *)&userName, strlen(userName));
				//memcpy(Buff, &userName, strlen(userName));
				buffer->flags = MESSAGE;
				buffer->sender = MyID;
				buffer->receiver = SERVER_ID;
				buffer->messageSize = sizeof(BUFFER) + mess.size() + 1;
				send(Sock, (char *)buffer, buffer->messageSize, 0);
			}
		}
		//cout << "my mess  " << mess << "   " << mess.c_str() << endl;

	}
	cout << "sended\n";
	cout << "all is good\n";
	shutdown(Sock, 2);
	closesocket(Sock);
	WSACleanup();
	cout << "closed\n";
	th.join();
}
