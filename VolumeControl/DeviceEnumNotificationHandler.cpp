#include "stdafx.h"

#include "DeviceEnumNotificationHandler.h"
#include <iostream>

using std::cout;
using std::endl;

/// refere to: https://msdn.microsoft.com/de-de/library/windows/desktop/dd370810(v=vs.85).aspx

/// Init the reference counter
DeviceEnumNotificationHandler::DeviceEnumNotificationHandler(): refCount(1UL), deviceEnum(nullptr) {

}

/// Use the default destructor.
DeviceEnumNotificationHandler::~DeviceEnumNotificationHandler() = default;

// ------------------------------------
// --- IUnknown Interface Functions ---
// ------------------------------------

// Return a pointer to the requested interface if this interface supports it.
HRESULT DeviceEnumNotificationHandler::QueryInterface(const IID& riid, void** ppvObject) {
	
	// validate given pointer
	if (!ppvObject) {
		return E_INVALIDARG;
	}

	*ppvObject = nullptr;
	if (riid == IID_IUnknown) {
		
		// Increment the reference count and return the pointer
		AddRef();
		
		*ppvObject = static_cast<IUnknown*>(this);
		
	} else if(riid == __uuidof(IMMNotificationClient)) {
		
		// Increment the reference count and return the pointer
		AddRef();

		*ppvObject = static_cast<IMMNotificationClient*>(this);		
	}
	else {
		*ppvObject = nullptr;

		return E_NOINTERFACE;
	}

	return S_OK;
}

// increment the object's internal counter
ULONG DeviceEnumNotificationHandler::AddRef() {	
	
	// use the InterlockedIncrement function for thread safety
	InterlockedIncrement(&refCount);

	return refCount;
}

// Decrement the object's internal counter
ULONG DeviceEnumNotificationHandler::Release() {
	
	// use the InterlockedDecrement function for thread safety
	const auto ulRefCount = InterlockedDecrement(&refCount);

	if (0 == refCount) {
		delete this;
	}

	return ulRefCount;
}


// -------------------------------------------------
// --- IMMNotificationClient Interface Functions ---
// ---------------- Overridable --------------------
// -------------------------------------------------

/// Indicates that the state of an audio endpoint device has changed
HRESULT DeviceEnumNotificationHandler::OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState) {
	return S_OK;
}

/// Indicates that a new audio endpoint device has been added
HRESULT DeviceEnumNotificationHandler::OnDeviceAdded(LPCWSTR pwstrDeviceId) {
	return S_OK;
}

/// Indicates that an audio endpoint device has been removed
HRESULT DeviceEnumNotificationHandler::OnDeviceRemoved(LPCWSTR pwstrDeviceId) {
	return S_OK;
}

/// Notifies the client that the default audio endpoint device for a particular role has changed
HRESULT DeviceEnumNotificationHandler::OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDefaultDeviceId) {
	return S_OK;
}

/// Indicates that the value of a property belonging to an audio endpoint device has changed
HRESULT DeviceEnumNotificationHandler::OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key) {
	return S_OK;
}