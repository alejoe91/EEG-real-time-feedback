/*
EEGRealTime Feedback

Alessio Buccino
*/

#include "stdafx.h"
#include "UDPClient.h"

UDPClient::UDPClient() {

	char server[] = IPSERVER;
	int port = 0, slen = sizeof(si_server);

	//Initialise winsock
	cout << endl << "Initialising Winsock..." << endl;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cout << "Failed. Error Code : " << WSAGetLastError() << endl;
		connected = false;
		return;
	}
	cout << "Initialised." << endl;

	//create socket for microEEG UDP connection (IPPROTO_UDP)
	if ((sSERVER = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		cout << "socket() Failed. Error Code : " << WSAGetLastError() << endl;
		connected = false;
		return;
	}

	//// prompt for IP address SERVER
	//cout << "Enter Server IPv4 : " << endl;
	//gets_s(server);
	////// prompt for port
	////cout << "Enter port : " << endl;
	////cin >> port;

	// setup address structure for uEEG connection
	memset((char *)&si_server, 0, sizeof(si_server));
	si_server.sin_family = AF_INET;
	si_server.sin_port = htons(UDPPORT);
	si_server.sin_addr.S_un.S_addr = inet_addr(server);
	//si_server.sin_addr.S_un.S_addr = inet_pton(AF_INET, server, &si_server.sin_addr);

	connected = true;
}

UDPClient::UDPClient(int port, const char* server, int strLen) {

	char* serverStr = new char[strLen];
	memcpy(serverStr, server, strLen);

	//Initialise winsock
	cout << endl << "Initialising Winsock..." << endl;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cout << "Failed. Error Code : " << WSAGetLastError() << endl;
		connected = false;
		return;
	}
	cout << "Initialised." << endl;

	//create socket for microEEG UDP connection (IPPROTO_UDP)
	if ((sSERVER = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		cout << "socket() Failed. Error Code : " << WSAGetLastError() << endl;
		connected = false;
		return;
	}


	//setup address structure for uEEG connection
	memset((char *)&si_server, 0, sizeof(si_server));
	si_server.sin_family = AF_INET;
	si_server.sin_port = htons(port);
	si_server.sin_addr.S_un.S_addr = inet_addr((const char*) server);

	connected = true;
}

UDPClient::~UDPClient() {
	// cleanup
	closesocket(sSERVER);
	WSACleanup();
}

bool UDPClient::isConnected(){
	return connected;
}

bool UDPClient::sendData(const char* buf, int bufLen){
	int sent = 0;
	sent = sendto(sSERVER, buf, bufLen, 0, (struct sockaddr *) &si_server, sizeof(si_server));
	if (sent < 0) 
	{
		cout << "send() Failed. Error Code : " << WSAGetLastError() << endl;
		return false;
	}
	else {
		//cout << "Bytes sent: " << sent << endl;
		return true;
	}

}

void UDPClient::UDPClose() {
	// clean up
	closesocket(sSERVER);
	WSACleanup();
}