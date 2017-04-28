/*
EEGRealTime Feedback

Alessio Buccino
*/
#pragma once
#include "stdafx.h"
#include "EEGHandler.h"
#include "UDPClient.h"
#include "USBController.h"

#include <fstream>



class ControllerTest {
private:
	//1 channel to test
	vector<vector<double>> eegRawData;
	vector<double> eegRawNorm;

	EEGHandler eegSeq; 
	EEGHandler eegPar;
	UDPClient udp;
	USBController usb;

	vector<Queue<double>> xxeeg;
	vector<double> BPcoeff;
	Queue<double> feedbackQueue;
	int Nchan;

	int nsamples;

	//output (features computed for every TW)
	vector<double> BPvalues;
	vector<double> BPvaluesPar;
	vector<int> feedBackvalues;

	// feedback value

public:
	ControllerTest(vector<string> dataFiles, string filterFile, int N);
	~ControllerTest();
	void loadData(vector<string> dataFile);
	void loadFilter(string filterFile);
	void computeBPValues();
	void saveResults();

};