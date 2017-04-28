/*
EEGRealTime Feedback

Alessio Buccino
*/

#pragma once
#include <boost/asio.hpp>
#include "stdafx.h"

using namespace::boost::asio;


class USBController {
private:
	char *PORT = NULL;
	io_service*  io;
	serial_port* port;
	bool opened;

public:
	USBController();
	USBController(int baudRate, char* com);
	~USBController();
	bool isOpen();
	void USBClose();
	bool USBWrite(unsigned char value);

};