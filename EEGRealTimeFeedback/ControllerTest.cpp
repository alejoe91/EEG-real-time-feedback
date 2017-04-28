/*
EEGRealTime Feedback

Alessio Buccino
*/
#pragma once
#include "stdafx.h"

#include "ControllerTest.h"

ControllerTest::ControllerTest(vector<string> dataFiles, string filterFile, int N){

	for (int i = 0; i < N; i++){
		xxeeg.push_back(Queue<double>(TW));
	}

	//xxeeg = Queue<double>(TW);
	feedbackQueue = Queue<double>(TW);
	Nchan = N;

	loadData(dataFiles);
	loadFilter(filterFile);

	vector<double> LPcoeff(TW, (double)1 / TW);

	eegSeq = EEGHandler(Nchan, BPcoeff, LPcoeff);
	eegPar = EEGHandler(Nchan, BPcoeff, LPcoeff);
	//udp = UDPClient(UDPPORT,IPSERVER,strlen(IPSERVER));
	usb = USBController(9600, "COM1");

	nsamples = (int)(TW - OVERLAP*TW);

}

ControllerTest::~ControllerTest(){}

void ControllerTest::loadData(vector<string> dataFiles){
	ifstream in;
	for (unsigned int i = 0; i < dataFiles.size(); i++){
		try{
			in.open(dataFiles[i], ios::in);
			vector<double> v;
			eegRawData.push_back(v);
		}
		catch (ifstream::failure e)
		{
			cout << "Data file not found" << endl;
			return;
		}

		while (!in.eof()){
			double newval;
			in >> newval;
			eegRawData[i].push_back(newval);
		}
		in.close();
	}
}

void ControllerTest::loadFilter(string filterFile){
	ifstream in;
	try{
		in.open(filterFile);
	}
	catch (ifstream::failure e)
	{
		cout << "Data file not found" << endl;
		return;
	}

	while (!in.eof()){
		double newval;
		in >> newval;
		BPcoeff.push_back(newval);
	}
	in.close();
}

void ControllerTest::computeBPValues(){

	double normValue = 0;
	double m = 0;
	vector<double> BPfeed, BPfeedPar;

	for (unsigned int ii = 0; ii < eegRawData[0].size(); ii++){

		for (int j = 0; j < Nchan; j++){
			//////// Update parameters, normalize and clip noise /////////

			// Enqueue eeg data in the queue
			if (!xxeeg[j].isFull())
			{
				// enqueue new value (after going to Volts)
				xxeeg[j].enqueue(eegRawData[j][ii]);
			}
			else //if it's full, dequeue and then enqueue
			{
				xxeeg[j].dequeue();
				xxeeg[j].enqueue(eegRawData[j][ii]);

				if (j==Nchan-1)
					nsamples++;
			}



			// when TW-overlap*TW samples are reached pass raw eeg to eeg handler
			if (xxeeg[Nchan - 1].isFull() && nsamples == (int)(TW - OVERLAP*TW)){
				// Process eegRawData vector only once in the for cycle over channels
				if (j == Nchan - 1) {
					nsamples = 0;


					cout << (double)ii / eegRawData[0].size() * 100 << " %" << endl;

					vector<vector<double>> v;

					// vec contains the eegData of one time window
					for (int i = 0; i < Nchan; i++){
						vector<double> vec;
						xxeeg[i].vectorize(&vec);
						v.push_back(vec);
					}

					//BPfeed = eegSeq.handleEEG(v);
					BPfeedPar = eegPar.handleEEGParallel(v);

					//cout << "SEQ: " << BPfeed[0] << "   ";
					cout << "PAR: " << BPfeedPar[0] << endl;

					// push new value in BPvalues
					//BPvalues.push_back(BPfeed[0]);
					BPvaluesPar.push_back(BPfeedPar[0]);

					//if (feedbackQueue.isFull()){
					//	//Update feedback queue with new value
					//	feedbackQueue.dequeue();
					//	feedbackQueue.enqueue(BPfeed[0]);
					//}
					//else //if it is not full yet
					//	feedbackQueue.enqueue(BPfeed[0]);
/*
					double sendBP = BPfeed[0];*/

					// Map 0-255 to 1.5std - -1.5std (255 -> max movement detected)
					m = -255 / 3;

					int realVal = 0;
					realVal = (int)m*(BPfeedPar[0] - 1.5);

					// Saturate at 0 and 255
					if (realVal > 255)
						realVal = 255;
					if (realVal < 0)
						realVal = 0;

					feedBackvalues.push_back(realVal);

					//// send computed feedback value via USB
					//if (usb.isOpen())
					//	usb.USBWrite((char)realVal);

					if (udp.isConnected()){
						int doubleLen = sizeof(double);
						double sendVal[4] = { BPfeedPar[0], BPfeedPar[0] + 0.25, BPfeedPar[0] + 0.5, BPfeedPar[0] - 1 };

						char buf[4 * sizeof(double)] = {};

						for (int jj = 0; jj < 4; jj++)
							for (int ii = 0; ii < doubleLen; ii++)
							{
								char * pdouble = (char *)&sendVal[jj];
								buf[jj*doubleLen + ii] = pdouble[ii];
							}

						bool sent = false;
						sent = udp.sendData(buf, sizeof(buf));

						/*if (sent)
							cout << realVal << endl;*/
					}
				}

			}
		}
		/*auto start = high_resolution_clock::now();
		Sleep((DWORD) 10);
		auto end = high_resolution_clock::now();
		cout << "sleep:  " << duration_cast<chrono::microseconds>(end - start).count() << endl;

		*///cout << ii << endl;

		if (_kbhit())
		{
			cout << endl << "Stop" << endl;
			break;
		}

	}

}

void ControllerTest::saveResults(){
	ofstream out1, out2, out3;
	try{
		//out1.open("BandPowerOutputSEQ.txt");
		out2.open("BandPowerOutputPAR.txt");
		out3.open("FeedbackOutput.txt");
	}
	catch (ofstream::failure e)
	{
		cout << "Unable to write output file" << endl;
		return;
	}

	for (unsigned int i = 0; i < BPvaluesPar.size(); i++){
		//out1 << BPvalues[i] << endl;
		out2 << BPvaluesPar[i] << endl;
		out3 << feedBackvalues[i] << endl;
	}

	//out1.close();
	out2.close();
	out3.close();
}