#pragma once
class Listener {
public:
	Listener();
	virtual ~Listener();

	virtual void OnDefaultDeviceChanged();
	virtual void OnSessionCreated(unsigned int id);
	virtual void OnSessionRemoved(unsigned int id);
	virtual void OnVolumeChanged(unsigned int id, const float newVolume, const bool newMute);
	virtual void OnDefaultDevicePropertyChanged(PROPERTYKEY key);

};

