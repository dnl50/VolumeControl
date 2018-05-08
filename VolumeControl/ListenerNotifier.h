#pragma once
#include <wincon.h>
#include <list>
#include "Listener.h"
#include <memory>

class ListenerNotifier {

private:
	const std::shared_ptr<std::list<std::shared_ptr<Listener>>> listeners;

public:
	ListenerNotifier();
	~ListenerNotifier();

	void registerListener(std::shared_ptr<Listener> listener) const;
	void unregisterListener(std::shared_ptr<Listener> listener) const;

	/// ----------------------------
	/// ----- Notifier Methods -----
	/// ----------------------------

	void notifyOnSessionAdded(const unsigned int id);
	void notifyOnSessionRemoved(const unsigned int id);
	void notifyOnVolumeChanged(const unsigned int id, const float newVolume, const bool newMute);
	void notifyOnDefaultDeviceChanged();
	void notifyOnDefaultDevicePropertyChanged(PROPERTYKEY key);
	void notifyOnEndpointVolumeChanged(float newVolume, bool newMute);
};

