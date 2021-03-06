// VolumeControl.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include "VolumeController.h"
#include "Logger.h"
#include <thread>
#include <iostream>

using std::unique_ptr;
using std::make_unique;
using std::runtime_error;


int main() {	
	
	const auto volController = make_unique<VolumeController>();
	const auto logger = std::make_shared<Logger>(*volController);

	volController->getListenerNotifier().registerListener(logger);

	if(FAILED(volController->init())) {
		return -1; 
	}

	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

}
