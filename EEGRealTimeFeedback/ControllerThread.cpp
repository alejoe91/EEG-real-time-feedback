/*
EEGRealTime Feedback

Alessio Buccino
*/

#pragma once
#include "stdafx.h"
#include "ControllerThread.h"

ControllerThread::ControllerThread(int N, char* com, vector<string>& chanNames, vector<string>& chosenChans,
	vector<double> LPcoeff, string filterFile) : 
	stream(StreamingThread(N)), process(stream, N, com,chanNames,chosenChans,LPcoeff,filterFile){
	stream.Start();
	process.Start();
}

ControllerThread::ControllerThread(int N, char* com, vector<string>& chanNames, vector<string>& chosenChans,
	vector<double> LPcoeff, string filterFile, bool load) :
	stream(StreamingThread(N)), process(stream, N, com, chanNames, chosenChans, LPcoeff, filterFile){
	stream.Start();
	process.Start();

	if (load)
		process.LoadParameters();
}

ControllerThread::~ControllerThread(){}

void ControllerThread::Start(){
	stream.Join();
	process.Join();


	// after the threads are terminated
	End();
	Save();
}

void ControllerThread::End() {
	cout << "STOP STREAMING and PROCESSING" << endl;
}

void ControllerThread::Save() {
	process.SaveParameters();
	//process.SaveResults();
}