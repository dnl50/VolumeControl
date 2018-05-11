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
	uninitCOM();
};

HRESULT VolumeController::initCOM() const {
	return CoInitializeEx(nullptr, COINIT_MULTITHREADED);
}

void VolumeController::uninitCOM() const {
	CoUninitialize();
}

HRESULT VolumeController::init() {
	// don't change order!
			
		
	deviceManager = make_shared<AudioDeviceManager>(*this);
		
	const auto hr = deviceManager->initAndNotify();

	if(FAILED(hr)) {
		return hr;
	}

	sessionMngr = make_shared<AudioSessionManager>(*this);
	
	return sessionMngr->init();
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
