/*
EEGRealTime Feedback

Alessio Buccino
*/

#pragma once
#include "stdafx.h"
#include "ProcessingThread.h"

class ControllerThread {
private:
	ProcessingThread process;
	StreamingThread stream;

public:
	ControllerThread(int N, char* com, vector<string>& chanNames, vector<string>& chosenChans,
		vector<double> LPcoeff, string filterFile);
	ControllerThread(int N, char* com, vector<string>& chanNames, vector<string>& chosenChans,
		vector<double> LPcoeff, string filterFile, bool norm);
	~ControllerThread();

	void Start();
	void End();
	void Save();

};