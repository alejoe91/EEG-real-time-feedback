/*
EEGRealTime Feedback

Alessio Buccino
*/

#include "stdafx.h"
#include "USBController.h"

USBController::USBController() {
	opened = false;
}

USBController::USBController(int baudRate, char* com)  {
	io = new io_service();
	port = new serial_port((*io));

	try {
		port->open(com);
		if (port->is_open()){
			cout << endl << "COM communication OPEN" << endl;
			opened = true;

			// what baud rate do we communicate at
			serial_port_base::baud_rate BAUD(baudRate);
			// how big is each "packet" of data (default is 8 bits)
			serial_port_base::character_size CSIZE(8);
			// what flow control is used (default is none)
			serial_port_base::flow_control FLOW(serial_port_base::flow_control::none);
			// what parity is used (default is none)
			serial_port_base::parity PARITY(serial_port_base::parity::none);
			// how many stop bits are used (default is one)
			serial_port_base::stop_bits STOP(serial_port_base::stop_bits::one);


			port->set_option(BAUD);
			port->set_option(CSIZE);
			port->set_option(FLOW);
			port->set_option(PARITY);
			port->set_option(STOP);
		}
	}
	catch (boost::system::system_error e) {
		cout << "Unable to OPEN COM communication" << endl;
		cout << "Error: " << e.what() << endl;
		opened = false;
	}

}

USBController::~USBController(){
	//port->close();
}

bool USBController::isOpen(){
	return opened;
}

void USBController::USBClose(){
	port->close();
}

bool USBController::USBWrite(unsigned char value){

	unsigned char command[1] = { 0 };
	command[0] = value;

	if (port->is_open()){
		port->write_some(buffer(command));
		return true;
	}
	else
		return false;
}
