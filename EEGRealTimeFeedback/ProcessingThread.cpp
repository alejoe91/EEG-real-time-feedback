/*
EEGRealTime Feedback

Alessio Buccino
*/

#pragma once
#include "stdafx.h"
#include "ProcessingThread.h"

ProcessingThread::ProcessingThread(StreamingThread& s, string filterFile, int N) : stream(s){

	loadFilter(filterFile);
	vector<double> LPcoeff(TW, (double)1 / TW);

	eeg = EEGHandler(N, BPcoeff, LPcoeff);
}

ProcessingThread::ProcessingThread(StreamingThread& s, int N, char* com, vector<string>& chanNames, vector<string>& chosenChans,
	vector<double> LPcoeff, vector<string>& FIRfilterFiles) : stream(s), feedbacksignal(0) {

	loadFilter(FIRfilterFiles);
	usb = USBController(BAUDRATE, com);
	//udp = UDPClient();
	eeg = EEGHandler(N, BPcoeff, LPcoeff);


	channelNames = chanNames;
	chosenChannels = chosenChans;

	// Map 0-255 to 1.5std - -1.5std (255 -> max movement detected)
	m = -255 / 3;
}

ProcessingThread::~ProcessingThread(){}

void ProcessingThread::Start(){
	m_thread = boost::thread(&ProcessingThread::Process, this);
}

void ProcessingThread::Join(){
	m_thread.join();
}

void ProcessingThread::Process(){
	// keep polling for tw_reached value

	while (1){
		// if time window is reached copy the data and restart streaming (set tw_Reached to false)
		if (stream.getTW_status()) {
			//clock_t t_start = clock();
			vector<vector<double>> newRaw = stream.getxxeeg();
			stream.setTW_status(false);

			// call eeg handle method to compute BP powers
			BPFeedback = eeg.handleEEGParallel(newRaw);
			//BPFeedback = eeg.handleEEG(newRaw);

			computeFeedbackValue();

			cout << "Grand Average: " << grandAverageCUR << endl;

			// send computed feedback value via USB
			if (usb.isOpen())
				usb.USBWrite((char)feedbackVal);

			// send mean values (grandAverage, c3c4, c1c2c3c4, c1c2c3c4t3t4) via UDP for real time plot
			if (udp.isConnected())
			{
				int doubleLen = sizeof(double);
				double sendVal[4] = { grandAverageCUR, c34meansignalCUR, c1234meansignalCUR, c1234t34meansignalCUR };

				char buf[4 * sizeof(double)] = {};

				for (int jj = 0; jj < 4; jj++)
					for (int ii = 0; ii < doubleLen; ii++)
					{
						char * pdouble = (char *)&sendVal[jj];
						buf[jj*doubleLen + ii] = pdouble[ii];
					}

				bool sent = false;
				sent = udp.sendData(buf, sizeof(buf));
			}

			/*clock_t now = clock();
			cout << "Processing time: " << now - t_start << " ms" << endl;*/

		}
		if (stream.getStopped())
		{
			cout << endl << "Stop Processing" << endl;
			usb.USBClose();
			udp.UDPClose();
			break;
		}
	}
}

// order: MU, BETA, ALPHA, DELTA, THETA, GAMMA
void ProcessingThread::loadFIRFilter(vector<string>& filterFiles){
	for (unsigned int i = 0; i < filterFiles.size(); i++){
		ifstream in;
		try{
			in.open(filterFiles[i]);
		}
		catch (ifstream::failure e)
		{
			cout << "Data file not found" << endl;
		}
		vector<double> currentCoeff;
		while (!in.eof()){
			double newval;
			in >> newval;
			currentCoeff.push_back(newval);
		}
		BPcoeff.push_back(currentCoeff);
		in.close();
	}
}

void ProcessingThread::SaveResults(){

	//save output signals
	ofstream out1, out2, out3, out4, out5;

	try {
		out1.open("C3C4Mean.txt");
		out2.open("C1C2C3C4Mean.txt");
		out3.open("C1C2C3C4T3T4Mean.txt");
		out4.open("GrandAverage.txt");
		out5.open("FeedbackSignal.txt");
	}
	catch (ofstream::failure e) {
		cout << "Unable to SAVE OUTPUT DATA" << endl;
		return;
	}

	for (unsigned int i = 0; i < feedbacksignal.size(); i++){
		out1 << c34meansignal[i] << endl;
		out2 << c1234meansignal[i] << endl;
		out3 << c1234t34meansignal[i] << endl;
		out4 << grandAverage[i] << endl;
		out5 << feedbacksignal[i] << endl;
	}

	out1.close();
	out2.close();
	out3.close();
	out4.close();
	out5.close();

	cout << "Output signals SAVED SUCCESSFULLY" << endl;


}

