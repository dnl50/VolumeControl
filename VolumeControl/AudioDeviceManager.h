#pragma once
#include <minwindef.h>
#include "DeviceEnumNotificationHandler.h"
#include "VolumeController.h"
#include "EndpointVolumeChangeHandler.h"

/// forward declare
class VolumeController;

class AudioDeviceManager : public DeviceEnumNotificationHandler, public  EndpointVolumeChangeHandler {

private:
	const VolumeController& volController;

	IMMDeviceEnumerator* deviceEnum;

	IMMDevice* currentDefaultDevice;

	IAudioEndpointVolume* defaultDeviceVolume;

	LPWSTR currentDefaultDeviceID;

	/// --- Private Functions ---
	void updateDefaultDeviceParamsAndNotify();

public:
	explicit AudioDeviceManager(const VolumeController& volController);
	~AudioDeviceManager();

	// initialize the AudioDeviceManager by getting the default device etc.
	// explicit method because it can't be done in the contructor because it
	// the member variable in VolumeController has to be set first
	void initAndNotify();

	// returns a pointer to the current default audio endpoint device.
	// NOTE: the pointer must be released by calling the IMMDevice::Release method!
	IMMDevice* getCurrentDefaultDevice() const;

	// returns a pointer to a wide character string that contains the devive ID.
	LPWSTR getCurrentDefaultDeviceID() const;

	// returns a pointer to the audio endpoint device specified by the ID.
	// NOTE: the pointer must be released by calling the IMMDevice::Release method!
	IMMDevice* getDeviceByID(const LPCWSTR id) const;

	// returns a pointer to the endpoint volume for the default device.
	// NOTE: the pointer must be released by calling the IAudioEndpointVolume::Release method!
	IAudioEndpointVolume* getEndpointVolume() const;

	// releases all references
	void shutdown() const;

	// -------------------------------------
	// --- DeviceEnumNotificationHandler ---
	// ------------ Functions --------------
	// -------------------------------------
		
	HRESULT OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDefaultDeviceId) final;
	HRESULT OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key) final;

	// -----------------------------------
	// --- EndpointVolumeChangeHandler ---
	// ----------- Functions -------------
	// -----------------------------------

	HRESULT OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify) override;
};

