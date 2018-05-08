#pragma once
#include <map>
#include <Audiopolicy.h>
#include "AudioSessionEventHandler.h"
#include "VolumeController.h"
#include <memory>
#include <vector>
#include "AudioSessionNotificationHandler.h"

/// forward declare
class VolumeController;
class AudioSessionEventHandler;

class AudioSessionManager : public AudioSessionNotificationHandler {

private:
	const VolumeController& volController;

	const std::shared_ptr<std::map<ULONG, IAudioSessionControl2*>> idMap;
	const std::shared_ptr<std::map<ULONG,AudioSessionEventHandler*>> eventHandlerMap;

	IAudioSessionManager2* sessionManager;
	IAudioSessionEnumerator* sessionEnum;
		
	ULONG currentID;

	ULONG getNextID();

	/// --- Private Functions ---

	// returns a smart pointer to a string containing the process name
	// throws a exception when it's not able to retrieve the name
	std::shared_ptr<std::wstring> getProcessNameByPID(DWORD pid) const;
		
	// add a new session and notify listeners
	HRESULT addSession(IAudioSessionControl* newSession);

public:
	explicit AudioSessionManager(VolumeController& volController);
	virtual ~AudioSessionManager();

	// returns a list of all currently known audio session IDs
	std::shared_ptr<std::vector<ULONG>> getSessionIDs() const;

	// returns the name of the audio session
	// throws a exception when it's not able to retrieve the name
	std::shared_ptr<std::wstring> getSessionNameByID(const ULONG id) const;

	// returns the object to control the volume
	IAudioSessionControl2* getSessionByID(ULONG id) const;

	// remove a audio session when it's no longer active
	void removeSessionByID(ULONG id) const;

	// release all references
	void shutdown(void) const;

	// gets called when a new session gets created
	HRESULT OnSessionCreated(IAudioSessionControl* NewSession) override;

	// get the IAudioSessionManager2 and the IAudioSessionEnumerator
	void init();

};

