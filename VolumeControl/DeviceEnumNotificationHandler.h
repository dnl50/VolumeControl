#pragma once

#include <Mmdeviceapi.h> 


class DeviceEnumNotificationHandler : public IMMNotificationClient {
private:

	// counts the references to the object
	ULONG refCount;
	IMMDeviceEnumerator* deviceEnum;

public:
	DeviceEnumNotificationHandler();
	~DeviceEnumNotificationHandler();

	/// ---------------------------
	/// --- Overriden Functions ---
	/// ---------------------------

	HRESULT __stdcall QueryInterface(const IID& riid, void** ppvObject) override;
	ULONG __stdcall AddRef() override;
	ULONG __stdcall Release() override;

	/// ------------------------------------------
	/// --- Overridable Default Implementation ---
	/// ------------------------------------------

	virtual HRESULT __stdcall OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState);
	virtual HRESULT __stdcall OnDeviceAdded(LPCWSTR pwstrDeviceId);
	virtual HRESULT __stdcall OnDeviceRemoved(LPCWSTR pwstrDeviceId);
	virtual HRESULT __stdcall OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDefaultDeviceId);
	virtual HRESULT __stdcall OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key);

};

