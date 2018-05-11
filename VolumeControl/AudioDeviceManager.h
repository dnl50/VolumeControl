#pragma once
#include <minwindef.h>
#include "DeviceEnumNotificationHandler.h"
#include "VolumeController.h"
#include "EndpointVolumeChangeHandler.h"

/// forward declare
class VolumeController;

class AudioDeviceManager : public DeviceEnumNotificationHandler, public  EndpointVolumeChangeHandler {

private:

	// reference zo the main controller
	const VolumeController& volController;
	
	// pointer to the IMMDeviceEnumerator interface to iterate
	// over all devices and listen for changes
	std::shared_ptr<IMMDeviceEnumerator> deviceEnum;
	
	// pointer to thr IMMDevice interface for the current default audio 
	// rendering device
	std::shared_ptr<IMMDevice> currentDefaultDevice;

	// pointer to the IAudioEndpointVolume interface to control the master
	// volume and listen to volume changes of the current default audio
	// rendering device
	std::shared_ptr<IAudioEndpointVolume> defaultDeviceVolume;
	
	// pointer to a wide character char array that contains the device ID
	// of the current audio rendering device
	std::shared_ptr<wchar_t> currentDefaultDeviceID;

	/// -------------------------
	/// --- Private Functions ---
	/// -------------------------
	
	// sets the default device, device ID and defaultDeviceVolume, registers this object as a 
	// volume change listener. notifies all listeners via the ListenerNotifier retrieved by
	// the VolumeController::getNotifier method.
	HRESULT updateDefaultDeviceParamsAndNotify();

public:

	// constructor
	explicit AudioDeviceManager(const VolumeController& volController);
	
	// destructor
	~AudioDeviceManager();

	// initialize the AudioDeviceManager by getting the default device etc.
	// explicit method because it can't be done in the contructor because it
	// the member variable in VolumeController has to be set first
	HRESULT initAndNotify();

	// returns a shared pointer to the current default audio endpoint device.
	std::shared_ptr<IMMDevice> getCurrentDefaultDevice() const;

	// returns a shared pointer to a wide character string that contains the devive ID.
	std::shared_ptr<wchar_t> getCurrentDefaultDeviceID() const;

	// returns a shared pointer to the audio endpoint device specified by the ID.
	std::shared_ptr<IMMDevice> getDeviceByID(const LPCWSTR id) const;

	// returns a shared pointer to the endpoint volume for the default device.
	std::shared_ptr<IAudioEndpointVolume> getEndpointVolume() const;
	
		
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

