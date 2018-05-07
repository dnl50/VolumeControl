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
	try {
		const auto volController = make_unique<VolumeController>();
		const auto logger = std::make_shared<Logger>();

		volController->getListenerNotifier().registerListener(logger);

		volController->init();

		std::this_thread::sleep_for(std::chrono::seconds(20));
	} catch (runtime_error& e)
	{
		DEBUG_PRINT(e.what());

		return -1;
	}
}
