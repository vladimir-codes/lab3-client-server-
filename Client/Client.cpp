#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <signal.h>

#pragma comment(lib, "Ws2_32.lib")
using namespace std;
SOCKET ClientSock;


void ctrl_c_handler(int signum)
{
	printf("\n ctrl+z \n");
}

void ClientHandler() 
{
	int msg_size;
	while (1) 
	{
		recv(ClientSock, (char*)&msg_size, sizeof(int), 0);
		char* msg = new char[msg_size + 1];
		msg[msg_size] = '\0';
		recv(ClientSock, msg, msg_size, 0);
		cout << msg << endl;
		delete[]msg;
	}
}

int main()
{
	setlocale(LC_ALL, "rus");
	WSADATA wsData;
	int erStat = WSAStartup(MAKEWORD(2, 2), &wsData);

	if (erStat != 0) {
		cout << "Error WinSock version initializaion #";
		cout << WSAGetLastError();
		return 1;
	}

	ClientSock = socket(AF_INET, SOCK_STREAM, 0);

	if (ClientSock == INVALID_SOCKET) {
		cout << "Error initialization socket # " << WSAGetLastError() << endl;
		closesocket(ClientSock);
		WSACleanup();
		return 1;
	}
	else
		cout << "Server socket initialization is OK" << endl;

	string serverip;
	cout << "Введите ip сервера\n";
	cin >> serverip;

	in_addr ip_to_num;

	while (inet_pton(AF_INET, serverip.c_str(), &ip_to_num) <= 0) {
		cout << "Ошибка преобразования ip адреса, попробуйте ввести еще раз" << endl;
		cin >> serverip;
	}

	sockaddr_in servInfo;

	ZeroMemory(&servInfo, sizeof(servInfo));

	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;	  // Server's IPv4 after inet_pton() function
	servInfo.sin_port = htons(1234);

	erStat = connect(ClientSock, (sockaddr*)&servInfo, sizeof(servInfo));

	if (erStat != 0) {
		cout << "Connection to Server is FAILED. Error # " << WSAGetLastError() << endl;
		closesocket(ClientSock);
		WSACleanup();
		return 1;
	}
	else
	
	cout << "Connection established SUCCESSFULLY. Ready to send a message to Server"<< endl;

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ClientHandler, 0, 0, 0);
	string msgl;
	while (1) 
	{
		getline(cin, msgl);

		if (signal(SIGINT, ctrl_c_handler) == SIG_ERR)
			return 1;

		int msg_size = msgl.size();
		send(ClientSock, (char*)&msg_size, sizeof(int), 0);
		send(ClientSock, msgl.c_str(), msg_size, 0);
		Sleep(10);
	}


	closesocket(ClientSock);
	WSACleanup();

	return 0;
}
