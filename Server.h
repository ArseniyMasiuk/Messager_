#pragma once

#include "Libraries.h"


mutex mt;
void connection(SOCKET &sock, vector<int> &clients)
{
	while (true)
	{
		//int socklen;
		//sockaddr client;
		int slavesocket = accept(sock, 0, 0);//(sockaddr*)&client, &socklen);
		if (slavesocket != INVALID_SOCKET)
			//cout << "                                                                        INVALID_SOCKET\n"; 
			//else 
		{
			cout << "                                   connected   socket = " << slavesocket << endl;
			//cout << "                                                 data = " << client.sa_data <<
			//    "                                               family = " << client.sa_family << endl;
			mt.lock();
			Sleep(400);
			clients.push_back(slavesocket);
			mt.unlock();
			slavesocket = NULL;
		}
		//cout << "connection\n";
	}

}

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


	vector<int> clients;


	thread th([&Sock, &clients]()
	{
		connection(Sock, clients);
	});

	system("pause");
	int i = 0;
	while (true)
	{
		if (clients.size())
		{
			char Buff[20] = { '0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0' };
			//cout << select(0, (fd_set*)&clients[i], 0, 0, 0) << endl;
			//system("pause");
			if (select(0, (fd_set*)&clients[i], 0, 0, 0))
			{
				//if (recv(clients[i], Buff, 4, 0) == -1) cout << "do not recv\n"; else cout << "received\n";
				recv(clients[i], Buff, 20, 0);
				if (Buff[0] != '0')
				{
					cout << "my message              " << Buff << endl;
					for (int jj = 0; jj < clients.size(); jj++)
					{
						if (jj != i)
							send(clients[jj], Buff, 20, 0);
					}
				}
				//Buff[0] = '^';
			}
			i++;
			if (i == clients.size()) i = 0;
		}

	}
	/*char Buff[5] = { '0','0','0','0' };
	system("pause");
	if (recv(slavesocket, Buff, 4, 0) == -1) cout << "do not recv\n"; else cout << "received\n";

	cout << "my message              " << Buff << endl;
	system("pause");
	shutdown(slavesocket, 2);
	while (recv(slavesocket, Buff, 1024, 0) != -1);
	closesocket(slavesocket);

	shutdown(slavesocket, 2);
	//while (recv(slavesocket, Buff, 4, 0) != -1);
	closesocket(slavesocket);
	*/

	for (int j = 0; j < clients.size(); j++)
	{
		shutdown(clients[j], 2);
		//while (recv(slavesocket, Buff, 4, 0) != -1);
		closesocket(clients[j]);
	}
	cout << "all is good\n";
	th.join();
	closesocket(Sock);
}
//int SERVER()
//{
//	inicialize winsock
//	WSADATA wsData;
//	WORD ver = MAKEWORD(2, 2);
//
//	int wsOK = WSAStartup(ver, &wsData);
//	if (wsOK != 0)
//	{
//		cout << "cant inicialize winsock\n";
//	}
//	create a sock
//	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
//	if (listening == INVALID_SOCKET)
//	{
//		cout << "cant create a socket\n";
//		return 0;
//	}
//	bind a sock to a ip addres and socket
//	sockaddr_in hint;
//	hint.sin_family = AF_INET;
//	hint.sin_port = htons(54000);
//	hint.sin_addr.S_un.S_addr = INADDR_ANY;
//
//	bind(listening, (sockaddr *)&hint, sizeof(hint));
//
//	 tell winsock the socket is for listening
//	listen(listening, SOMAXCONN);
//
//	wait for a connection
//	sockaddr_in client;
//	int clientSize = sizeof(client);
//
//	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
//	if(clientSocket = INVALID_SOCKET)
//	{
//		cout << "cant create a socket 2 \n";
//		return 0;
//	}
//	char host[NI_MAXHOST];
//	char service[NI_MAXHOST];
//	ZeroMemory(host, NI_MAXHOST);
//	ZeroMemory(service, NI_MAXHOST);
//
//	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST,service, NI_MAXSERV, 0) == 0)
//	{
//		cout << host << " connected on port " << service << endl;
//	}
//	else
//	{
//		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
//		cout << host << " connected on port" <<
//			ntohs(client.sin_port) << endl;
//	}
//	
//	close listenjng socket
//	closesocket(listening);
//	...
//}