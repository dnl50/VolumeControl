#pragma once

#include <boost/asio/serial_port.hpp>
#include <boost/asio.hpp>

using boost::asio::serial_port;

//typedef serial_port SerialPort;

class COMController {

private:

	//std::shared_ptr<>
	//std::shared_ptr<SerialPort> serial;

public:
	COMController();
	~COMController();


};

