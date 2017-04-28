/*
EEGRealTime Feedback

Alessio Buccino
*/

#include "stdafx.h"
#include "EEGHandler.h"




// PUBLIC //

EEGHandler::EEGHandler(){
	for (int i = 0; i < DEFAULT_CHANNELS; i++){
		eegRaw.push_back(vector<double>(2 * TW, 0));
		eegBP.push_back(vector<double>(TW, 0));


		BandPowerMean.push_back(0);

	}
	eegBuffer = vector<double>(NTWINFILTERED*TW, 0);

	Nchannels = DEFAULT_CHANNELS;
}

EEGHandler::EEGHandler(int Nchan){

	// Initialize all member variables for each channel
	for (int i = 0; i < Nchan; i++){
		eegRaw.push_back(vector<double>(NTWINFILTERED * TW, 0));
		eegBP.push_back(vector<double>(TW, 0));

		BandPowerMean.push_back(0);
		
		/*mu.push_back(0);
		sig.push_back(1);
		sum.push_back(0);
		sum2.push_back(0);

		muS.push_back(0);
		sigS.push_back(1);
		sumS.push_back(0);
		sum2S.push_back(0);*/
	}

	Nchannels = Nchan;
	/*ntw = 1;
	n = 1;*/
	norm = true;
}

EEGHandler::EEGHandler(int Nchan, vector<vector<double>> BP, vector<double> LP){

	for (int i = 0; i < Nchan; i++){
		eegRaw.push_back(vector<double>(NTWINFILTERED * TW, 0));
		eegBP.push_back(vector<double>(TW, 0));

		BandPowerMean.push_back(0);
		eegBufferMu.push_back(vector<double>(NTWINFILTERED*TW, 0));

		// assign worker threads
		//t.push_back(new boost::thread(&EEGHandler::parallelProcess, this, i));

		mu.push_back(0);
		sig.push_back(1);
		sum.push_back(0);
		sum2.push_back(0);

		muS.push_back(0);
		sigS.push_back(1);
		sumS.push_back(0);
		sum2S.push_back(0);

		n.push_back(1);
	}

	ntw = 1;	

	BPcoeff = BP;
	LPcoeff = LP;
	Nchannels = Nchan;

	eegBuffer = vector<double>(NTWINFILTERED*TW, 0);

	norm = true;
}

EEGHandler::~EEGHandler(){	
}


vector<double> EEGHandler::handleEEGParallel(vector<vector<double>>& newRaw){
	int overlap = (int)(TW - OVERLAP*TW);

	// workers threads for filtering each channel
	vector<boost::thread*> t;
	//vector<boost::thread> t;

	for (int i = 0; i < Nchannels; i++) {
		t.push_back(new boost::thread(&EEGHandler::parallelProcess, this, i));
		//t.push_back(boost::thread(&EEGHandler::parallelProcess, this, i));

		//Load new data (newRaw into eegRaw): overlap% -> copy only last (NTWINFILTERED*TW - overlap) samples of newRaw 
		//into (NTWINFILTERED*TW - overlap) of eegRaw
		copy(newRaw[i].end() - overlap, newRaw[i].end(), eegRaw[i].begin() + (int)(NTWINFILTERED*TW - overlap));
	}

	for (int i = 0; i < Nchannels; i++){
		t[i]->join();
		delete t[i];
	}

	ntw++;

	return BandPowerMean;

}

// Filter member function will filter all the channels in EEG raw
void EEGHandler::filter(vector<double>* coeff, vector<double>* in, vector<double>* out, int filterLength, int inLength, bool absolute){
	{
		double acc;     // accumulator for MACs
		int n;
		int k;

		// allows to use same pointer for in and out
		//vector<double>* inCopy = new vector<double>((*in));
		vector<double> inCopy = vector<double>((*in));
		vector<double> coefficients = vector<double>(*coeff);

		// apply the filter to each input sample
		for (n = filterLength; n < inLength; n++) {
			acc = 0;
			//leave first filterLength samples unchanged
			// calculate output n as: o[n] = sum k(h[k]*y[n-k]) CONVOLUTION
			//for (k = 0; k < filterLength && (n - k) >= 0; k++) {
			for (k = 0; k < filterLength; k++) {
				acc += coefficients[k] * (inCopy)[n - k];
			}
			if (absolute)
				(*out)[n] = fabs(acc);
			else
				(*out)[n] = acc;
		}
	}
}

