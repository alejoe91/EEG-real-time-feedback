/*
EEGRealTime Feedback

Alessio Buccino
*/

#pragma once
#include "stdafx.h"

class UDPClient {
private:
	struct sockaddr_in si_server;
	int sSERVER;
	char buf[BUFLEN];
	WSADATA wsa;
	bool connected;

public:
	UDPClient();
	UDPClient(int port,const char* server, int strLen);
	~UDPClient();
	bool isConnected();
	bool sendData(const char* buf, int bufLen);

	void UDPClose();

};