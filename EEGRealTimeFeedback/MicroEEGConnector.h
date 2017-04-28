/*
EEGRealTime Feedback

Alessio Buccino
*/


#pragma once
#include "stdafx.h"

#include "EEGHandler.h"


class MicroEEGConnector {
private:

	EEGHandler* eegHandler;
	struct sockaddr_in si_microEEG;
	int sEEG;
	char buf[BUFLEN];
	WSADATA wsa;
	EEGDatagram* datagram;
	vector<Queue<double>> xxeeg;
	clock_t Start;


	int Nchan;
	//overlap samples
	int nsamples;
	int cont;
	


	void EEGdecode(EEGDatagram* datagram);

public:

	static const int ADClevels;

	MicroEEGConnector();
	MicroEEGConnector(int Nchannel);
	MicroEEGConnector(EEGHandler* eeg, int Nchannel);
	~MicroEEGConnector();
	bool initUDPSocket();
	void UDPClose();
	void registerEEGHandler(EEGHandler* eeg);
	bool receiveEEG(vector<vector<double>>& v);
	int getCont();
	
};