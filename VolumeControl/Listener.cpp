#include "stdafx.h"
#include "Listener.h"
#include <iostream>

Listener::Listener() = default;

Listener::~Listener() = default;

void Listener::OnDefaultDeviceChanged() {
	/// default implementation
	/// do nothing
}

void Listener::OnSessionCreated(unsigned id) {
	/// default implementation
	/// do nothing
}

void Listener::OnSessionRemoved(unsigned id) {
	/// default implementation
	/// do nothing
}

void Listener::OnVolumeChanged(unsigned id, const float newVolume, const bool newMute) {
	/// default implementation
	/// do nothing
}

void Listener::OnDefaultDevicePropertyChanged(PROPERTYKEY key) {
	/// default implementation
	/// do nothing
}
