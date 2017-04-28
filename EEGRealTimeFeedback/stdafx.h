// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#include "targetver.h"

#define _WINSOCKAPI_
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <tchar.h>
#include <objbase.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#include <windows.h>




// TODO: reference additional headers your program requires here

#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <conio.h>
#include <fstream>
#include <ctime>
#include <chrono>

//using namespace chrono;
using namespace std;

//#define NTWMEAN 100
#define TW 250
#define OVERLAP 0.9

#define NTWINFILTERED 2

#define PORTuEEGctrl 27010   //The port on which to listen for incoming data from microEEGctrl
#define DEFAULT_CHANNELS 21
#define MAXEEG_CHAN 26
#define BUFLEN 512
#define ADCRANGE 10
#define EEGSPAN 6226
#define NBIT 16
#define BAUDRATE 9600

#define IPSERVER "127.0.0.1"
#define UDPPORT 8888

// EEG BANDS definition
#define MAX_BANDS 6
#define ALPHA 2
#define BETA 1
#define MU 0
#define GAMMA 3
#define DELTA 4
#define THETA 5

// threshold to clip normalized data in order to reduca artifacts
#define CLIP_THRESH 3
#define CLIP_THRESH_SIG 15

//define Datagram struct
typedef struct {
	short int preamble; //2 bytes
	int index; //32 bit integer -> ID datagram
	char packetType; // 1 byte
	char batteryLevel;	// 1 byte
	unsigned short int values[MAXEEG_CHAN]; // signed 16-bit array of max channels
} EEGDatagram; // size = 56

