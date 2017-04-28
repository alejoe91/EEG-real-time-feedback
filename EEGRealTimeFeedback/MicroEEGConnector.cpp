/*
EEGRealTime Feedback

Alessio Buccino
*/
//
#include "stdafx.h"
#include "MicroEEGConnector.h"


MicroEEGConnector::MicroEEGConnector() : sEEG(0), Nchan(0), cont(0) {
	memset(buf, '0', BUFLEN);
}

MicroEEGConnector::MicroEEGConnector(int Nchannel) : sEEG(0), Nchan(0), cont(0){

	//Initialize Socket
	initUDPSocket();

	Nchan = Nchannel;

	//create array of queues with 
	for (int i = 0; i < Nchannel; i++){
		xxeeg.push_back(Queue<double>(TW));
	}

	cont = 0;
	memset(buf, '0', BUFLEN);

	nsamples = (int)(TW - OVERLAP*TW);

	Start = clock();

}

MicroEEGConnector::MicroEEGConnector(EEGHandler* eeg, int Nchannel) : sEEG(0), Nchan(0), cont(0){

	//Initialize Socket
	initUDPSocket();

	Nchan = Nchannel;

	//create array of queues with 
	for (int i = 0; i < Nchannel; i++){
		xxeeg.push_back(Queue<double>(TW));
	}

	cont = 0;

	registerEEGHandler(eeg);
	memset(buf, '0', BUFLEN);

	nsamples = (int)(TW - OVERLAP*TW);

	Start = clock();

}

MicroEEGConnector::~MicroEEGConnector(){
}

// Initialize UDP socket trasmission
bool MicroEEGConnector::initUDPSocket(){

	int broadcast = 1;

	//Initialise winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		std::cout << "Failed. Error Code : " << WSAGetLastError() << endl;
		return false;
	}
	std::cout << endl <<"WinSock Initialised." << endl;

	//create socket for microEEG UDP connection (IPPROTO_UDP)
	//if ((sEEG = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	if ((sEEG = socket(AF_INET, SOCK_DGRAM, 0)) == SOCKET_ERROR)
	{
		std::cout << endl << "socket() Failed. Error Code : " << WSAGetLastError() << endl;
		return false;
	}

	// set broadcast option
	if (setsockopt(sEEG, SOL_SOCKET, SO_BROADCAST, (const char*)&broadcast, sizeof broadcast) == -1)
	{
		std::cout << endl << "setsockopt() Failed. Error Code : " << WSAGetLastError() << endl;
		return false;
	}

	DWORD timeout = 100000;

	// set timeout value option
	if (setsockopt(sEEG, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof timeout) == -1)
	{
		std::cout << endl << "setsockopt() Failed. Error Code : " << WSAGetLastError() << endl;
		return false;
	}

	//setup address structure for uEEG connection
	memset((char *)&si_microEEG, 0, sizeof(si_microEEG));
	si_microEEG.sin_family = AF_INET;
	si_microEEG.sin_port = htons(PORTuEEGctrl);
	si_microEEG.sin_addr.S_un.S_addr = INADDR_ANY;
	//si_microEEG.sin_addr.S_un.S_addr = inet_pton(AF_INET, IPSERVER, &si_microEEG.sin_addr);

	//Broadcast Server -> Bind client

	if (::bind(sEEG, (sockaddr *)&si_microEEG, sizeof(sockaddr)) != 0)
	{
		std::cout << endl << "bind() failed. error code : " << WSAGetLastError() << endl;
		return false;
	}

	return true;
}

void MicroEEGConnector::UDPClose() {
	// clean up
	closesocket(sEEG);
	WSACleanup();
}

//Register microEEGConnector to EEGHandler object
void MicroEEGConnector::registerEEGHandler(EEGHandler* eeg){

	//register eegHandler to eeg (parameter)
	eegHandler = eeg;

}

//Receive and decode new datagram from microEEGCtrl
bool MicroEEGConnector::receiveEEG(vector<vector<double>>& v){
	int recvBytes = 0;
	int slen = sizeof(si_microEEG);
	int Nchanrecv;
	sockaddr si_other;

	//receive from ueegctrl
	//clear the buffer by filling null, it might have previously received data
	memset(buf, '0', BUFLEN);

	//try to receive some data, this is a blocking call
	if (sEEG != INVALID_SOCKET)
		recvBytes = recvfrom(sEEG, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen);

	if (recvBytes > 0 && recvBytes != SOCKET_ERROR)
	{
		/*clock_t Actual = clock();
		cout << Actual - Start << endl;
		Start = Actual;*/
		Nchanrecv = (recvBytes - 8) / 2;

		if (Nchanrecv != Nchan)
			cout << endl << "Loss of Data!" << endl;

		// load eegdatagram struct
		datagram = reinterpret_cast<EEGDatagram*> (buf);

		EEGdecode(datagram);

		// when TW - overlap*TW samples are reached pass raw eeg to eeg handler
		if (xxeeg[0].isFull() && (nsamples == (int)(TW - OVERLAP*TW) || nsamples >= (TW - (int)(TW - OVERLAP*TW)))){
			nsamples = 0;

			//cout << "TW reached: n = " << cont << endl;
			cont++;
			/*clock_t Actual = clock();
			cout << Actual - Start << endl;
			Start = Actual;*/
			
			// Clear vector v and then fill it with new data
			v.clear();

			for (int i = 0; i < Nchan; i++){
				vector<double> vec;
				xxeeg[i].vectorize(&vec);
				v.push_back(vec);
			}

			return true;
		}
		else
			return false;
	}
	else {
		cout << endl << "recvfrom() failed. error code : " << WSAGetLastError() << endl;
		throw exception("Timeout exception");
		return false;
	}

}

int MicroEEGConnector::getCont(){
	return cont;
}


//PRIVATE

// decode datagram and fill eeg queues
void MicroEEGConnector::EEGdecode(EEGDatagram* datagram){

	double eegm = 0;
	double ADC = (ADClevels / 2 - 1);
	double eegSpan = (double)EEGSPAN;

	// new datagram received -> increase nsamples
	nsamples++;


	for (int i = 0; i < Nchan; i++) {

		//convert to int
		int value = 0;
		value = (int)datagram->values[i];

		//compute uV value
		eegm = ((double)value * eegSpan) / ADC - eegSpan;

		if (!xxeeg[i].isFull())
		{
			// enqueue new value 
			xxeeg[i].enqueue(eegm);
		}
		else //if it's full, dequeue and then enqueue
		{
			xxeeg[i].dequeue();
			xxeeg[i].enqueue(eegm);

		}

	}
}