// Filter member function will filter all the channels in EEG raw
void EEGHandler::filterFIR(vector<double>* coeff, vector<double>* in, vector<double>* out, int filterLength, int inLength, bool absolute){
	{
		double acc;     // accumulator for MACs
		int n;
		int k;

		// allows to use same pointer for in and out
		//vector<double>* inCopy = new vector<double>((*in));
		vector<double> inCopy = vector<double>((*in));
		vector<double> coefficients = vector<double>(*coeff);

		// apply the filter to each input sample
		for (n = filterLength; n < inLength; n++) {
			acc = 0;
			//leave first filterLength samples unchanged
			// calculate output n as: o[n] = sum k(h[k]*y[n-k]) CONVOLUTION
			//for (k = 0; k < filterLength && (n - k) >= 0; k++) {
			for (k = 0; k < filterLength/2; k++) {
				acc += coefficients[k] * (inCopy)[n - k] + coefficients[filterLength - k - 1] * (inCopy)[n - filterLength + 1];
			}
			// if filter is odd add middle value
			bool even = false;
			filterLength % 2 == 0 ? even = true : even = false;
			if (!even)
				acc += coefficients[filterLength / 2] * (inCopy)[n - filterLength / 2];

			if (absolute)
				(*out)[n] = fabs(acc);
			else
				(*out)[n] = acc;
		}
	}
}

// Filter member function will filter all the channels in EEG raw
void EEGHandler::filterFIR(vector<double>* coeff, vector<double>* in, vector<double>* out, int filterLength, int inLength, bool absolute, int from){
	{
		double acc;     // accumulator for MACs
		int n;
		int k;

		// allows to use same pointer for in and out
		//vector<double>* inCopy = new vector<double>((*in));
		vector<double> inCopy = vector<double>((*in));
		vector<double> coefficients = vector<double>(*coeff);


		// apply the filter to each input sample
		for (n = from; n < inLength; n++) {
			acc = 0;
			//leave first filterLength samples unchanged
			// calculate output n as: o[n] = sum k(h[k]*y[n-k]) CONVOLUTION
			for (k = 0; k < filterLength / 2; k++) {
				acc += coefficients[k] * (inCopy)[n - k] + coefficients[filterLength - k - 1] * (inCopy)[n - filterLength + 1];
			}
			// if filter is odd add middle value
			bool even = false;
			filterLength % 2 == 0 ? even = true : even = false;
			if (!even)
				acc += coefficients[filterLength / 2] * (inCopy)[n - filterLength / 2];

			if (absolute)
				(*out)[n] = fabs(acc);
			else
				(*out)[n] = acc;
		}
	}
}

// Compute vector mean
double EEGHandler::computeMean(vector<double> sig){
	double acc = 0;

	for (unsigned int i = 0; i < sig.size(); i++)
		acc += sig[i];

	return acc / (double) sig.size();
}

// Compute vector variance
double EEGHandler::computeStd(vector<double> sig){
	double mu = computeMean(sig);
	double var = 0;

	for (unsigned int i = 0; i < sig.size(); i++)
		var += pow((sig[i] - mu), 2);
	var /= (sig.size() - 1);

	return sqrt(var);
}

void EEGHandler::setNorm(bool normal){
	norm = normal;
}

// load and save parameters
void EEGHandler::loadParameters(){
	ifstream in1, in2;
	try {
		in1.open("SignalParameters.txt");
		in2.open("FeaturesParameters.txt");
	}
	catch (ofstream::failure e) {
		cout << "Unable to LOAD PARAMETERS DATA" << endl;
		norm = true;
		return;
	}

	for (int i = 0; i < Nchannels; i++){
		in1 >> muS[i] >> sigS[i];
		in2 >> mu[i] >>  sig[i];
	}

	in1.close();
	in2.close();
}

void EEGHandler::saveParameters(){
	// save parameters only if online normalization has been done

	if (norm) {
		ofstream out1, out2;

		try {
			out1.open("SignalParameters.txt");
			out2.open("FeaturesParameters.txt");
		}
		catch (ofstream::failure e) {
			cout << "Unable to SAVE PARAMETERS DATA" << endl;
			return;
		}

		for (int i = 0; i < Nchannels; i++){
			out1 << muS[i] << "\t" << sigS[i] << endl;
			out2 << mu[i] << "\t" << sig[i] << endl;
		}

		out1.close();
		out2.close();
	}

}



// PRIVATE

