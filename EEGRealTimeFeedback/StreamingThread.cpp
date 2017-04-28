/*
EEGRealTime Feedback

Alessio Buccino
*/
//

#pragma once

#include "stdafx.h"
#include "StreamingThread.h"


StreamingThread::StreamingThread(int Nchannels){
	nsamples = (int)(TW - OVERLAP*TW);

	uEEG = MicroEEGConnector(Nchannels);
	tw_reached = false;
	stopped = false;

}

StreamingThread::~StreamingThread(){}

void StreamingThread::Start(){
	m_thread = boost::thread(&StreamingThread::Stream, this);
}

void StreamingThread::Join(){
	m_thread.join();
}

void StreamingThread::Stream(){
	// Infinite loop for streaming eeg data from uEEGCtrl
	// When time window is reached: tw_reached = true. Stop streaming untile Processing thread reset tw_reached to false
	while (1){
		//t_start = clock();
		if (!tw_reached) {

			try {
				tw_reached = uEEG.receiveEEG(eegRaw);
				if (tw_reached){
					/*clock_t now = clock();
					cout << "Time Window " << uEEG.getCont() << " after " << now - t_start << " ms" <<endl;
					t_start = now;*/
				}
			}
			catch (exception e) {
				std::cout << e.what() << endl;
				break;
			}

			}

			if (_kbhit())
			{
				stopped = true;
				cout << endl << "Stop Streaming" << endl;
				break;
			}
		}

	closeConnection();
	stopped = true;
}


bool StreamingThread::getTW_status(){
	return tw_reached;
}

void StreamingThread::setTW_status(bool status){
	tw_reached = status;
}

bool StreamingThread::getStopped(){
	return stopped;
}

vector<vector<double>>& StreamingThread::getxxeeg(){
	return eegRaw;
}

void StreamingThread::closeConnection(){
	uEEG.UDPClose();
}