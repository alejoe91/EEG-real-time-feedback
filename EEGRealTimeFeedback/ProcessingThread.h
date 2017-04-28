/*
EEGRealTime Feedback

Alessio Buccino
*/

#pragma once
#include "stdafx.h"
#include "StreamingThread.h"
#include "EEGHandler.h"
#include "USBController.h"
#include "UDPClient.h"

#include <boost\thread.hpp>

class ProcessingThread {

private:
	boost::thread m_thread;
	StreamingThread& stream;
	EEGHandler eeg;

	USBController usb;
	UDPClient udp;

	vector<vector<double>> BPcoeff;

	// channel names from uEEGCtrl
	vector<string> channelNames;
	// channels chosen by user to compute grandAverage
	vector<string> chosenChannels;

	// linear coefficient
	double m;

	// current time window feedback value and BPpowers
	int feedbackVal;
	vector<double> BPFeedback;

	//output signals

	double grandAverageCUR;
	double c34meansignalCUR;
	double c1234meansignalCUR;
	double c1234t34meansignalCUR;

	vector<double> grandAverage;
	vector<double> c34meansignal;
	vector<double> c1234meansignal;
	vector<double> c1234t34meansignal;
	vector<int> feedbacksignal;

	void computeFeedbackValue();
	void loadFIRFilter(vector<string>& filterFile);

public:
	// register the 2 Processing thread with Streaming thread
	ProcessingThread(StreamingThread& s, string filterFile, int N);
	ProcessingThread(StreamingThread& s, int N, char* com, vector<string>& chanNames, vector<string>& chosenChans,
		vector<double> LPcoeff, vector<string>& filterFile);
	~ProcessingThread();

	void Start();
	void Join();

	void Process();

	void SaveResults();

	void LoadParameters();
	void SaveParameters();
};