#pragma once
#include "Listener.h"

class Logger : public Listener {
public:
	Logger();
	~Logger();

	void OnDefaultDeviceChanged() override;
	void OnSessionCreated(unsigned id) override;
	void OnSessionRemoved(unsigned id) override;
	void OnVolumeChanged(unsigned id, const float newVolume, const bool newMute) override;
	void OnDefaultDevicePropertyChanged(PROPERTYKEY key) override;
};

