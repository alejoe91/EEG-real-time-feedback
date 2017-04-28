/*
EEGRealTime Feedback

Alessio Buccino
*/
//
#include "stdafx.h"
#include "MicroEEGConnector.h"

#include <boost\thread.hpp>

class StreamingThread {

private:
	boost::thread m_thread;
	bool tw_reached;
	bool stopped;
	MicroEEGConnector uEEG;

	vector<vector<double>> eegRaw;

	int Nchan;
	int nsamples;

	clock_t t_start;

public:
	StreamingThread();
	StreamingThread(int Nchannels);
	~StreamingThread();
	void Start();
	void Join();
	void Stream();

	bool getTW_status();
	void setTW_status(bool status);
	bool getStopped();
	vector<vector<double>>& getxxeeg();

	void closeConnection();

};