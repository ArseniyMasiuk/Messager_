
#include "Client.h"
#include "Server.h"

/*
192.168.5.218
*/

int main()
{
	
	cout << "who are you want to be\n enter 0 if you want to be a server\n enter 1 if you want to be a client\n";

	int i = 0;
	cin >> i;
	if (i == 1) CLIENT();
	if (i == 0) SERVER();

	return 0;
}