// Compute mean of eegBP (mean of band power over a time window)
void EEGHandler::computeBPMeansPar(int chan){

	double BPnotNorm;

	BPnotNorm = computeMean(eegBP[chan]);
	/*if (i == 3)
	cout << BPnotNorm << endl;*/

	// Log transform
	if (BPnotNorm == 0)
		BPnotNorm = 0.1;
	BPnotNorm = log2(BPnotNorm);

	//////// Update parameters, normalize and clip noise /////////

	//mean
	mu[chan] = sum[chan] / ntw + BPnotNorm / ntw;
	sum[chan] += BPnotNorm;
	sum2[chan] += pow(BPnotNorm, 2);

	if (ntw >= 30){ //start estimating variance after 15 s
		sig[chan] = (sum2[chan] - 2 * mu[chan] * sum[chan]) / (ntw - 1) + pow(mu[chan], 2) + pow(BPnotNorm - mu[chan], 2) / (ntw - 1);
	}

	BandPowerMean[chan] = (BPnotNorm - mu[chan]) / sqrt(sig[chan]);

	//Clip the signals
	if (BandPowerMean[chan] > CLIP_THRESH)
		BandPowerMean[chan] = CLIP_THRESH;
	else if (BandPowerMean[chan] < -CLIP_THRESH)
		BandPowerMean[chan] = -CLIP_THRESH;

}
	
void EEGHandler::parallelProcess(int i){

	vector<double> eegBufferSmooth(NTWINFILTERED*TW, 0);

	int overlap = (int)(TW - OVERLAP*TW);

	//Band Pass Filter and return absolute value
	filterFIR(&BPcoeff[0], &eegRaw[i], &eegBufferMu[i], BPcoeff[0].size(), eegRaw[i].size(), false , eegRaw[i].size() - overlap);

	////// Update parameters, normalize and clip noise /////////

	// use last (int)(TW - OVERLAP*TW) values
	for (int j = (int)(NTWINFILTERED*TW - overlap); j < NTWINFILTERED*TW; j++){
		//Clip the signals
		if (eegBufferMu[i][j] > CLIP_THRESH_SIG)
			eegBufferMu[i][j] = CLIP_THRESH_SIG;
		else if (eegBufferMu[i][j] < -CLIP_THRESH_SIG)
			eegBufferMu[i][j] = -CLIP_THRESH_SIG;


		//If parameters are not stored in file estimate them real time
		if (norm) {
			//mean
			muS[i] = sumS[i] / n[i] + eegBufferMu[i][j] / n[i];

			sumS[i] += eegBufferMu[i][j];
			sum2S[i] += pow(eegBufferMu[i][j], 2);

			if (n[i] >= 250){ //start estimating variance after 1 s
				sigS[i] = (sum2S[i] - 2 * muS[i] * sumS[i]) / (n[i] - 1) + pow(muS[i], 2) + pow(eegBufferMu[i][j] - muS[i], 2) / (n[i] - 1);
			}
			n[i]++;
		}

		eegBufferMu[i][j] = (eegBufferMu[i][j] - muS[i]) / sqrt(sigS[i]);

		//Clip the signals
		if (eegBufferMu[i][j] > CLIP_THRESH)
			eegBufferMu[i][j] = CLIP_THRESH;
		else if (eegBufferMu[i][j] < -CLIP_THRESH)
			eegBufferMu[i][j] = -CLIP_THRESH;

		// Compute absolute value
		eegBufferMu[i][j] = abs(eegBufferMu[i][j]);

	}
	

	//Low Pass Filter -> compute envelope
	filterFIR(&LPcoeff, &eegBufferMu[i], &eegBufferSmooth, LPcoeff.size(), eegBuffer.size(), false , eegBuffer.size() - overlap);

	//Copy filtered data (last TW) into eegBP
	copy(eegBufferSmooth.begin() + (NTWINFILTERED - 1)*TW, eegBufferSmooth.end(), eegBP[i].begin());


	computeBPMeansPar(i);
	// after computing Powers: shift EEGRaw back to leave room for next TW/2 samples
	copy(eegRaw[i].begin() + overlap, eegRaw[i].end(), eegRaw[i].begin());
	copy(eegBufferMu[i].begin() + overlap, eegBufferMu[i].end(), eegBufferMu[i].begin());
	//copy(eegBufferSm[i].begin() + overlap, eegBufferSm[i].end(), eegBufferSm[i].begin());


}
