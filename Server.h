#pragma once

#include "Libraries.h"
#include <set>
#include <algorithm>


#define POOL_SIZE 1024


/*

перевіряти чи клієнт відєднався від сервера
спростити обмін повідомленнями між клієнтами
ввести структуру для кращого читання даних
           здатність обмінюватися різною кількістю даних (пізніше і по можливості)
забезпечити обмін повідомленнями між конкретними користувачами @general, @username 
відєднання сервера від усіх клієнтів і завершення його функціонування
здатність сервера надсилати повідомлення будь якому клієнту


*/



// переаедення сокета в неблокуючий режим на лінуксі

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


	SOCKET Sock = socket(AF_INET, SOCK_STREAM, 0);
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
	
	set<int> Clients;


	struct pollfd Set[POOL_SIZE];
	Set[0].fd = Sock;
	Set[0].events = POLLIN;

	
	while (true)
	{
		unsigned int index = 1;

		for (auto iter = Clients.begin(); iter != Clients.end(); iter++)
		{
			Set[index].fd = *iter;
			Set[index].events = POLLIN;
			index++;
		}

		int Size = 1 + Clients.size();

		WSAPoll(Set, Size, -1);//on windows, on linux is poll(*,*,*);

		for (int i=0;i<Size;i++)
		{
			if (Set[i].revents & POLLIN)
			{
				if (i)
				{
					static char buffer[1024];
					int RecvSize = recv(Set[i].fd, buffer, sizeof(buffer), 0);
					cout << buffer << endl;

					if ((RecvSize == 0) && (errno != EAGAIN))
					{
						shutdown(Set[i].fd, 2);
						closesocket(Set[i].fd);
						 Clients.erase(Set[i].fd);

					}else if (RecvSize != 0)
					{
						for (int j = 0; j<Size; j++)
						{
							if (j != i) send(Set[j].fd, buffer, RecvSize, 0);
						}
					}
					
				}
				else
				{
					int NewConn = accept(Sock, 0, 0);

					DWORD nonBlocking = 1;
					if (ioctlsocket(Sock, FIONBIO, &nonBlocking) != 0)
					{
						cout << "failed to set non-blocking socket\n";
					}
					Clients.insert(NewConn);
				}
				
			}
		}
	}


	//завершення роботи сервера
	for (auto iter = Clients.begin(); iter != Clients.end(); iter++)
	{
		shutdown(*iter, 2);
		closesocket(*iter);
		Clients.erase(iter);
	}

	closesocket(Sock);

	WSACleanup();
}
