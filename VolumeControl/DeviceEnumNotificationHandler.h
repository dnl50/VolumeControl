#pragma once

#include <Mmdeviceapi.h> 


class DeviceEnumNotificationHandler : public IMMNotificationClient {
private:

	// counts the references to the object
	ULONG refCount;
	IMMDeviceEnumerator* deviceEnum;

public:
	DeviceEnumNotificationHandler();
	virtual ~DeviceEnumNotificationHandler();

	/// ---------------------------
	/// --- Overriden Functions ---
	/// ---------------------------

	HRESULT __stdcall QueryInterface(const IID& riid, void** ppvObject) final;
	ULONG __stdcall AddRef() final;
	ULONG __stdcall Release() final;

	/// ------------------------------------------
	/// --- Overridable Default Implementation ---
	/// ------------------------------------------

	virtual HRESULT __stdcall OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState);
	virtual HRESULT __stdcall OnDeviceAdded(LPCWSTR pwstrDeviceId);
	virtual HRESULT __stdcall OnDeviceRemoved(LPCWSTR pwstrDeviceId);
	virtual HRESULT __stdcall OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDefaultDeviceId);
	virtual HRESULT __stdcall OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key);

};

