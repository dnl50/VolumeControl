#pragma once

#include <Audiopolicy.h>
#include "SessionWrapper.h"

/// forward declare
class SessionWrapper;

class AudioSessionEventHandler : public IAudioSessionEvents {
private:
	const SessionWrapper& sessionWrapper;

	ULONG refCount;

public:
	explicit AudioSessionEventHandler(const SessionWrapper& wrapper);
	virtual ~AudioSessionEventHandler();

	/// ---------------------------
	/// --- Overriden Functions ---
	/// ---------------------------

	HRESULT QueryInterface(const IID& riid, void** ppvObject) final;
	ULONG AddRef() final;
	ULONG Release() final;

	/// -----------------------------------------------
	/// --- IAudioSessionEvents Interface Functions ---
	/// -----------------------------------------------

	HRESULT OnDisplayNameChanged(LPCWSTR NewDisplayName, LPCGUID EventContext) override;
	HRESULT OnIconPathChanged(LPCWSTR NewIconPath, LPCGUID EventContext) override;
	HRESULT OnSimpleVolumeChanged(float NewVolume, BOOL NewMute, LPCGUID EventContext) override;
	HRESULT OnChannelVolumeChanged(DWORD ChannelCount, float NewChannelVolumeArray[], DWORD ChangedChannel,
		LPCGUID EventContext) override;
	HRESULT OnGroupingParamChanged(LPCGUID NewGroupingParam, LPCGUID EventContext) override;
	HRESULT OnStateChanged(AudioSessionState NewState) override;
	HRESULT OnSessionDisconnected(AudioSessionDisconnectReason DisconnectReason) override;

};

