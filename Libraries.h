#pragma once
#define _CRT_SECURE_NO_WARNINGS

#pragma comment (lib,"ws2_32.lib")

#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>

#include <WS2tcpip.h>


using namespace std;


string MY_ADDRESS = "192.168.1.4";//"192.168.5.218";
#define MTU 1450//#define MTU 1460 //1452 при vpn ше менше Maximum Transmission Unit

//struct USER
//{
//
//	USER()
//	{
//			
//	}
//	USER(char * name,int inum)
//	{
//		memcpy(username,name,strlen(name));
//		identNumb = inum;
//	}
//	//int socket;
//	char username[15];
//	int identNumb;
//};

//FLAGS

#define REGISTRATION 0
#define GET_USERS_INFO 1
#define MESSAGE 2



//receivers and senders
#define SERVER_ID 1
#define TO_ALL 0



struct BUFFER
{            

	unsigned short messageSize; //in bytes
	unsigned short sender;      //like a receiver
	unsigned short receiver; 
	char flags; // 0 registration 1 request to download username ...
	char beginOfMess;

	BUFFER(){}

	BUFFER(char flag,unsigned short size, unsigned short sender, unsigned short receiver)
	{
		flags = flag;
		messageSize = size;
		this->sender = sender;
		this->receiver = receiver;
	}
	
};
