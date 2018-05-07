#pragma once

#include <Audiopolicy.h>
#include "AudioSessionManager.h"

/// forward declare
class VolumeController;

class AudioSessionEventHandler : public IAudioSessionEvents
{
private:
	const VolumeController& volController;

	ULONG refCount;

	UINT observedSessionID;

public:
	explicit AudioSessionEventHandler(const VolumeController& volController, const UINT observedSessionID);
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

