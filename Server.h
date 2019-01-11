#pragma once

#include "Libraries.h"
#include <set>
#include <algorithm>

/*

перевіряти чи клієнт відєднався від сервера
спростити обмін повідомленнями між клієнтами
ввести структуру для кращого читання даних
           здатність обмінюватися різною кількістю даних (пізніше і по можливості)
забезпечити обмін повідомленнями між конкретними користувачами @general, @username 
відєднання сервера від усіх клієнтів і завершення його функціонування
здатність сервера надсилати повідомлення будь якому клієнту


*/



// переведення сокета в неблокуючий режим на лінуксі

//int set_nonblock(int fd)
//{
//	int flags;
//#if defined(O_NONBLOCK)
//	if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
//		flags = 0;
//	return fcntl(fd, F_SETFL, FLAGS | O_NONBLOCK);
//#else
//	flags = 1;
//	return ioctl(fd,FIOBIO,&flags);
//#endif
//}



void SERVER()
{
	cout << "enter your IP\n";
	//cin >> MY_ADDRESS;

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


	SOCKET Sock = socket(AF_INET, SOCK_STREAM, 0);// дописати перевірку помилки
	
	cout << "socket   " << Sock << endl;
	sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(50000);
	//sockaddr.sin_addr.s_addr = htonl(INADDR_ANY); //inet_addr("127.0.0.1");//htonl(INADDR_LOOPBACK);
	inet_pton(AF_INET, MY_ADDRESS.c_str(), &sockaddr.sin_addr.s_addr);//"192.168.1.5"

	DWORD nonBlocking = 1;
	if (ioctlsocket(Sock, FIONBIO, &nonBlocking) != 0)
	{
		cout << "failed to set non-blocking socket\n";
	}

	cout << "                                      bind  " << ::bind(Sock, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) << endl;

	cout << "                                    listen  " << listen(Sock, SOMAXCONN) << endl;

	system("pause");
	
	//set<int> Clients;

	map<int, string> Clients;
	//BUFFER * buff  = (BUFFER *)malloc();     //576байт


	while (true)
	{
		fd_set Set;
		FD_ZERO(&Set);
		FD_SET(Sock, &Set);
		int maxElem;
		maxElem = 0;
		for (auto iter = Clients.begin(); iter != Clients.end(); iter++)
		{
			FD_SET(iter->first, &Set);
			maxElem = iter->first;
		}

		int Max = max(Sock, maxElem);

		select(Max + 1, &Set, NULL, NULL, NULL);

		for (auto iter = Clients.begin(); iter != Clients.end();)
		{
			if (FD_ISSET(iter->first, &Set))
			{
				static BUFFER * buffer = (BUFFER *)malloc(MTU);
				int RecvSize = recv(iter->first, (char *)buffer, MTU, 0);
				//cout << (char *)buffer->beginOfMess << endl;

				if (RecvSize != 0)
				{
					if (buffer->flags == (char)REGISTRATION && buffer->receiver == SERVER_ID)
					{
						iter->second = (char *)&buffer->beginOfMess;
						cout << "connected: " << iter->second << endl;//(char )&buffer->beginOfMess << endl;
					}

					if (buffer->flags == (char)MESSAGE && buffer->receiver == SERVER_ID)
					{
						cout << "sended to server: " << iter->second<<": "<< (char *)&buffer->beginOfMess << endl;
						/*
						
						when messsage is not in one package use function getAllPackages
							
						*/
					}

					if (buffer->flags == (char)GET_USERS_INFO && buffer->receiver == SERVER_ID)
					{
						cout << "asked users informatiom : " << iter->second << endl;
						USER * users = new USER[Clients.size()-1];
						for (auto it = Clients.begin(); it != Clients.end(); it++)
						{
							if (it != iter)
							{
								users->id = it->first;
								strcpy(users->userName,it->second.c_str());
								
							}
						}
						memcpy(&buffer->beginOfMess, users, sizeof(users));
						buffer->messageSize = sizeof(BUFFER) + (sizeof(USER)*Clients.size() - 1) + 1;
						buffer->sender = SERVER_ID;
						buffer->receiver = iter->first;
						buffer->flags = GET_USERS_INFO;
						send(iter->first, (char *)buffer, buffer->messageSize, 0);
						delete[] users;
					}

					if (buffer->flags == (char)MESSAGE && buffer->receiver == TO_ALL)
					{
						cout << iter->second << "sended to all users: " << (char *)buffer->beginOfMess << endl;
						for (auto it = Clients.begin(); it != Clients.end(); it++)
						{
							if (it != iter)
								send(it->first, (char *)buffer, RecvSize, 0);
						}
					}
				}
				if ((RecvSize == 0) && (errno != EAGAIN))
				{
					cout << "disconnected: " << iter->second << endl;
					shutdown(iter->first,2);
					closesocket(iter->first);
					iter = Clients.erase(iter);
					maxElem = 0;
					
				}
				else
				{
					iter++;
				}
				
			}
			else
			{
				iter++;
			}
		}
		if (FD_ISSET(Sock, &Set))
		{
			int NewConn = accept(Sock, 0, 0);

			DWORD nonBlocking = 1;
			if (ioctlsocket(Sock, FIONBIO, &nonBlocking) != 0)
			{
				cout << "failed to set non-blocking socket\n";
			}
			Clients.insert(pair<int,string>(NewConn,""));
			BUFFER buff((char)REGISTRATION,sizeof(BUFFER),SERVER_ID,NewConn);

			int sendsize = send(NewConn,(char *)&buff,sizeof(BUFFER),0);
			//system("pause");
		}
	}


	//завершення роботи сервера
	for (auto iter = Clients.begin(); iter != Clients.end(); iter++)
	{
		shutdown(iter->first, 2);
		closesocket(iter->first);
		Clients.erase(iter);
	}

	closesocket(Sock);

	WSACleanup();
}
