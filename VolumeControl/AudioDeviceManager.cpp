#include "stdafx.h"
#include "AudioDeviceManager.h"
#include <iostream>


void AudioDeviceManager::updateDefaultDeviceParamsAndNotify() {
	SAFERELEASE(currentDefaultDevice);

	/// get default device
	auto hr = deviceEnum->GetDefaultAudioEndpoint(
		EDataFlow::eRender,
		ERole::eMultimedia,
		&currentDefaultDevice
	);

	if (FAILED(hr) || !currentDefaultDevice) {
		DEBUG_PRINT("Failed to get default render device!");

		return;
	}

	/// get the default device ID
	CoTaskMemFree(currentDefaultDeviceID);
	currentDefaultDeviceID = nullptr;

	hr = currentDefaultDevice->GetId(&currentDefaultDeviceID);

	if(FAILED(hr) || !currentDefaultDeviceID) {
		DEBUG_PRINT("Failed to get default render device ID!");

		return;
	}

	volController.getListenerNotifier().notifyOnDefaultDeviceChanged();
}

AudioDeviceManager::AudioDeviceManager(const VolumeController& volController) : volController(volController),
		deviceEnum(nullptr), 
		currentDefaultDevice(nullptr), 
		currentDefaultDeviceID(nullptr) {
}

AudioDeviceManager::~AudioDeviceManager() = default;

void AudioDeviceManager::initAndNotify() {
	/// get device enum
	const auto hr = CoCreateInstance(
		__uuidof(MMDeviceEnumerator),
		nullptr,
		CLSCTX_ALL,
		__uuidof(IMMDeviceEnumerator),
		reinterpret_cast<void**>(&deviceEnum)
	);

	if (FAILED(hr) || !deviceEnum) {
		DEBUG_PRINT("Failed to get Device Enum!");

		SAFERELEASE(deviceEnum);

		throw std::runtime_error("Failed to get Device Enum!");
	}

	ASSERT(deviceEnum);

	/// register as device change listener
	deviceEnum->RegisterEndpointNotificationCallback(this);

	updateDefaultDeviceParamsAndNotify();
}

IMMDevice* AudioDeviceManager::getCurrentDefaultDevice() const {
	return currentDefaultDevice;
}

LPWSTR AudioDeviceManager::getCurrentDefaultDeviceID() const {
	return this->currentDefaultDeviceID;
}

IMMDevice* AudioDeviceManager::getDeviceByID(const LPCWSTR id) const {
	IMMDevice* device = nullptr;

	const auto hr = deviceEnum->GetDevice(
		id,
		&device
	);	

#ifdef _DEBUG

	if (FAILED(hr) || !device) {
		DEBUG_PRINT("Error getting device!");
	}

	ASSERT(device);

#endif

	return device;
}

void AudioDeviceManager::shutdown() const {
	/// unregister as listener
	if(deviceEnum) {
		deviceEnum->UnregisterEndpointNotificationCallback((IMMNotificationClient*) this);		
	}

	SAFERELEASE(currentDefaultDevice);
	SAFERELEASE(deviceEnum);
}

HRESULT AudioDeviceManager::OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDefaultDeviceId) {
	const auto newDefault = wcscmp(pwstrDefaultDeviceId, currentDefaultDeviceID);
		
	if(flow == EDataFlow::eRender && role == ERole::eMultimedia && newDefault) {
		updateDefaultDeviceParamsAndNotify();
	}

	return S_OK;
}

HRESULT AudioDeviceManager::OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key) {
	const auto ofDefault = wcscmp(pwstrDeviceId, currentDefaultDeviceID);

	if(ofDefault) {
		volController.getListenerNotifier().notifyOnDefaultDevicePropertyChanged(key);
	}

	return S_OK;
}
