#include "stdafx.h"
#include "AudioDeviceManager.h"
#include <iostream>

#define DEVICE_ENUM_DELETER [this](IMMDeviceEnumerator* devEnum) { \
	if (devEnum) { \
		devEnum->UnregisterEndpointNotificationCallback(this); \
	} \
	SAFERELEASE(devEnum); \
}

#define DEVICE_DELETER [](IMMDevice* dev) { \
	SAFERELEASE(dev) \
}

#define LPWSTR_DELETER [](wchar_t* arr) { \
	CoTaskMemFree(arr); \
}

#define VOLUME_DELETER [this](IAudioEndpointVolume* vol) { \
	if (vol) { \
		vol->UnregisterControlChangeNotify(this); \
	} \
	SAFERELEASE(vol); \
}


using std::shared_ptr;
using std::wstring;
using std::make_shared;

AudioDeviceManager::AudioDeviceManager(const VolumeController& volController) : volController(volController),
		deviceEnum(nullptr, DEVICE_ENUM_DELETER), 
		currentDefaultDevice(nullptr, DEVICE_DELETER), 
		currentDefaultDeviceID(nullptr, LPWSTR_DELETER), 
		defaultDeviceVolume(nullptr, VOLUME_DELETER)
{

}

AudioDeviceManager::~AudioDeviceManager() = default;

HRESULT AudioDeviceManager::initAndNotify() {
	
	// get device enum
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

		return hr;
	}

	// register as device change listener
	deviceEnum->RegisterEndpointNotificationCallback(this);

	return updateDefaultDeviceParamsAndNotify();
}

HRESULT AudioDeviceManager::updateDefaultDeviceParamsAndNotify() {
	// get default device
	IMMDevice* defDev = nullptr;

	auto hr = deviceEnum->GetDefaultAudioEndpoint(
		EDataFlow::eRender,
		ERole::eMultimedia,
		&defDev
	);

	if (FAILED(hr) || !defDev) {
		DEBUG_PRINT("Failed to get default render device!");

		return hr;
	}

	currentDefaultDevice = shared_ptr<IMMDevice>(defDev, DEVICE_DELETER);

	/// get default device ID
	auto devId = new wchar_t;
		
	hr = currentDefaultDevice->GetId(&devId);

	if (FAILED(hr) || !devId) {
		DEBUG_PRINT("Failed to get default render device ID!");

		delete devId;

		return hr;
	}

	currentDefaultDeviceID = shared_ptr<wchar_t>(devId, LPWSTR_DELETER);

	// get default device volume
	IAudioEndpointVolume* volPtr = nullptr;
	
	hr = currentDefaultDevice->Activate(
		__uuidof(IAudioEndpointVolume),
		CLSCTX_ALL,
		nullptr,
		reinterpret_cast<void**>(&volPtr)
	);
		
	if (FAILED(hr) || !volPtr) {
		DEBUG_PRINT("Failed to get default render device volume!");

		return hr;
	}

	defaultDeviceVolume = shared_ptr<IAudioEndpointVolume>(volPtr, VOLUME_DELETER);

	// add ref
	defaultDeviceVolume->AddRef();

	// register as listener
	defaultDeviceVolume->RegisterControlChangeNotify(this);

	// notify the listeners that the default device changed
	volController.getListenerNotifier().notifyOnDefaultDeviceChanged();

	// notify the listeners that the default device volume level changed
	const auto vol = getDefaultDeviceVolumeFloat();
	const auto mute = getDefautlDeviceVolumeMute();

	volController.getListenerNotifier().notifyOnEndpointVolumeChanged(vol, mute);

	return hr;
}

shared_ptr<IMMDevice> AudioDeviceManager::getCurrentDefaultDevice() const {
	return currentDefaultDevice;
}

shared_ptr<wchar_t> AudioDeviceManager::getCurrentDefaultDeviceID() const {
	return currentDefaultDeviceID;
}

shared_ptr<IMMDevice> AudioDeviceManager::getDeviceByID(const LPCWSTR id) const {
	IMMDevice* device = nullptr;

	const auto hr = deviceEnum->GetDevice(
		id,
		&device
	);	

	// return nullptr if it failed
	if(FAILED(hr) || !device) {
		return shared_ptr<IMMDevice>(nullptr);
	}

	// return shared pointer with custom deleter
	return shared_ptr<IMMDevice>(device, DEVICE_DELETER);
}

shared_ptr<wstring> AudioDeviceManager::getDefaultDeviceName() const {
	IPropertyStore* deviceProps = nullptr;
	PROPVARIANT varName;

	auto hr = getCurrentDefaultDevice()->OpenPropertyStore(STGM_READ,
		&deviceProps);

	if(FAILED(hr)) {
		return make_shared<wstring>(L"Error getting device name!");
	}

	hr = deviceProps->GetValue(
		PKEY_Device_FriendlyName,
		&varName
	);

	shared_ptr<wstring> deviceName;

	if(SUCCEEDED(hr)) {
		deviceName = make_shared<wstring>(varName.pwszVal);
	} else {
		deviceName = make_shared<wstring>(L"Error getting device name!");
	}

	// clear the variables properly
	PropVariantClear(&varName);

	if(deviceProps) {
		deviceProps->Release();
		deviceProps = nullptr;
	}

	return deviceName;
}

shared_ptr<IAudioEndpointVolume> AudioDeviceManager::getEndpointVolume() const {
	return defaultDeviceVolume;
}

float AudioDeviceManager::getDefaultDeviceVolumeFloat() const {
	auto vol = -1.f;

	getEndpointVolume()->GetMasterVolumeLevelScalar(&vol);
		
	return vol;
}

BOOL AudioDeviceManager::getDefautlDeviceVolumeMute() const {
	auto mute = FALSE;

	getEndpointVolume()->GetMute(&mute);

	return mute;
}

HRESULT AudioDeviceManager::OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDefaultDeviceId) {
	// only do something when the default device really changed (new ID)
	const auto newDefault = (!currentDefaultDeviceID || !currentDefaultDeviceID.get()) ? true : wcscmp(pwstrDefaultDeviceId, currentDefaultDeviceID.get());
	
	if(flow == EDataFlow::eRender && role == ERole::eMultimedia && newDefault) {
		const auto hr = updateDefaultDeviceParamsAndNotify();

		// notify the listeners that the device volume could have been
		// changed potentially
		if(SUCCEEDED(hr)) {
			auto devVol = 0.f;
			auto muted = FALSE;

			const auto hr1 = defaultDeviceVolume->GetMasterVolumeLevelScalar(&devVol);
			const auto hr2 = defaultDeviceVolume->GetMute(&muted);

			if(SUCCEEDED(hr1) && SUCCEEDED(hr2)) {
				volController.getListenerNotifier().notifyOnEndpointVolumeChanged(devVol, muted);
			}
		}
	}

	return S_OK;
}

HRESULT AudioDeviceManager::OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key) {
	// only care for default device changes
	const auto ofDefault = (currentDefaultDeviceID.get() == nullptr) ? false : wcscmp(pwstrDeviceId, currentDefaultDeviceID.get());

	if(ofDefault) {
		volController.getListenerNotifier().notifyOnDefaultDevicePropertyChanged(key);
	}

	return S_OK;
}

HRESULT AudioDeviceManager::OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify) {
	volController.getListenerNotifier().notifyOnEndpointVolumeChanged(pNotify->fMasterVolume, pNotify->bMuted);

	return S_OK;
}