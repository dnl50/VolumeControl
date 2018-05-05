#pragma once
#include <map>
#include <datetimeapi.h>
#include <Audiopolicy.h>
#include "AudioSessionEventHandler.h"
#include "VolumeController.h"
#include <memory>
#include <vector>


class AudioSessionManager
{
	
private:
	const VolumeController& volController;

	const std::shared_ptr<std::map<UINT, IAudioSessionControl2*>> idMap;
	const std::shared_ptr<std::map<UINT, AudioSessionEventHandler*>> eventHandlerMap;

	

	UINT currentID;

	UINT getNextID();

	/// --- Private Functions ---

	// returns a smart pointer to a string containing the process name
	// throws a exception when it's not able to retrieve the name
	std::shared_ptr<std::wstring> getProcessNameByPID(DWORD pid) const;

public:
	AudioSessionManager(VolumeController& volController);
	~AudioSessionManager();

	// returns a list of all currently known audio session IDs
	std::shared_ptr<std::vector<UINT>> getSessionIDs() const;

	// returns the name of the audio session
	// throws a exception when it's not able to retrieve the name
	std::shared_ptr<std::wstring> getSessionNameByID(UINT id) const;

	// returns the object to control the volume
	IAudioSessionControl2* getSessionByID(UINT id) const;

	// remove a audio session when it's no longer active
	void removeSessionByID(UINT id) const;

	// release all references
	void shutdown(void);

};

