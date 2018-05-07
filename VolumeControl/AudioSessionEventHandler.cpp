#include "stdafx.h"
#include "AudioSessionEventHandler.h"
#include <iostream>

/// Init the reference counter and set the session ID to identify the observed Session
AudioSessionEventHandler::AudioSessionEventHandler(const VolumeController& volController, const UINT observedSessionID) : refCount(1UL), volController(volController), observedSessionID(observedSessionID) {

}

/// Use the default destructor
AudioSessionEventHandler::~AudioSessionEventHandler() = default;

/// ------------------------------------
/// --- IUnknown Interface Functions ---
/// ------------------------------------

// Return a pointer to the requested interface if this interface supports it.
HRESULT AudioSessionEventHandler::QueryInterface(const IID& riid, void** ppvObject) {

	// validate given pointer
	if (!ppvObject) {
		return E_INVALIDARG;
	}

	*ppvObject = nullptr;
	if (riid == IID_IUnknown) {

		// Increment the reference count and return the pointer
		AddRef();

		*ppvObject = static_cast<IUnknown*>(this);

	}
	else if (riid == __uuidof(IAudioSessionEvents)) {

		// Increment the reference count and return the pointer
		AddRef();

		*ppvObject = static_cast<IAudioSessionEvents*>(this);
	}
	else {
		*ppvObject = nullptr;

		return E_NOINTERFACE;
	}

	return S_OK;
}

// increment the object's internal counter
ULONG AudioSessionEventHandler::AddRef() {

	// use the InterlockedIncrement function for thread safety
	InterlockedIncrement(&refCount);

	return refCount;
}

// Decrement the object's internal counter
ULONG AudioSessionEventHandler::Release() {

	// use the InterlockedDecrement function for thread safety
	const auto ulRefCount = InterlockedDecrement(&refCount);

	if (0 == refCount) {
		delete this;
	}

	return ulRefCount;
}


/// -----------------------------------------------
/// --- IAudioSessionEvents Interface Functions ---
/// -----------------------------------------------

// Notifies the client that the display name for the session has changed
HRESULT AudioSessionEventHandler::OnDisplayNameChanged(LPCWSTR NewDisplayName, LPCGUID EventContext) {
	return S_OK;
}

// Notifies the client that the display icon for the session has changed
HRESULT AudioSessionEventHandler::OnIconPathChanged(LPCWSTR NewIconPath, LPCGUID EventContext) {
	return S_OK;
}

// Notifies the client that the volume level or muting state of the session has changed
HRESULT AudioSessionEventHandler::OnSimpleVolumeChanged(float NewVolume, BOOL NewMute, LPCGUID EventContext) {
	volController.getListenerNotifier().notifyOnVolumeChanged(observedSessionID, NewVolume, NewMute);

	return S_OK;
}

// Notifies the client that the volume level of an audio channel in the session submix has changed
HRESULT AudioSessionEventHandler::OnChannelVolumeChanged(DWORD ChannelCount, float NewChannelVolumeArray[],
	DWORD ChangedChannel, LPCGUID EventContext) {

	/// channel specific volume changes are irrelevant (for now LUL)

	return S_OK;
}

// Notifies the client that the grouping parameter for the session has changed
HRESULT AudioSessionEventHandler::OnGroupingParamChanged(LPCGUID NewGroupingParam, LPCGUID EventContext) {
	return S_OK;
}

// Notifies the client that the stream-activity state of the session has changed
HRESULT AudioSessionEventHandler::OnStateChanged(AudioSessionState NewState) {
	if(NewState == AudioSessionStateExpired) {
		// TODO: Session Expired
	}

	return S_OK;
}

// Notifies the client that the session has been disconnected.
HRESULT AudioSessionEventHandler::OnSessionDisconnected(AudioSessionDisconnectReason DisconnectReason) {

	// TODO: implement AudioSessionEventHandler::OnSessionDisconnected!

	std::cout << "Session Disconnected!" << std::endl;

	return S_OK;
}
