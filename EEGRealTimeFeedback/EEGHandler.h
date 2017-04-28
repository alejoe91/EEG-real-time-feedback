/*
EEGRealTime Feedback

Alessio Buccino
*/
#pragma once
#include "stdafx.h"
#include "Queue.h"

#include <boost\thread.hpp>


#pragma once

class EEGHandler {

private:

	// vector containing raw data to filter (length = 2*TW) for each channel
	vector<vector<double>> eegRaw;
	// vector containing filtered data (length = TW) for each channel for each freq band
	vector<vector<vector<double>>> eegBP;

	//Filter INFO
	int Nbands;

	// vector buffer to filter: NBANDS x NCHAN x BufferSize
	vector<vector<vector<double>>> eegBuffer;

	// vector containing BPMeans of different channels in different bands
	vector<vector<double>> BandPowerMean;

	// bool for normalize or load mu and sigma from file: norm = true -
	bool norm;

	////normalization parameters (mean and variance, sum of values, sum of squares)
	vector<double> muS;
	vector<double> sigS;
	vector<double> sumS;
	vector<double> sum2S;

	vector<long long> n;

	vector<double> mu;
	vector<double> sig;
	vector<double> sum;
	vector<double> sum2;

	long long ntw;

	// vectors containing filter coefficients
	vector<vector<double>> BPcoeff;
	vector<double> LPcoeff; 

	// number of channels
	int Nchannels;

	int tw;
	int percentOverlap;
	
	void computeBPMeans(int chan);
	void computeBPMeansPar(int chan);
	void parallelProcess(int chan);
	

public:

	// Default Constructor
	EEGHandler();
	// Constructor: creates Nchan vectors for every member variable 
	EEGHandler(int Nchan);
	// Constructor: creates Nchan vectors for every member variable and loads filter parameters
	EEGHandler(int Nchan, vector<vector<double>> BP, vector<double> LP);
	// Destructor
	~EEGHandler();

	// HandleEEG will process the signals and compute and return the Band Pass powers
	vector<double> handleEEGParallel(vector<vector<double>>& newRaw);

	void setBPcoeff();
	void setLPcoeff();

	// set from controller if normalization should be done online
	void setNorm(bool normal);

	// load and save mu and sigma (for signals and output)
	void loadParameters();
	void saveParameters();

	// Filter filters in into out with coeff filter coefficients
	void filter(vector<double>* coeff, vector<double>* in, vector<double>* out, int filterLength, int inLength, bool abs);
	// FIR Filter filters in into out with coeff filter coefficients
	void filterFIR(vector<double>* coeff, vector<double>* in, vector<double>* out, int filterLength, int inLength, bool abs);
	// FIR Filter filters in into out with coeff filter coefficients from 'from' to end
	void filterFIR(vector<double>* coeff, vector<double>* in, vector<double>* out, int filterLength, int inLength, bool abs, int from);
	// Compute mean of vector
	double computeMean(vector<double> sig);
	// Compute std of vector
	double computeStd(vector<double> sig);
	// Normalize vector with mean and std
	void normalize(vector<double>* sig, double mean, double std);

};