#include "stdafx.h"
#include "VolumeController.h"
#include <iostream>

using std::make_shared;
using std::runtime_error;

VolumeController::VolumeController() : deviceManager(nullptr), sessionMngr(nullptr), notifier(make_shared<ListenerNotifier>()) {
	const auto hr = initCOM();

	if(FAILED(hr)) {
		throw std::runtime_error("Failed to init COM!");
	}
	
}

VolumeController::~VolumeController() {
	if(sessionMngr) {
		sessionMngr->shutdown();
	}

	if(deviceManager) {
		deviceManager->shutdown();
	}

	uninitCOM();
};

HRESULT VolumeController::initCOM() const {
	return CoInitializeEx(nullptr, COINIT_MULTITHREADED);
}

void VolumeController::uninitCOM() const {
	CoUninitialize();
}

void VolumeController::init() {
	// don't change order!
	try {
		sessionMngr = make_shared<AudioSessionManager>(*this);
		notifier->registerListener(sessionMngr);

		deviceManager = make_shared<AudioDeviceManager>(*this);
		deviceManager->initAndNotify();

	} catch(std::runtime_error& e) {
		std::cout << "ERROR" << std::endl;
	}
}

ListenerNotifier& VolumeController::getListenerNotifier() const {
	return *notifier;
}

AudioSessionManager& VolumeController::getAudioSessionManager() const {
	return *sessionMngr;
}

AudioDeviceManager& VolumeController::getAudioDeviceManager() const {
	return *deviceManager;
}
