#pragma once
#include <minwindef.h>
#include "DeviceEnumNotificationHandler.h"
#include "VolumeController.h"

/// forward declare
class VolumeController;

class AudioDeviceManager : public DeviceEnumNotificationHandler {

private:
	const VolumeController& volController;

	IMMDeviceEnumerator* deviceEnum;

	IMMDevice* currentDefaultDevice;

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

	IMMDevice* getCurrentDefaultDevice() const;
	
	// the pointer returned by this function must be freed with the CoTaskFreeMem Function
	LPWSTR getCurrentDefaultDeviceID() const;

	IMMDevice* getDeviceByID(const LPCWSTR id) const;

	void shutdown() const;

	/// -------------------------------------
	/// --- DeviceEnumNotificationHandler ---
	/// ------------ Functions --------------
	/// -------------------------------------
		
	HRESULT OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDefaultDeviceId) final;
	HRESULT OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key) final;

};

