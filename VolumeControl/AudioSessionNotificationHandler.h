#pragma once

#include <Audiopolicy.h>

class AudioSessionNotificationHandler : public IAudioSessionNotification {
private:
	ULONG refCount;

public:
	AudioSessionNotificationHandler();
	~AudioSessionNotificationHandler();

	/// ------------------------------------
	/// --- IUnknown Interface Functions ---
	/// ------------------------------------

	HRESULT QueryInterface(const IID& riid, void** ppvObject) final;
	ULONG AddRef() final;
	ULONG Release() final;

	/// ------------------------------------------
	/// --- Overridable Default Implementation ---
	/// ------------------------------------------

	HRESULT OnSessionCreated(IAudioSessionControl* NewSession) override;
};

