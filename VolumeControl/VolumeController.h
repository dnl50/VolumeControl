#pragma once

#include "ListenerNotifier.h"
#include "AudioDeviceManager.h"
#include "AudioSessionManager.h"

/// forward declare
class AudioDeviceManager;
class AudioSessionManager;

class VolumeController {

private:
	std::shared_ptr<AudioDeviceManager> deviceManager;
	std::shared_ptr<AudioSessionManager> sessionMngr;

	const std::shared_ptr<ListenerNotifier> notifier;

	HRESULT initCOM() const;
	void uninitCOM() const;

public:
	VolumeController();
	~VolumeController();

	HRESULT init();

	ListenerNotifier& getListenerNotifier() const;
	AudioSessionManager& getAudioSessionManager() const;
	AudioDeviceManager& getAudioDeviceManager() const;
};

