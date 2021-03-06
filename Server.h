#pragma once

#include "Libraries.h"
#include <set>
#include <algorithm>

/*

��������� �� �볺�� �������� �� �������
��������� ���� ������������� �� �볺�����
������ ��������� ��� ������� ������� �����
           ��������� ����������� ����� ������� ����� (������ � �� ���������)
����������� ���� ������������� �� ����������� ������������� @general, @username 
�������� ������� �� ��� �볺��� � ���������� ���� ��������������
��������� ������� ��������� ����������� ���� ����� �볺���


*/



// ����������� ������ � ����������� ����� �� �����

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

	while (true)
	{
		fd_set Set;
		FD_ZERO(&Set);
		FD_SET(Sock, &Set);
		int maxElem = 0;
		for (auto iter = Clients.begin(); iter != Clients.end(); iter++)
		{
			FD_SET(*iter, &Set);
			maxElem = *iter;
		}

		int Max = max(Sock, maxElem);

		select(Max + 1, &Set, NULL, NULL, NULL);

		for (auto iter = Clients.begin(); iter != Clients.end(); iter++)
		{
			if (FD_ISSET(*iter, &Set))
			{
				static char buffer[1024];
				int RecvSize = recv(*iter, buffer, sizeof(buffer), 0);
				cout << buffer << endl;
				if ((RecvSize == 0) && (errno != EAGAIN))
				{
					shutdown(*iter,2);
					closesocket(*iter);
					Clients.erase(iter);
				}
				else if (RecvSize!=0)
				{
					send(*iter, buffer, RecvSize, 0);
				}
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
			Clients.insert(NewConn);
		}
	}





	//���������� ������ �������
	for (auto iter = Clients.begin(); iter != Clients.end(); iter++)
	{
		shutdown(*iter, 2);
		closesocket(*iter);
		Clients.erase(iter);
	}

	closesocket(Sock);
}
