#include "stdafx.h"

#include "ListenerNotifier.h"
#include <algorithm>
#include <iostream>

using std::make_shared;
using std::shared_ptr;
using std::list;
using std::find;

ListenerNotifier::ListenerNotifier() : listeners(make_shared<list<shared_ptr<Listener>>>()) {
}

ListenerNotifier::~ListenerNotifier() = default;

void ListenerNotifier::registerListener(shared_ptr<Listener> listener) const {
	listeners->insert(listeners->end(), listener);
}

void ListenerNotifier::unregisterListener(shared_ptr<Listener> listener) const {
	listeners->remove(listener);
}

void ListenerNotifier::notifyOnSessionCreated(const unsigned id) {
	for (const auto& listener : *listeners) {
		listener->OnSessionCreated(id);
	}
}

void ListenerNotifier::notifyOnSessionRemoved(const unsigned id) {
	for (const auto& listener : *listeners) {
		listener->OnSessionCreated(id);
	}
}

void ListenerNotifier::notifyOnVolumeChanged(const unsigned id, const float newVolume, const bool newMute) {
	for (const auto& listener : *listeners) {
		listener->OnVolumeChanged(id, newVolume, newMute);
	}
}

void ListenerNotifier::notifyOnDefaultDeviceChanged() {
	for(const auto& listener : *listeners) {
		listener->OnDefaultDeviceChanged();
	}
}

void ListenerNotifier::notifyOnDefaultDevicePropertyChanged(PROPERTYKEY key) {
	for (const auto& listener : *listeners) {
		listener->OnDefaultDevicePropertyChanged(key);
	}
}
