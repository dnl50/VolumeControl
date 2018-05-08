#pragma once
#include "Listener.h"
#include "VolumeController.h"

using std::unique_ptr;

class Logger : public Listener {
private:
	VolumeController& volController;
	
public:
	explicit Logger(VolumeController& volController);
	~Logger();

	void OnDefaultDeviceChanged() override;
	void OnSessionCreated(unsigned id) override;
	void OnSessionRemoved(unsigned id) override;
	void OnVolumeChanged(unsigned id, const float newVolume, const bool newMute) override;
	void OnDefaultDevicePropertyChanged(PROPERTYKEY key) override;
	void OnEndpointVolumeChanged(float newVolume, bool newMute) override;
};

