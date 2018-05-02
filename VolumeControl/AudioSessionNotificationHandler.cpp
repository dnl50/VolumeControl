#include "stdafx.h"
#include "AudioSessionNotificationHandler.h"


/// Init the reference counter
AudioSessionNotificationHandler::AudioSessionNotificationHandler() : refCount(1UL) {

}

/// Use the default destructor
AudioSessionNotificationHandler::~AudioSessionNotificationHandler() = default;

/// ------------------------------------
/// --- IUnknown Interface Functions ---
/// ------------------------------------

// Return a pointer to the requested interface if this interface supports it.
HRESULT AudioSessionNotificationHandler::QueryInterface(const IID& riid, void** ppvObject) {

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
	else if (riid == __uuidof(IAudioSessionNotification)) {

		// Increment the reference count and return the pointer
		AddRef();

		*ppvObject = static_cast<IAudioSessionNotification*>(this);
	}
	else {
		*ppvObject = nullptr;

		return E_NOINTERFACE;
	}

	return S_OK;
}

// increment the object's internal counter
ULONG AudioSessionNotificationHandler::AddRef() {

	// use the InterlockedIncrement function for thread safety
	InterlockedIncrement(&refCount);

	return refCount;
}

// Decrement the object's internal counter
ULONG AudioSessionNotificationHandler::Release() {

	// use the InterlockedDecrement function for thread safety
	const auto ulRefCount = InterlockedDecrement(&refCount);

	if (0 == refCount) {
		delete this;
	}

	return ulRefCount;
}

/// ------------------------------------------
/// --- Overridable Default Implementation ---
/// ------------------------------------------

// Notifies the registered processes that the audio session has been created
HRESULT AudioSessionNotificationHandler::OnSessionCreated(IAudioSessionControl* NewSession) {

	/// default implementation

	return S_OK;
}