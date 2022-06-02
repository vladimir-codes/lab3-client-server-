#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")
using namespace std;

const int MAX_CONNECTIONS = 100;
SOCKET connections[MAX_CONNECTIONS];
int Counter = 0;

void ClientHandler(int index)
{
	setlocale(LC_ALL, "rus");
	uint64_t msg_size;
	while (1) 
	{	
		if (recv(connections[index], (char*)&msg_size, sizeof(int), 0)>0) {
			char *msg = new char[msg_size + 1];
			msg[msg_size] = '\0';
			recv(connections[index], msg, msg_size, 0);
			for (int i = 0; i < Counter; i++)
			{
				if (i == index) continue;

				send(connections[i], (char*)&msg_size, sizeof(int), 0);
				send(connections[i], msg, msg_size, 0);
			}
			delete[]msg;
		}
		else 
		{
			closesocket(connections[index]);
			connections[index] = INVALID_SOCKET;
			cout << "Клиент " << index << " отключился\n";
			return;
		}
	}
}


int main()
{
	setlocale(LC_ALL, "rus");
	const unsigned int BUFF_SIZE = 10000;					// Maximum size of buffer for exchange info between server and client
	WSADATA wsData;

	int erStat = WSAStartup(MAKEWORD(2, 2), &wsData);
		
	if (erStat != 0) {
		cout << "Error WinSock version initializaion #";
		cout << WSAGetLastError();
		return 1;
	}

	SOCKET ServSock = socket(AF_INET, SOCK_STREAM, 0);

	if (ServSock == INVALID_SOCKET) {
		cout << "Error initialization socket # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else
		cout << "Server socket initialization is OK" << endl;

	in_addr ip_to_num;
	erStat = inet_pton(AF_INET, "192.168.0.102", &ip_to_num);
	if (erStat <= 0) {
		cout << "Error in IP translation to special numeric format" << endl;
		return 1;
	}

	sockaddr_in servInfo;
	ZeroMemory(&servInfo, sizeof(servInfo));

	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;
	servInfo.sin_port = htons(1234);

	erStat = bind(ServSock, (sockaddr*)&servInfo, sizeof(servInfo));
	if (erStat != 0) {
		cout << "Error Socket binding to server info. Error # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else
		cout << "Binding socket to Server info is OK" << endl;


	erStat = listen(ServSock, SOMAXCONN);
	if (erStat != 0) {
		cout << "Can't start to listen to. Error # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else {
		cout << "Listening..." << endl;
	}

	sockaddr_in clientInfo;

	ZeroMemory(&clientInfo, sizeof(clientInfo));

	int clientInfo_size = sizeof(clientInfo);

	SOCKET ClientConn;
	for (int i = 0; i < 100; i++)
	{
		ClientConn = accept(ServSock, (sockaddr*)&clientInfo, &clientInfo_size);

		if (ClientConn == INVALID_SOCKET) {
			cout << "Client detected, but can't connect to a client. Error # " << WSAGetLastError() << endl;
			closesocket(ServSock);
			closesocket(ClientConn);
			WSACleanup();
			continue;
		}
		else
			cout << "Подключился клиент "<<Counter<< endl;
			connections[i] = ClientConn;
			Counter++;
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(i), 0, 0);

	}

	closesocket(ServSock);
	WSACleanup();

	return 0;

}