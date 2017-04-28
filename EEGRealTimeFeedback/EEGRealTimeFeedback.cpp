// EEGRealTimeFeedback.cpp : Defines the entry point for the console application.
//
#pragma once
#include "stdafx.h"


#import "microEEGCtrl.tlb" no_namespace

#include "ControllerThread.h"

// Final Processing test:
#include "ControllerTest.h"


const int MicroEEGConnector::ADClevels = (int)pow(2, NBIT);


int _tmain(int argc, _TCHAR* argv[])
{
	////REAL TIME FEEDBACK////
	int Nchan = 0;
	string chans, chosenChansStr;
	char com[10];
	string theDelimiter = ",";
	vector<string> channelNames;
	vector<string> chosenChannels;
	size_t start = 0, end = 0;
	
	vector<double> LPsmooth(TW, (double)1 / TW);

	// Initialize COM interface
	ImicroEEGCtrlControllerPtr microPtr;
	
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr)) {
		cout << endl << "COM interface initialized" << endl;
	}
	else {
		cout << endl << "COM interface NOT initialized" << endl;
		//return false;
	}

	// Create microEEGCtrlController instance
	microPtr.CreateInstance("microEEGCtrl.microEEGCtrlController");
	cout << endl << "Created MicroEEGCtrl object" << endl;

	// Display measurement parameters
	microPtr->PutUDPPort(PORTuEEGctrl);
	cout << "UDP Port: " << microPtr->GetUDPPort() <<
		" Channels: " << microPtr->GetChannels() <<
		" Channels Names: " << microPtr->GetChannelNames() <<
		" status: " << microPtr->GetData() << endl;

	Nchan = microPtr->GetChannels();
	chans = microPtr->GetChannelNames();

	// Tokenize Channels string
	while (end != string::npos)
	{
		end = chans.find(theDelimiter, start);

		// If at end, use length=maxLength.  Else use length=end-start.
		channelNames.push_back(chans.substr(start,
			(end == string::npos) ? string::npos : end - start));

		// If at end, use start=maxSize.  Else use start=end+delimiter.
		start = ((end > (string::npos - theDelimiter.size()))
			? string::npos : end + theDelimiter.size());
	}


	// Start feedback if microEEG connected (Nchan > 0)
	if (Nchan > 0)
	{
		// prompt the User to insert COM port
		cout << endl << "Type COM port for USB trasmission: (COMX)" << endl;
		gets_s(com);

		// ask the user for normalization (Online Vs Load parameters)
		cout << endl << "Online Normalization (1) or Load Parameters (2)" << endl;
		int choice = 0;
		bool load;
		do {
			cin >> choice;
			if (choice != 1 && choice != 2)
				cout << "Invalid Enter" << endl;

		} while (choice != 1 && choice != 2);

		(choice == 2) ? load = true : load = false;

		// prompt the User to insert Channels to Average
		cout << endl << "Type Channels for Grand Average (comma separated)" << endl;
		cin >> chosenChansStr;
		start = end = 0;

		// tokenize chosen channels string
		while (end != string::npos)
		{
			end = chosenChansStr.find(theDelimiter, start);

			// If at end, use length=maxLength.  Else use length=end-start.
			chosenChannels.push_back(chosenChansStr.substr(start,
				(end == string::npos) ? string::npos : end - start));

			// If at end, use start=maxSize.  Else use start=end+delimiter.
			start = ((end > (string::npos - theDelimiter.size()))
				? string::npos : end + theDelimiter.size());
		}

		// Create Controller instance
		ControllerThread c(Nchan, com, channelNames, chosenChannels, LPsmooth, "BPcoeff.txt", load);

		c.Start();
	}
	else
	{
		cout << "microEEG device NOT CONNECTED!" << endl;
	}

	system("Pause");
	return 0;	

	////TEST////
	//vector<string> dataFiles = { "RawC1.txt", "RawC2.txt", "RawC3.txt", "RawC4.txt", "RawT3.txt", "RawT4.txt",
	//	"RawC1.txt", "RawC2.txt", "RawC3.txt", "RawC4.txt", "RawT3.txt", "RawT4.txt"
	//	, "RawC1.txt", "RawC2.txt", "RawC3.txt", "RawC4.txt", "RawT3.txt", "RawT4.txt"
	//	, "RawC1.txt", "RawC2.txt", "RawC3.txt", "RawC4.txt", "RawT3.txt", "RawT4.txt"
	//	/*,"RawC1.txt", "RawC2.txt", "RawC3.txt", "RawC4.txt", "RawT3.txt", "RawT4.txt"
	//	,"RawC1.txt", "RawC2.txt", "RawC3.txt", "RawC4.txt", "RawT3.txt", "RawT4.txt"
	//	, "RawC1.txt", "RawC2.txt", "RawC3.txt", "RawC4.txt", "RawT3.txt", "RawT4.txt"
	//	, "RawC1.txt", "RawC2.txt", "RawC3.txt", "RawC4.txt", "RawT3.txt", "RawT4.txt" */
	//};

	//vector<string> dataFiles = { "C3MeloHand.txt"
	//	, "C3MeloHand.txt"//, "C3MeloHand.txt", "C3MeloHand.txt", "C3MeloHand.txt"
	//	//"C3MeloHand.txt", "C3MeloHand.txt", "C3MeloHand.txt", "C3MeloHand.txt", "C3MeloHand.txt", "C3MeloHand.txt", "C3MeloHand.txt",
	//	//"C3MeloHand.txt", "C3MeloHand.txt", "C3MeloHand.txt", "C3MeloHand.txt", "C3MeloHand.txt", "C3MeloHand.txt", "C3MeloHand.txt", 
	//	//"C3MeloHand.txt", "C3MeloHand.txt", "C3MeloHand.txt", "C3MeloHand.txt"
	//};

	//ControllerTest c(dataFiles, "BPcoeff.txt", dataFiles.size());

	//c.computeBPValues();

	//c.saveResults();

	//system("Pause");
	//return 0;

}