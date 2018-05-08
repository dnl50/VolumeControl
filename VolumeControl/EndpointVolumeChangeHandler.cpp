#include "stdafx.h"
#include "EndpointVolumeChangeHandler.h"


EndpointVolumeChangeHandler::EndpointVolumeChangeHandler() : refCount(1UL) {

}


EndpointVolumeChangeHandler::~EndpointVolumeChangeHandler() = default;

// ------------------------------------
// --- IUnknown Interface Functions ---
// ------------------------------------

/// Return a pointer to the requested interface if this interface supports it.
HRESULT EndpointVolumeChangeHandler::QueryInterface(const IID& riid, void** ppvObject) {

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
	else if (riid == __uuidof(IAudioEndpointVolumeCallback)) {

		// Increment the reference count and return the pointer
		AddRef();

		*ppvObject = static_cast<IAudioEndpointVolumeCallback*>(this);
	}
	else {
		*ppvObject = nullptr;

		return E_NOINTERFACE;
	}

	return S_OK;
}

/// increment the object's internal counter
ULONG EndpointVolumeChangeHandler::AddRef() {

	// use the InterlockedIncrement function for thread safety
	InterlockedIncrement(&refCount);

	return refCount;
}

/// Decrement the object's internal counter
ULONG EndpointVolumeChangeHandler::Release() {

	// use the InterlockedDecrement function for thread safety
	const auto ulRefCount = InterlockedDecrement(&refCount);

	if (0 == refCount) {
		delete this;
	}

	return ulRefCount;
}

// -------------------------------------------------
// --- IAudioEndpointVolumeCallback Interface Functions ---
// ---------------- Overridable --------------------
// -------------------------------------------------

/// notifies the client that the volume level or muting state of the audio endpoint device has changed
HRESULT EndpointVolumeChangeHandler::OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify) {
	// default implementation
	return S_OK;
}