void ProcessingThread::LoadParameters(){
	eeg.loadParameters();
	// set EEGHandler flag to not estimate online parameters
	eeg.setNorm(false);
}

void ProcessingThread::SaveParameters(){
	eeg.saveParameters();
}

//PRIVATE
void ProcessingThread::computeFeedbackValue() {

	//double c34mean, c1234mean, c1234t34mean;
	int realVal = 0;
	//double grandAvVal = 0;
	int chanPresent = 0;

	//initialize output variables
	c34meansignalCUR = 0;
	c1234meansignalCUR = 0;
	c1234t34meansignalCUR = 0;
	grandAverageCUR = 0;


	// Update queues
	int itC3, itC4, itC1, itC2, itT3, itT4;
	itC3 = find(channelNames.begin(), channelNames.end(), "C3") - channelNames.begin();
	itC4 = find(channelNames.begin(), channelNames.end(), "C4") - channelNames.begin();
	itC1 = find(channelNames.begin(), channelNames.end(), "C1") - channelNames.begin();
	itC2 = find(channelNames.begin(), channelNames.end(), "C2") - channelNames.begin();
	itT3 = find(channelNames.begin(), channelNames.end(), "T3") - channelNames.begin();
	itT4 = find(channelNames.begin(), channelNames.end(), "T4") - channelNames.begin();

	
	// COMPUTE GRAND AVERAGE of cselected channels
	if (chosenChannels.size() != 0)
		for (unsigned int i = 0; i < chosenChannels.size(); i++){
			int it = find(channelNames.begin(), channelNames.end(), chosenChannels[i]) - channelNames.begin();
			if (it != channelNames.end() - channelNames.begin()){
				grandAverageCUR += BPFeedback[it - 1];
				chanPresent++;
			}
		}

	grandAverageCUR /= chanPresent;

	// C3 and C4 are present
	if (itC3 != channelNames.end() - channelNames.begin() && itC4 != channelNames.end() - channelNames.begin())
		c34meansignalCUR = (BPFeedback[itC3 - 1] + BPFeedback[itC4 - 1]) / 2;
	else
		c34meansignalCUR = 0;
	// C3 C4 C1 and C2 are present
	if (itC3 != channelNames.end() - channelNames.begin() && itC4 != channelNames.end() - channelNames.begin() &&
		itC1 != channelNames.end() - channelNames.begin() && itC2 != channelNames.end() - channelNames.begin())
		c1234meansignalCUR = (BPFeedback[itC3 - 1] + BPFeedback[itC4 - 1] + BPFeedback[itC1 - 1] + BPFeedback[itC2 - 1]) / 4;
	else
		c1234meansignalCUR = 0;
	// C3 C4 C1 C2 T3 and T4 are present
	if (itC3 != channelNames.end() - channelNames.begin() && itC4 != channelNames.end() - channelNames.begin() &&
		itC1 != channelNames.end() - channelNames.begin() && itC2 != channelNames.end() - channelNames.begin() &&
		itT3 != channelNames.end() - channelNames.begin() && itT4 != channelNames.end() - channelNames.begin())
		c1234t34meansignalCUR = (BPFeedback[itC3 - 1] + BPFeedback[itC4 - 1] + BPFeedback[itC1 - 1] + BPFeedback[itC2 - 1] +
		BPFeedback[itT3 - 1] + BPFeedback[itT4 - 1]) / 6;
	else
		c1234meansignalCUR = 0;

	////push back new value
	//c34meansignal.push_back(c34mean);
	//c1234meansignal.push_back(c1234mean);
	//c1234t34meansignal.push_back(c1234t34mean);
	//grandAverage.push_back(grandAvVal);

	realVal = (int) m*(grandAverageCUR - 1.5);

	// Saturate at 0 and 255
	if (realVal > 255)
		realVal = 255;
	if (realVal < 0)
		realVal = 0;

	feedbackVal = realVal;

	//feedbacksignal.push_back(feedbackVal);

}