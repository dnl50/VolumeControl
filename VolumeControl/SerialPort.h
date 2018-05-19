#pragma once

#include <boost/asio/serial_port.hpp>

// A wrapper class for the boost serial_port class
class SerialPort {

	std::unique_ptr<boost::asio::serial_port> serial_;


public:
	SerialPort();
	~SerialPort();

	const static std::vector<std::string> getAllPorts();
};

