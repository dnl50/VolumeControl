#pragma once
#include <Endpointvolume.h>

class EndpointVolumeChangeHandler : public IAudioEndpointVolumeCallback {

private:
	// counts the references to the object
	ULONG refCount;

public:
	EndpointVolumeChangeHandler();
	virtual ~EndpointVolumeChangeHandler();

	// ---------------------------
	// --- Overriden Functions ---
	// ---------------------------

	HRESULT QueryInterface(const IID& riid, void** ppvObject) final;
	ULONG AddRef() final;
	ULONG Release() final;

	// ------------------------------------------
	// --- Overridable Default Implementation ---
	// ------------------------------------------

	virtual HRESULT OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify);
};

